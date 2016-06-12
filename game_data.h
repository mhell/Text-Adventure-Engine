//
//  game_data.h
//  Lab3
//
//  Created by Mattias Hellman on 8/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_GAME_DATA
#define LAB3_GAME_DATA

namespace lab3  {
	
	class Environment;
	class Actor;
	
	class Game_data {
	public:
		virtual Environment* get_current_environment() const = 0;
		
		virtual void set_current_environment(const std::string &name) = 0;
		
		virtual Environment* get_environment(const std::string &name) const = 0;
		
		virtual const std::string& get_prev_environment_name() const = 0;
        
		virtual const std::vector<Actor*>& get_playables() const = 0;
		
		virtual const std::vector<Actor*>& get_enemies() const = 0;
		
		virtual Actor* get_playable(const std::string &name) = 0;
		
		virtual Actor* get_enemy(const std::string &name) = 0;
		
		virtual const std::string& get_time_string() const = 0;
		
		virtual const std::string& get_long_time_string() const = 0;
		
		virtual void update() = 0;
		
	private:
		Game_data& operator=(const Game_data&); // prevent slicing
		
	};
	
}

#endif