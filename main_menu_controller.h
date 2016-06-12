//
//  main_menu_controller.h
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_MAIN_MENU_CONTROLLER
#define LAB3_MAIN_MENU_CONTROLLER

#include <iostream>
#include "controller.h"

namespace lab3  {

	class Main_menu_controller : public Controller {
	public:
		using Controller::Controller; // Inherit constructors
		
		virtual bool update(const std::string &commands) final override;
		
		virtual void became_primary_impl() final override;
		
		virtual void left_primary_impl() final override;
		
		virtual void re_became_primary() final override;
		
	private:
		typedef bool (Main_menu_controller::*Command_ptr)(const std::string&, const std::string&);
		
		std::unordered_map<std::string, Command_ptr> m_commands;
		
		/**
		 * Create new game
		 */
		bool new_game(const std::string &unused = "", const std::string &unused2 = "");
		
		/**
		 * Load a game
		 */
		bool load_game(const std::string &file_path = "", const std::string &unused = "");
		
		/**
		 * Exit the program
		 */
		bool quit(const std::string &unused = "", const std::string &unused2 = "");
	};
	
}

#endif
