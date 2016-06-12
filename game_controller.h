//
//  game_controller.h
//  Lab3
//
//  Created by Mattias Hellman on 8/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_GAME_CONTROLLER
#define LAB3_GAME_CONTROLLER

#include <iostream>
#include "controller.h"
#include "game_delegate.h"
#include "world.h"

namespace lab3 {
    
    class Game_controller : public Controller, public Game_delegate {
    public:
        Game_controller(Application &app, View &view, std::unique_ptr<World> world);
		
		virtual bool update(const std::string &commands) final override;
		
		virtual void became_primary_impl() final override;
		
		virtual void left_primary_impl() final override;
		
		virtual void re_became_primary() final override;
		
		virtual void entered_environment(const std::string &name) final override;
		
		virtual void show_examination(const Game_object &caller, const Game_object &item, const String_map &singles = {}, const String_map_map &mapables = {}, const String_map_vec &lists = {}) const final override;
        
		virtual void show_messege(const std::string &messege = "", bool line_break = true) const final override;
        
        void game_over(const std::string &messege, const Game_object &winner);
        
        void check_game_state();
		
	private:
		std::unique_ptr<World> m_world;
		
		typedef bool (Game_controller::*Command_ptr)(const std::string&, const std::string&);

		std::unordered_map<std::string, Command_ptr> m_commands;
        
        bool game_round_on = false;
        
        /**
         * Load a game
         */
        bool load_game(const std::string &file_path = "", const std::string &unused = "");
        
        /**
         * Save a game
         */
        bool save_game(const std::string &file_path = "", const std::string &unused = "");
       
        /**
         * Create new game
         */
        bool new_game(const std::string &unused = "", const std::string &unused2 = "");

        /**
         * Exit the program
         */
        bool quit(const std::string &unused = "", const std::string &unused2 = "");
        
        /**
         * Displayy help text
         */
        bool display_help(const std::string &command = "", const std::string &unused = "");
		
		void print_env_description(const std::string &name) const;
		
		bool is_round_over() const;
    };
    
}

#endif
