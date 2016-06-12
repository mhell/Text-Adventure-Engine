//
//  Application.cpp
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <sstream>
#include <algorithm>
#include <ncurses.h>
#include "application.h"
#include "controller.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Initialization

Application::Application() {
	setlocale(LC_ALL, "");
	initscr();
}


#pragma mark - Main game loop

void Application::run() {
	
	while (true) {
		bool command_found = false;
		
		const auto &controller = *(m_controller_stack.back());
		
		// get input command
		std::string input = controller.get_view().prompt_user();
		
		// convert input to lowercase
		Utils::to_lower(input);
		
		// send input to controllers. Top of stack first then downward if not responding
		// note: since this is a text adventure an update or a 'tick' constitutes a command line input
		for (auto i = m_controller_stack.size()-1; i < m_controller_stack.size() && !command_found; --i)
			command_found = m_controller_stack[i]->update(input);
        
        /*
        if (std::find_if (m_controller_stack.begin (), m_controller_stack.end (), [=](decltype(m_controller_stack)::value_type const& elem) { return elem->update (input);}) == m_controller_stack.end ())
            throw 123;
		*/
        
		// try running the command locally if no controller accepts the input
		if (!command_found) {
			std::istringstream ss(input);
			std::string first, second, third;
			std::getline(ss, first, ':');
			std::getline(ss, second, ':');
			std::getline(ss, third, '\0');
			
			auto it = m_commands.find(Utils::trim(first));
			
			if(it != m_commands.end()) {
				(this->*(it->second))(Utils::trim(second), Utils::trim(third));
			} else if (first.length()) {
				// command not found; print error messege in current view
				controller.get_view().render_text("\nCommands or arguments unknown.\n");
			}
		}
	}
	
	clear();
}


#pragma mark - Controller stack

void Application::push_controller(std::unique_ptr<Controller> controller) {
	Controller *prev = m_controller_stack.empty() ? nullptr : m_controller_stack.back().get();
	
	m_controller_stack.push_back(std::move(controller));
	
	m_controller_stack.back()->became_primary();
	
	if (prev) prev->left_primary();
}

std::unique_ptr<Controller> Application::pop_controller() {
	if (m_controller_stack.empty()) {
		return nullptr;
	} else {
		std::unique_ptr<Controller> top = std::move(m_controller_stack.back());
		
		m_controller_stack.pop_back();
		
		top->left_primary();
		
		if (!m_controller_stack.empty())
			m_controller_stack.back()->became_primary();
		
		return top;
	}
}


#pragma mark - Command methods

bool Application::quit(const std::string& unused, const std::string& unused2) {
	endwin();
	std::exit(0);
}


#pragma mark - destructor

Application::~Application() {
    endwin();
}

