//
//  JSON_tools.h
//  Lab3
//
//  Created by Mattias Hellman on 18/02/14.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_JSON_TOOLS
#define LAB3_JSON_TOOLS

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

namespace lab3 {
    
    class JSON_tools {
    public:
        /**
         * Removes num intendations from a json declaration
         */
        static std::string remove_indentation(const std::string &json, int num = 1);
        
        /**
         * adds num intendationa from a json declaration
         */
        static std::string add_indentation(const std::string &json, int num = 1);
        
        /**
         * Finds value of key in root of json definition
         */
        static std::string find_value_in_object(const std::string& json, const std::string& key);
        
        /**
         * Find all key-values in a json object and store these in a map. New keys is inserted only if the key is not already present in the map.
         */
        static std::pair<std::string, std::string> to_std_map(const std::string &json, std::unordered_map<std::string, std::string> &map, bool remove_indentations = true, bool remove_quotes = true);
        
        /**
         * Converts a std unordered_map<string, string> to a json object ({...})
         */
        static std::string to_json_object(const std::unordered_map<std::string, std::string> &map);
		
		/**
         * Converts a std unordered_map<string, int> to a json object ({...})
         */
        static std::string to_json_object(const std::unordered_map<std::string, int> &map);
        
		/**
		 * merges two json definitions (top-level objects with option to merge second-level array/objects). new_def has precedence
		 */
		static std::string merge_json(const std::string &original_def, const std::string &new_def, bool merge_sec_layer = 0);
		
        /**
         * Merges two json objects. new_def has precedence
         */
        static std::string merge_object(const std::string &original_def, const std::string &new_def);
        
		/**
		 * merges two json arrays. new_def has precedence
		 */
		static std::string merge_array(const std::string &original_def, const std::string &new_def);
		
        /**
         * Returns all objects in a json definition, including nested objects
         */
        static void get_objects(const std::string &json, std::vector<std::string>& result);
      
		/**
		 * Returns true if the element is an json object
		 */
		static bool is_json_object(const std::string &json);
		
        /**
         * Returns true if the argument is an json array
         */
        static bool is_json_array(const std::string &json);
        
        /**
         * Returns a vector converted from an json array
         */
        static std::vector<std::string> to_std_vector(const std::string &json, bool remove_quotes = true);
        
		/**
		 * returns a set of values from an json array
		 */
		static std::set<std::string> to_std_set(const std::string &json);
			
        /**
         * Returns a json array converted from a std vector of strings
         */
        static std::string to_json_array(const std::vector<std::string> &vec);
        
		/**
		 * Returns a json array converted from a std set of strings
		 */
		static std::string to_json_array(const std::set<std::string> &set);
		
		/**
         * Replaces all occurrences of a word
         */
		static std::string replace(const std::string &text, const std::string &old_word, const std::string &new_word);
			
        /**
         * Replaces a single value in a json object
         */
        static std::string replace_value_in_object(const std::string &json, const std::string &key, const std::string &new_value);
        
        /**
         * Strips away double qoutes in strings
         */
        static std::string to_std_string(const std::string &json);
        
        /**
         * Adds double qoutes in strings
         */
        static std::string to_json_string(const std::string &std);
    };

}

#endif
