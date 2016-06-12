//
//  utils.cpp
//  Lab3
//
//  Created by Mattias Hellman on 23/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <regex>
#include "utils.h"

using namespace lab3;


std::string& Utils::to_upper(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}

std::string Utils::to_upper(const std::string &s) {
	std::string result = s;
	std::transform(s.begin(), s.end(), result.begin(), toupper);
	return result;
}

std::string& Utils::to_lower(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}

std::string Utils::to_lower(const std::string &s) {
	std::string result = s;
	std::transform(s.begin(), s.end(), result.begin(), tolower);
	return result;
}

std::string& Utils::uncapitalize_word(std::string &s) {
    if (s.length()) {
        s[0] = tolower(s[0]);
    }
    return s;
}

std::string Utils::uncapitalize_word(const std::string &s) {
    std::string result = s;
    if (result.length()) {
        result[0] = tolower(result[0]);
    }
    return result;
}

std::string& Utils::capitalize_word(std::string &s) {
	if (s.length()) {
		s[0] = toupper(s[0]);
	}
	return s;
}

std::string Utils::capitalize_word(const std::string &s) {
	std::string result = s;
	if (result.length()) {
		result[0] = toupper(result[0]);
	}
	return result;
}

std::string& Utils::capitalize(std::string &s) {
	std::smatch m;
	std::string remains(s);
	std::regex exp("\\b\\.\\s(.)");
	
	while (std::regex_search (remains, m, exp)) {
		s += m.prefix().str();
		s += ". " + Utils::to_upper(m[1].str());
		remains = m.suffix().str();
	}
	
	return s += remains;
}

std::string Utils::capitalize(const std::string &s) {
	std::string result = s;
	std::smatch m;
	std::string remains(result);
	std::regex exp("\\b\\.\\s(.)");
	
	while (std::regex_search (remains, m, exp)) {
		result += m.prefix().str();
		result += ". " + Utils::to_upper(m[1].str());
		remains = m.suffix().str();
	}
	
	return result += remains;
}

std::string Utils::trim(const std::string &s) {
	return std::regex_replace(s, std::regex("^\\s+|\\s$+"), "");
}


std::string Utils::add_punktuation(const std::string &s, char punctuation) {
    std::smatch m;
    std::regex exp("\\w$");
    
    if (std::regex_search (trim(s), m, exp)) {
        return std::string({punctuation});
    } else {
        return "";
    }
}


