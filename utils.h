//
//  utils.h
//  Lab3
//
//  Created by Mattias Hellman on 23/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_UTILS
#define LAB3_UTILS

#include <iostream>

namespace lab3 {

	class Utils {
	public:
		/**
		 * Convert a string to upper case
		 */
		static std::string& to_upper(std::string &s);
		static std::string to_upper(const std::string &s);
		
		/**
		 * Convert a string to lower case
		 */
		static std::string& to_lower(std::string &s);
		static std::string to_lower(const std::string &s);
		
        /**
         * Un-capitalizes first letter in a sentence
         */
        static std::string& uncapitalize_word(std::string &s);
        static std::string uncapitalize_word(const std::string &s);
        
		/**
		 * Capitalizes first letters in a sentence
		 */
		static std::string& capitalize(std::string &s);
		static std::string capitalize(const std::string &s);
		
		/**
		 * Capitalizes first letter in a string
		 */
		static std::string& capitalize_word(std::string &s);
		static std::string capitalize_word(const std::string &s);
		
		/**
		 * Removes leading and trailing spaces from a string
		 */
		static std::string trim(const std::string &s);
        
        /**
         * Returns a punktuation if needed
         */
        static std::string add_punktuation(const std::string &s, char punctuation);
	};
	
}

#endif
