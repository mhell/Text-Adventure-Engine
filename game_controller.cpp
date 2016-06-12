//
//  game_controller.cpp
//  Lab3
//
//  Created by Mattias Hellman on 8/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <memory>
#include <chrono>
#include <thread>
#include "game_controller.h"
#include "game_builder.h"
#include "environment.h"
#include "exterior.h"
#include "utils.h"
#include "globals.h"

using namespace lab3;


#pragma mark - Constructor

Game_controller::Game_controller(Application &app, View &view, std::unique_ptr<World> world) :
m_world(std::move(world)), Controller(app, view) {
}


#pragma mark - Update

bool Game_controller::update(const std::string &commands) {
	bool npc_turn = false;
	std::stringstream commands_ss(commands);
	std::string command_call;
	unsigned tick = 0;
	
    // alternate between user actions (the input commands separated by ',') and npc actions
	do {
		if (npc_turn) {
			m_world->action(tick++);
		} else {
			// if any commands remains
			if (!commands_ss.eof()) {
				bool success = false;
				std::getline(commands_ss, command_call, ',');
				
				// divide command call into its seperate parts
				std::stringstream parts_ss(command_call);
				std::string command, argument, remains;
				std::getline(parts_ss, command, ':');
				std::getline(parts_ss, argument, ':');
				std::getline(parts_ss, remains, '\0');
				
				auto it = m_commands.find(Utils::trim(command));
				
				// run the command if found
				if(it != m_commands.end()) {
					success = (this->*(it->second))(Utils::trim(argument), Utils::trim(remains));
                // else send to world as interact command
				} else {
					// examine, exit should always be run on current_environment. Does not trigger game round
					if (Utils::trim(command) == "examine" || Utils::trim(command) == "exit") {
						success = m_world->get_current_environment()->interact(command_call);
                    // else send to a playable (command should be the name of a playable actor e.g. "Tom")
					} else {
						Actor *playable = m_world->get_playable(Utils::trim(command));
						if (playable) {
							success = playable->interact(Utils::trim(argument) + " : " + Utils::trim(remains));
                        
							// start game round
                            //game_round_on = true;
                        } else if (!game_round_on) {
                            // abort if game_round not yet started (let lower controller handle command)
                            return false;
                        }
					}
				}
				
				// command not found; print error messege in current view
				if (!success) {
					get_view().render_text("\nCommands or arguments unknown (" + Utils::trim(command_call) + ").\n");
				}
			}
		}
		
		if (game_round_on) {
			// check game state
			if (is_round_over()) game_round_on = false;
			
			// toggle npc/user turn
			npc_turn = !npc_turn; // player först och sätt npc turn om ingen command
			
			// add delay
			std::this_thread::sleep_for(std::chrono::milliseconds(TURN_DELAY_MS));
			
			get_view().render_text("\nhoj\n");
		}
        
        // check if won/game over
        check_game_state();
	} while (game_round_on);
	
	// error handling managed locally
	return true;
}


#pragma mark - Controller implementation

void Game_controller::became_primary_impl() {
	// set game delegate
	m_world->set_delegate(this);
	
	// init command methods
    m_commands = { {"help", &Game_controller::display_help},
                   {"load", &Game_controller::load_game},
                   {"save", &Game_controller::save_game},
                   {"new", &Game_controller::new_game},
                   {"quit", &Game_controller::quit} };
	
	// show intro
	get_view().render_text_from_file("intro.txt", true);
	get_view().render_text("\nPress Enter to continue.  ");
	get_view().prompt_user("");
	
	// activate the world and display start environment's scene description
	m_world->set_active(true);
}

void Game_controller::left_primary_impl() {
	
}

void Game_controller::re_became_primary() {
	
}


#pragma mark - Game_delegate implementation

void Game_controller::entered_environment(const std::string &name) {// print artwork
	// print environment artwork & descriptions
	print_env_description(name);
	
	// write prompt text
	get_view().render_text(" What do you do?\n");
	
}

void Game_controller::show_examination(const Game_object &caller, const Game_object &item, const String_map &singles, const String_map_map &mapables, const String_map_vec &lists) const {
	// Om owner ändras skriv ut role_in_owner (+owner?)
	
	// print callers name
	auto &caller_name = caller.get_name();
	get_view().render_text("\n" + Utils::to_upper(caller_name) + "\n", /*A_UNDERLINE |*/ A_BOLD);
	
	// print examined name if not same as caller
	auto &item_name = item.get_name();
	if (item_name != caller_name)
		get_view().render_text("\n" + item_name + "\n");
	
	// print type
	get_view().render_text("Type: " + Utils::capitalize_word(Utils::to_lower(item.get_type() + " " + item.get_root())) + "\n");
	
	// print description if any
	auto description = item.get_description();
	if (description.length())
		get_view().render_text("Description: " + description + /*(description[description.length()-1] == '.' ? "" : ".") + */ "\n");
	
	// print type specific single value attributes
	for (auto &attribute : singles) {
		get_view().render_text(attribute.first + ": " + Utils::capitalize_word(Utils::to_lower(attribute.second)) + "\n");
	}
	
	// print type specific mapable value attributes and lists
	if (mapables.size() || lists.size()) {
		get_view().render_text("\n");
		
		std::vector<std::string> output;
		
		// mapables
		for (auto &map : mapables) {
			// convert content to string paragraph
			std::string paragraph;
			for (auto &pair : map.second) {
				paragraph += "\n" + Utils::capitalize_word(pair.first) + ": " + Utils::capitalize_word(pair.second);
			}
			// add title + contents to output
			output.emplace_back("<u>" + map.first + "</u>" + paragraph);
		}
		
		// lists
		for (auto &map : lists) {
			// convert content to string paragraph
			std::string paragraph;
			for (auto &s : map.second) {
				paragraph += "\n" + Utils::capitalize_word(s);
			}
			// add title + contents to output
			output.emplace_back("<u>" + map.first + "</u>" + paragraph);
		}
		
		get_view().render_table(output, A_NORMAL);
	}
	
}

void Game_controller::show_messege(const std::string &messege, bool line_break) const {
    get_view().render_text((line_break ? "\n" : "") + messege + Utils::add_punktuation(messege, '.') + "\n");
}

void Game_controller::game_over(const std::string &messege, const Game_object &winner) {
    get_view().render_text("\n<b>GAME OVER! " + messege + "</b>\n", A_NORMAL, true);
    get_view().render_text("\nPress Enter to return to main menu.");
    get_view().prompt_user("");
    
    get_application().pop_controller();
}

void Game_controller::check_game_state() {
    // check if only one player remains
    if (m_world->get_playables().size() == 1) {
        game_over("Winner is: " + m_world->get_playables()[0]->get_name() + "!", *(m_world->get_playables()[0]));
    }
}


#pragma mark - Helper (member) functions

void Game_controller::print_env_description(const std::string &name) const {
	// print artwork
	try {
		get_view().render_text_from_file(m_world->get_environment(name)->get_artwork());
	} catch (std::exception&) {
		// do nothing
	}
	
	// print heading
	std::string slug_line;
	Exterior* ext = dynamic_cast<Exterior*>(m_world->get_current_environment());
	slug_line = ext ? "EXT. " : "INT. ";
	slug_line += m_world->get_current_environment()->get_name() + " - ";
	slug_line += m_world->get_time_string() + (ext ? " (" + ext->get_weather()->get_name() + ")" : "");
	get_view().render_text("\n" + Utils::to_upper(slug_line) + "\n", A_STANDOUT /*| A_BOLD*/);
	
	// print coordinates
	//get_view()->render_text(m_world->get_current_environment()->get_coordinates() + " \n");
	
	// print scene
    std::string scene = m_world->get_current_environment()->get_scene();
    get_view().render_text("\n" + scene + Utils::add_punktuation(scene, '.'), A_NORMAL, true);
	
}

bool Game_controller::is_round_over() const {
	if (!m_world->get_enemies().size() || !m_world->get_playables().size()) {
		return true;
	} else {
		return false;
	}
}

#pragma mark - Command methods

bool Game_controller::load_game(const std::string &file_path, const std::string &unused) {
    if (game_round_on) {
        get_view().render_text("\nCannot load now.\n");
        return true;
    }
    
    if (!file_path.length()) {
        return false;
    }
    
    // prompt user for confirmation
    get_view().render_text("\nLoading game. All unsaved progress will be lost. Continue? Y/N:  ");
    std::string answer = get_view().prompt_user("");
    
    if (Utils::to_lower(answer) == "y") {
        try {;
            m_world = Game_builder::load(file_path);
            
            // set game delegate
            m_world->set_delegate(this);
            
            // clear screen
            get_view().clear_screen();
            
            // activate the world and display start environment's scene description
            m_world->set_active(true);
            
            return true;
        } catch (std::runtime_error &e) {
            get_view().render_text("\nSavegame " + file_path + " not found.\n");
        }
    }
    
    return true;
}

bool Game_controller::save_game(const std::string &file_path, const std::string &unused) {
    if (game_round_on) {
        get_view().render_text("\nCannot save now.\n");
        return true;
    }
    
    if (!file_path.length()){
        return false;
    }
    
    Game_builder::save(*m_world, file_path);
    get_view().render_text("\nGame saved to " + file_path + ".\n");
    return true;
}

bool Game_controller::new_game(const std::string& unused, const std::string& unused2) {
    if (game_round_on) {
        get_view().render_text("\nCannot start new game now.\n");
        return true;
    }
    
    // prompt user for confirmation
    get_view().render_text("\nLoading new game. All unsaved progress will be lost. Continue? Y/N:  ");
    std::string answer = get_view().prompt_user("");
    
    if (Utils::to_lower(answer) == "y") {
        m_world = Game_builder::load(DATA_PATH);
        
        // set game delegate
        m_world->set_delegate(this);
        
        // clear screen
        get_view().clear_screen();
        
        // activate the world and display start environment's scene description
        m_world->set_active(true);
    }
    
    return true;
}

bool Game_controller::quit(const std::string &unused, const std::string &unused2) {
    // prompt user for confirmation
    get_view().render_text("\nQuitting. All unsaved progress will be lost. Continue? Y/N:  ");
    std::string answer = get_view().prompt_user("");
    
    if (Utils::to_lower(answer) == "y") {
        endwin();
        std::exit(0);
    }
    
    return true;
}

bool Game_controller::display_help(const std::string& command, const std::string& unused) {
    get_view().render_text("\n");
    get_view().render_text_from_file("help.txt");
	
	return true;
}





