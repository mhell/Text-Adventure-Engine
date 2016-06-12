//
//  JSON_tools.cpp
//  Lab3
//
//  Created by Mattias Hellman on 18/02/14.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <regex>
#include <sstream>
#include "json_tools.h"

using namespace lab3;


std::string JSON_tools::remove_indentation(const std::string &json, int num) {
    std::string result(json);
    for (/*empty*/; num > 0; --num) {
        std::regex exp("\\n\\t");
        result = std::regex_replace(result, exp, "\n");
    }
    return result;
}

std::string JSON_tools::add_indentation(const std::string &json, int num) {
    std::string result(json);
    for (/*empty*/; num > 0; --num) {
        std::regex exp("\\n");
        result = std::regex_replace(result, exp, "\n\t");
    }
    return result;
}

std::string JSON_tools::find_value_in_object(const std::string& json, const std::string& key) {
    std::smatch match;
    std::string pre("\\n\\t\"\\b"+key+"\"\\s+:\\s+");
    std::string single_value("\"?([^\\n^,^\\[^\"]+)\"?");
    std::string object_or_array("([\\[\\{][\\s\\S]+?\\n\\t[\\]\\}])");
    
    std::regex exp("(?:"+pre+object_or_array+")|(?:"+pre+single_value+")");
    
    return (std::regex_search(json, match, exp)) ? (match[1].length() ? remove_indentation(match[1].str()) : remove_indentation(match[2].str()) ) : "";
}

std::pair<std::string, std::string> JSON_tools::to_std_map(const std::string &json, std::unordered_map<std::string, std::string> &map, bool remove_indentations, bool remove_quotes) {
	std::smatch m;
    std::pair<std::string, std::string> pre_suf;
    std::string remains(json);
    
    std::string pre("\\n\\t(\"\\b[^\"]+\")\\s+:\\s+");
    std::string single_value("(\"[^\\n^\\[]+\"|\\d+)");
    std::string object_or_array("([\\[\\{][\\s\\S]*?\\n\\t[\\]\\}])");
    
    std::regex e("("+pre+object_or_array+")|("+pre+single_value+")");
    
    while (std::regex_search (remains, m, e)) {
        // get prefix
        if (!pre_suf.first.length()) pre_suf.first = m.prefix().str();
        
        // get suffix
        pre_suf.second = m.suffix().str();
        
        if (m[1].length()) {
			if (remove_quotes) {
				map.insert({to_std_string(m[2]), (remove_indentations) ? remove_indentation(to_std_string(m[3])) : to_std_string(m[3])});
			} else {
				map.insert({m[2], (remove_indentations) ? remove_indentation(m[3]) : m[3]});
			}
        } else if (m[4].length()) {
			if (remove_quotes) {
				map.insert({to_std_string(m[5]), to_std_string(m[6])});
			} else {
				map.insert({m[5], m[6]});
			}
        }
        
        // try rest for more matches
        remains = m.suffix().str();
    }
	
    return pre_suf;
}

std::string JSON_tools::to_json_object(const std::unordered_map<std::string, std::string> &map) {
    std::string result("{");
    
    for (auto elem : map) {
        // add qoutes only when single values (not object or array)
        result += "\n\t" + to_json_string(elem.first) + " : " + (elem.second[0] == '[' || elem.second[0] == '{' ? add_indentation(elem.second) + "," : to_json_string(elem.second) + ",");
    }
    
    if (result.length() > 1) result.pop_back(); // remove last ","
    
    result += "\n}";
    
    return result;
}

std::string JSON_tools::to_json_object(const std::unordered_map<std::string, int> &map) {
    std::string result("{");
	std::ostringstream convert;
    
    for (auto elem : map) {
		convert << elem.second;
        result += "\n\t" + to_json_string(elem.first) + " : " + add_indentation(convert.str()) + ",";
		convert.str("");
		convert.clear();
    }
    
    if (result.length() > 1) result.pop_back(); // remove last ","
    
    result += "\n}";
    
    return result;
}

std::string JSON_tools::merge_json(const std::string &original_def, const std::string &new_def, bool merge_sec_layer) {
    // skapa std::map object av båda
	std::unordered_map<std::string, std::string> map_orig;
	to_std_map(original_def, map_orig, true, true);
	std::unordered_map<std::string, std::string> map_new;
	to_std_map(new_def, map_new, true, true);
	
    // insert new definitions in old
	for (auto &pair : map_new) {
		auto result = map_orig.insert(pair);
		
		// element existed (insertion failure),
		if (!result.second) {
            // manually merge original value with new value
            if (merge_sec_layer) {
                if (is_json_object(pair.second)) {
                    map_orig[pair.first] = merge_object(map_orig[pair.first], pair.second);
                } else if (is_json_array(pair.second)) {
                    map_orig[pair.first] = merge_array(map_orig[pair.first], pair.second);
                } else {
                    map_orig[pair.first] = pair.second;
                }
            // replace original value with new value
            } else {
                 map_orig[pair.first] = pair.second;
            }
		}
	}
    
    return to_json_object(map_orig);
}

std::string JSON_tools::merge_object(const std::string &original_def, const std::string &new_def) {
    std::string result;
    std::unordered_map<std::string, std::string> map;
    
    // combine new_def and original_def into same map (new is added first, orig does not overwrite)
    std::pair<std::string, std::string> pre_suf = to_std_map(new_def, map, false, false);
    to_std_map(original_def, map, false, false);
    
    // add prefix to result string
    result += pre_suf.first;
    
    // add all attributes (key-value) to result string
    for (auto element : map) {
        result += "\n\t"+element.first+" : "+element.second;
    }
    
    // add suffix to result string
    result += pre_suf.second;
    
    return result;
}

std::string JSON_tools::merge_array(const std::string &original_def, const std::string &new_def) {
	// convert to std::vector
	auto set_orig = to_std_set(original_def);
	auto set_new = to_std_set(new_def);
	set_orig.insert(set_new.begin(), set_new.end());
	
	// convert back to json array and return
	return to_json_array(set_orig);
}

void JSON_tools::get_objects(const std::string &json, std::vector<std::string>& result) {
    // copy input string
	std::string json_copy = json;
	
	// add the json to the result
    result.push_back(json_copy);
    
    // find all objects in the json
    std::smatch m;
    std::string remains(json_copy);
    std::string key("\\n\\t\"\\b[^\"]+\"\\s:\\s");
    std::string object(key+"(\\{[\\s\\S]+?\\n\\t\\})");
    std::string object_in_array("(\\n\\t\\t\\{[\\s\\S]+?\\n\\t\\t\\})");
    std::regex exp(object+"|"+object_in_array);
    
    while (std::regex_search (remains, m, exp)) {
        // remove excessive tabs in the match (an object of array or an object in root)
        json_copy = m[2].length()>0 ? remove_indentation(m[2].str(), 2) : remove_indentation(m[1].str());
        
        // recursively find objects in the match
        get_objects(json_copy, result);
        
        // try remains for more matches
        remains = m.suffix().str();
    }
}

std::set<std::string> JSON_tools::to_std_set(const std::string &json) {
    std::set<std::string> result;
    
    // find objects
    std::smatch m;
    std::string remains(json);
    //std::regex exp("\\n\\t(\\{[\\s\\S]+?\\n\\t\\})");
	std::string pre("\\n\\t");
    std::string single_value("([^\\n^,^\\[]+)");
    std::string object_or_array("([\\[\\{][\\s\\S]*?\\n\\t[\\]\\}])");
    std::regex exp(pre+object_or_array+"|"+pre+single_value);
	
    while (std::regex_search (remains, m, exp)) {
        result.emplace(m[1].length()>0 ? remove_indentation(m[1]) : m[2]);
		
        // try remains for more matches
        remains = m.suffix().str();
    }
    
    return result;
}

std::vector<std::string> JSON_tools::to_std_vector(const std::string &json, bool remove_quotes) {
    std::vector<std::string> result;
    
    // find objects
    std::smatch m;
    std::string remains(json);
    //std::regex exp("\\n\\t(\\{[\\s\\S]+?\\n\\t\\})");
	std::string pre("\\n\\t");
    std::string single_value("([^\\n^,^\\[]+)");
    std::string object_or_array("([\\[\\{][\\s\\S]*?\\n\\t[\\]\\}])");
    std::regex exp(pre+object_or_array+"|"+pre+single_value);
 
    while (std::regex_search (remains, m, exp)) {
        result.push_back(m[1].length()>0 ? remove_indentation(m[1]) : remove_quotes ? to_std_string(m[2]) : m[2]);
		
        // try remains for more matches
        remains = m.suffix().str();
    }
    
    return result;
}

std::string JSON_tools::to_json_array(const std::vector<std::string> &vec) {
    std::string result("[");
    
    for (std::string elem : vec) {
        result += "\n\t" + (elem[0] == '[' || elem[0] == '{' ? add_indentation(elem) + "," : to_json_string(elem) + ",");
    }
    
    if (result.length() > 1) result.pop_back(); // remove last ","
    
    result += "\n]";
    
    return result;
}

std::string JSON_tools::to_json_array(const std::set<std::string> &set) {
    std::string result("[");
    
    for (std::string elem : set) {
        result += "\n\t" + add_indentation(elem) + ",";
    }
    
    if (result.length() > 1) result.pop_back(); // remove last ","
    
    result += "\n]";
    
    return result;
}

bool JSON_tools::is_json_object(const std::string &json) {
    return std::regex_match(json, std::regex("\\{[\\s\\S]*[^\\t]\\}[\\n\\s]*"));
}

bool JSON_tools::is_json_array(const std::string &json) {
    return std::regex_match(json, std::regex("\\[[\\s\\S]*[^\\t]\\][\\n\\s]*"));
}

std::string JSON_tools::replace(const std::string &text, const std::string &old_word, const std::string &new_word) {
    std::regex exp(old_word);
    return std::regex_replace(text, exp, new_word);
}

std::string JSON_tools::replace_value_in_object(const std::string &json, const std::string &key, const std::string &new_value) {
    std::regex exp("\\n\\t\""+key+"\"\\s:\\s.+");
    return std::regex_replace(json, exp, "\n\t\""+key+"\" : "+new_value);
}

std::string JSON_tools::to_std_string(const std::string &json) {
    std::regex exp("(?:^\")|(?:\"$)");
    return std::regex_replace(json, exp, "");
}

std::string JSON_tools::to_json_string(const std::string &std) {
    return "\"" + std + "\"";
}







