//
//  string_replacer.h
//  Lab3
//
//  Created by Mattias Hellman on 24/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_STRING_REPLACER
#define LAB3_STRING_REPLACER

#include <iostream>
#include <unordered_map>
#include <string>

namespace lab3 {
	
	class String_replacer {
	public:
		/**
		 * Default constructor
		 */
		String_replacer() = default;
		
		/**
		 * Constructor that takes a replacement map
		 */
		String_replacer(const std::unordered_map<std::string, std::string> &replacements);
		
		/**
		 * Map strings to be replaced with its replacement string
		 */
		void add_replacement(const std::string &key, const std::string &replacement);
		
		/**
		 * Replace occurences of mapped keys with mapped strings in input
		 */
		std::string replace(const std::string &s);
		
		/**
		 * Assign a replacement map
		 */
		void set_replacements(const std::unordered_map<std::string, std::string> &replacements);
		
	private:
		std::unordered_map<std::string, std::string> m_replacements;
		
	};
	
}

#endif
