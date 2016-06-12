//
//  main_menu_controller.cpp
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//


#include <sstream>
#include "main_menu_controller.h"
#include "globals.h"
#include "game_builder.h"
#include "game_controller.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Update

bool Main_menu_controller::update(const std::string& commands) {
	std::stringstream ss(commands);
	std::string first, second, third;
	std::getline(ss, first, ':');
	std::getline(ss, second, ':');
	std::getline(ss, third, '\0');
	
	auto it = m_commands.find(Utils::trim(first));
	
	// run the command if found
	if(it != m_commands.end()) {
		(this->*(it->second))(Utils::trim(second), Utils::trim(third));
		return true;
	}
	
	// no commands recognized
	return false;
}


#pragma mark - Controller implementation

void Main_menu_controller::became_primary_impl() {
	// print splash & menu
	get_view().clear_screen();
	get_view().render_text_from_file("splash.ascii");
	get_view().render_text("[1] New Game\n[2] Load Game\n[0] Exit\n");
	
	// init command methods
	m_commands = { {"0", &Main_menu_controller::quit},
				   {"1", &Main_menu_controller::new_game},
				   {"2", &Main_menu_controller::load_game} };
}

void Main_menu_controller::left_primary_impl() {
	// init command methods (new keys)
	m_commands = { {"quit", &Main_menu_controller::quit},
				   {"new", &Main_menu_controller::new_game}};
}

void Main_menu_controller::re_became_primary() {
    became_primary_impl();
}


#pragma mark - Command methods

bool Main_menu_controller::new_game(const std::string& unused, const std::string& unused2) {
	get_application().push_controller(std::unique_ptr<Game_controller>(new Game_controller(get_application(), get_view(), Game_builder::load(DATA_PATH))));
	return true;
}

bool Main_menu_controller::load_game(const std::string &file_path, const std::string &unused) {
    std::string path = file_path;
    try {
        // promt user for file path if not given
        if (!file_path.length()) {
            get_view().render_text("\nEnter savegame name:  ");
            path = get_view().prompt_user("");
        }
        get_application().push_controller(std::unique_ptr<Game_controller>(new Game_controller(get_application(), get_view(), Game_builder::load(Utils::trim(path)))));
        return true;
    } catch (std::runtime_error &e) {
        get_view().render_text("\nSavegame '" + path + "' not found\n");
    }
	
	return false;
}

bool Main_menu_controller::quit(const std::string &unused, const std::string &unused2) {
	endwin();
	std::exit(0);
}




