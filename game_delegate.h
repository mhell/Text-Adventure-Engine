//
//  game_event_delegate.h
//  Lab3
//
//  Created by Mattias Hellman on 7/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_GAME_DELEGATE
#define LAB3_GAME_DELEGATE

#include <iostream>
#include <unordered_map>
#include <vector>

namespace lab3  {
	
	class Game_object;
    
    class Game_delegate {
    public:
		using String_map = std::unordered_map<std::string, std::string>;
		using String_map_map = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
		using String_map_vec = std::unordered_map<std::string, std::vector<std::string>>;
		
		virtual void entered_environment(const std::string &name) = 0;
        virtual void show_examination(const Game_object &caller, const Game_object &item, const String_map &singles, const String_map_map &mapables, const String_map_vec &lists) const = 0;
        virtual void show_messege(const std::string &messege = "", bool line_break = true) const = 0;
		
	private:
		Game_delegate& operator=(const Game_delegate&); // prevent slicing
		
    };

}

#endif
