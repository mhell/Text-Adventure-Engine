//
//  Application.h
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_APPLICATION
#define LAB3_APPLICATION

#include <iostream>
#include <vector>
#include <unordered_map>

namespace lab3  {
	
	class Controller;

	class Application {
	public:
		Application();
		
		/**
		 * Start the main loop
		 */
		void run();
		
		/**
		 * Push a scene to the top of the stack
		 */
		void push_controller(std::unique_ptr<Controller> controller);
		
		/**
		 *	Pop the top scene from the stack and return it
		 */
		std::unique_ptr<Controller> pop_controller();
		
		/**
		 * Exit the program
		 */
		bool quit(const std::string &unused = "", const std::string &unused2 = "");
		
		~Application();
		
	private:
		std::vector<std::unique_ptr<Controller>> m_controller_stack;
		
		typedef bool (Application::*Command_ptr)(const std::string&, const std::string&);
		
		std::unordered_map<std::string, Command_ptr> m_commands{ {"quit", &Application::quit} };
		
	};
	
}


#endif
