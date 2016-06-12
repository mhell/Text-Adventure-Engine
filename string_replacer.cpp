//
//  string_replacer.cpp
//  Lab3
//
//  Created by Mattias Hellman on 24/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <regex>
#include "string_replacer.h"

using namespace lab3;


String_replacer::String_replacer(const std::unordered_map<std::string, std::string> &replacements) :
m_replacements(replacements) {
	
}

void String_replacer::add_replacement(const std::string &key, const std::string &replacement) {
	m_replacements[key] = replacement;
}

std::string String_replacer::replace(const std::string &s) {
	std::string result = s;
	std::regex exp;
	
	// for each entry in m_replacements; replace occurence of key with mapped value
	for (auto &pair : m_replacements) {
		result = std::regex_replace(result, std::regex(pair.first), pair.second);
	}
	
	return result;
}

void String_replacer::set_replacements(const std::unordered_map<std::string, std::string> &replacements) {
	m_replacements = replacements;
}