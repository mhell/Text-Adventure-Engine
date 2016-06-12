//
//  environment.cpp
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "environment.h"
#include "string_replacer.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Environment::ROOT = "Environment";


#pragma mark - Construction/destruction

bool Environment::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Game_object_common<Environment>::request_add_member(prototype_name, role)) {
        return true;
    } else {
        if (role == "actor") {
            auto actor = Actor::make(prototype_name, get_event_repository(), get_game_data(), this, "actor");
            // set active status
            actor->set_active(this->is_active());
            // add to member vector
            add_actor(std::move(actor));
            return true;
        } else if (role == "item") {
            auto item = Item::make(prototype_name, get_event_repository(), get_game_data(), this, "item");
            // set active status
            item->set_active(this->is_active());
            // add to member vector
            add_item(std::move(item));
            return true;
        }
    }

    return false;
}

bool Environment::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Game_object_common<Environment>::request_release_member(member, role)) {
		return true;
	} else {
		// find, release back to corresponding pool, erase from member vector
		if (role == "actor") {
			auto it = std::find_if(m_actors.begin(), m_actors.end(), [&member](const std::unique_ptr<Actor> &actor){return Utils::to_lower(actor->get_name()) == Utils::to_lower(member);});
			if (it != m_actors.end()) {
				Actor::release(std::move(*it));
				m_actors.erase(it);
                return true;
			}
		} else if (role == "item") {
			auto it = std::find_if(m_items.begin(), m_items.end(), [&member](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(member);});
			if (it != m_items.end()) {
				Item::release(std::move(*it));
				m_items.erase(it);
                return true;
            }
        }
        
        return false;
    }
}

void Environment::destroy_impl() {
    // call parent's implementation
    Game_object_common<Environment>::destroy_impl();

    // destroy members
    while (m_actors.begin() != m_actors.end()) {
        m_actors[0]->destroy();
    }
    while (m_items.begin() != m_items.end()) {
        m_items[0]->destroy();
    }
    
    // clear member vectors/maps (behövs ej - görs av destroy?)
    m_actors.clear();
    m_items.clear();
	m_exits.clear();
	
    // reset values
    m_artwork.clear();
	m_coordinates.clear();
	m_scene.clear();
}


#pragma mark - Getters/setters

const std::string& Environment::get_root() const {
    return ROOT;
}

const std::string& Environment::get_artwork() const {
    return m_artwork;
}

void Environment::set_artwork(const std::string &artwork_path) {
    m_artwork = artwork_path;
}

const std::string& Environment::get_coordinates() const {
	return m_coordinates;
}

void Environment::set_coordinates(const std::string &coordinates) {
	m_coordinates = coordinates;
}

std::string Environment::get_scene() const {
	// call virtual implementation to add replacement keys
	std::unordered_map<std::string, std::string> keys;
	get_scene_impl(keys);
	
	// replace keys in scene description with the correct values and return it
	String_replacer replacer(keys);
	return replacer.replace(m_scene);
}

void Environment::get_scene_impl(std::unordered_map<std::string, std::string> &keys) const {
	// call parent implementation
	
	// insert every value to data as a strings
	const Game_data *game_data = get_game_data();
	if (game_data) {
		// add <timestring>
		keys["<timestring>"] = Utils::uncapitalize_word(game_data->get_time_string());
		
		// add <longtimestring>
		keys["<longtimestring>"] = Utils::uncapitalize_word(game_data->get_long_time_string());
		
		// add <playablesdesc>
		std::string playables_desc;
		auto &playables = get_game_data()->get_playables();
		for (auto it = playables.begin(); it != playables.end(); ++it) {
			if (it != playables.begin()) {
				playables_desc  += it+1 == playables.end() ? " and " : ", ";
			}
			playables_desc += (*it)->get_description();
		}
		keys["<playablesdesc>"] = playables_desc;
		
		// add <environmentname>
		keys["<environmentname>"] = Utils::uncapitalize_word(get_name());
		
		// add <environmentdesc>
        keys["<environmentdesc>"] = Utils::uncapitalize_word(get_description());
		
		// add <exitdesc>
		std::string exit_desc;
		auto &exits = get_exits();
		int i = 1;
		for (auto it = exits.begin(); it != exits.end(); ++it) {
			if (it != exits.begin()) {
				exit_desc += (i == exits.size()) ? " and " : ", ";
			}
			exit_desc += it->second.at("description") + (i==1 ? " leads to " : " to ") + Utils::uncapitalize_word(game_data->get_environment(it->second.at("name"))->get_description());
			++i;
		}
		keys["<exitdesc>"] = exit_desc;
		
		// add <itemsdesc>
		std::string items_desc;
		auto &items = game_data->get_current_environment()->get_items();
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it != items.begin()) {
				items_desc  += it+1 == items.end() ? " and " : ", ";
            }
            items_desc += Utils::uncapitalize_word((*it)->get_description());
		}
		keys["<itemsdesc>"] = items_desc.size() ? items_desc : "no items";
		
		// add <enemiesdesc>
		std::string enemies_desc;
		auto &enemies = game_data->get_enemies();
		for (auto it = enemies.begin(); it != enemies.end(); ++it) {
			if (it != enemies.begin()) {
				enemies_desc  += it+1 == enemies.end() ? " and " : ", ";
			}
            enemies_desc += Utils::uncapitalize_word((*it)->get_description());
		}
		keys["<enemiesdesc>"] = enemies_desc.size() ? enemies_desc : "no enemies";
	}
}


void Environment::set_scene(const std::string &scene) {
	m_scene = scene;
}

const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Environment::get_exits() const {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> unhidden_exits;
    
    for (auto &exit : m_exits) {
        try {
            // don't add if hidden
            if (exit.second.at("hidden") != "1") {
                unhidden_exits[exit.first] = exit.second;
            }
        } catch (std::exception&) {
            // add when hidden is not defined
            unhidden_exits[exit.first] = exit.second;
        }
    }
    
    return unhidden_exits;
}

void Environment::set_exits(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &exits) {
	m_exits = exits;
}

bool Environment::activate_exit(const std::string &cardinal) {
    try {
        m_exits.at(cardinal).at("hidden") = "0";
        return true;
    } catch (std::exception&) {
        // do nothing
    }
    return false;
}

bool Environment::hide_exit(const std::string &cardinal) {
    try {
        m_exits.at(cardinal).at("hidden") = "1";
        return true;
    } catch (std::exception&) {
        // do nothing
    }
    return false;
}

const std::vector<std::unique_ptr<Actor>>& Environment::get_actors() const {
    return m_actors;
}

void Environment::add_actor(std::unique_ptr<Actor> actor) {
    m_actors.emplace_back(std::move(actor));
}

std::unique_ptr<Actor> Environment::remove_actor(const std::string &name) {
	auto it = std::find_if(m_actors.begin(), m_actors.end(), [&name](const std::unique_ptr<Actor> &actor){return Utils::to_lower(actor->get_name()) == Utils::to_lower(name);});
	if (it != m_actors.end()) {
		// transfer ownership
		auto ptr = std::move(*it);
		
		// remove from vector
		m_actors.erase(it);

		return ptr;
	}
	
	return nullptr;
}

const std::vector<std::unique_ptr<Item>>& Environment::get_items() const {
    return m_items;
}

void Environment::add_item(std::unique_ptr<Item> item) {
    m_items.emplace_back(std::move(item));
}

std::unique_ptr<Item> Environment::remove_item(const std::string &name) {
	auto it = std::find_if(m_items.begin(), m_items.end(), [&name](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(name);});
	if (it != m_items.end()) {
		// transfer ownership
		auto ptr = std::move(*it);
		
		// remove from vector
		m_items.erase(it);
		
		return ptr;
	}
	
	return nullptr;
}

Item* Environment::get_item(const std::string &name) const {
    auto it = std::find_if(m_items.begin(), m_items.end(), [&name](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(name);});
    if (it != m_items.end()) {
        return it->get();
    }
    
    return nullptr;
}

Game_object* Environment::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Game_object_common<Environment>::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
        // search actors
		if (!role.length() || Utils::to_lower(role) == "actor") {
			auto it_actors = std::find_if(m_actors.begin(), m_actors.end(), [&member](const std::unique_ptr<Actor> &actor){return Utils::to_lower(actor->get_name()) == Utils::to_lower(member);});
			if (it_actors != m_actors.end()) {
				return it_actors->get();
			}
		}
		
		// search items
		if (!role.length() || Utils::to_lower(role) == "item") {
			auto it_items = std::find_if(m_items.begin(), m_items.end(), [&member](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(member);});
			if (it_items != m_items.end()) {
				return it_items->get();
			}
		}
        
        // search exits
        if (!role.length() || Utils::to_lower(role) == "exit") {
            auto it_exits = std::find_if(m_exits.begin(), m_exits.end(), [&member](const std::pair<std::string, std::unordered_map<std::string, std::string>> &item){return Utils::to_lower(item.first) == Utils::to_lower(member);});
            if (it_exits != m_exits.end()) {
                const Game_data *game_data = get_game_data();
                if (game_data) {
                    return game_data->get_environment(it_exits->second.at("name"));
                }
            }
        }
	}
	
	return nullptr;
}


#pragma mark - Serializing

void Environment::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Game_object_common<Environment>::serialize_impl(data);
    
    // add every value to data as a strings
    data.emplace("artwork", get_artwork());
    data.emplace("coordinates", get_coordinates());
    data.emplace("scene", m_scene); // unformatted scene
    
	// for every member map: convert the map to a json object and add it to the data
	std::unordered_map<std::string, std::string> string_map;
	for (auto &exit : m_exits) {
        string_map.emplace(exit.first, JSON_tools::to_json_object(exit.second));
	}
	data.emplace("exits", JSON_tools::to_json_object(string_map));
	
    // for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
    std::vector<std::string> vec;
	for (auto &actor : m_actors) {
        vec.emplace_back(actor->serialize());
	}
    data.emplace("actors", JSON_tools::to_json_array(vec));
    
    vec.clear();
	for (auto &item : m_items) {
        vec.emplace_back(item->serialize());
	}
    data.emplace("items", JSON_tools::to_json_array(vec));
}

bool Environment::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Game_object_common<Environment>::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "artwork") {
			set_artwork(value);
        } else if (key == "coordinates") {
            set_coordinates(value);
        } else if (key == "scene") {
            set_scene(value);
		} else if (key == "exits") {
			if (JSON_tools::is_json_object(value)) {
				// convert and insert a json object into an unorder_map<string, unordered_map>
				std::unordered_map<std::string, std::string> string_map;
				JSON_tools::to_std_map(value, string_map);
				for (auto &exit : string_map) {
					std::unordered_map<std::string, std::string> inner_map;
					JSON_tools::to_std_map(exit.second, inner_map);
					m_exits.insert({ {exit.first, inner_map} });
				}
			}
		} else if (key == "actors") {
            add_actor(Actor::make(value, get_event_repository(), get_game_data(), this, "actor"));
        } else if (key == "items") {
            add_item(Item::make(value, get_event_repository(), get_game_data(), this, "item"));
        } else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void Environment::action_impl(unsigned tick) {
	// call parent implementation
	Game_object_common<Environment>::action_impl(tick);
	
	// propagate action to enemy #tick
	auto &enemies = get_game_data()->get_enemies();
	enemies[tick % enemies.size()]->action(tick);
}


#pragma mark - Interacting

bool Environment::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Game_object_common<Environment>::interact_impl(command, value, remains, caller)) {
		return true;
	} else {
		auto it = m_commands.find(command);
		
		// run the command if found
		if(it != m_commands.end()) {
			return (this->*(it->second))(Utils::trim(value), Utils::trim(remains), caller);
		}
		
		// no commands recognized
		return false;
	}
}

void Environment::became_active() {
    // call parent's implementation
    Game_object_common<Environment>::became_active();
    
    // activate all members
    for (auto &actor : m_actors) {
        actor->set_active(true);
    }
    for (auto &item : m_items) {
        item->set_active(true);
    }
    
	// inform delegate of new active environment
	auto delegate = get_delegate();
	if (delegate) {
		delegate->entered_environment(get_name());
	}
}

void Environment::became_inactive() {
    // call parent's implementation
    Game_object_common<Environment>::became_inactive();
    
    // deactivate all members
    for (auto &actor : m_actors) {
        actor->set_active(false);
    }
    for (auto &item : m_items) {
        item->set_active(false);
    }
}


#pragma mark - Command functions

void Environment::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Game_object_common<Environment>::examine_impl(singles, mapables, lists);
	
    // environment needs to be active (current) to view details
    if (is_active()) {
        // add cooordinates
        //singles["Coordinates"] = get_coordinates();
        
        // add time string
        singles["Time"] = get_game_data()->get_long_time_string();
        
        // add exits
        for (auto &exit : get_exits()) {
            mapables["Exits"].emplace(exit.first, exit.second.at("name"));
        }
        
        // add playables
        for (auto &playable : get_game_data()->get_playables()) {
            lists["Playables"].emplace_back(playable->get_name());
        }
        
        // add actors
        for (auto &enemy : get_game_data()->get_enemies()) {
            lists["Enemies"].emplace_back(enemy->get_name());
        }
        
        // add items
        for (auto &item : get_items()) {
            lists["Items"].emplace_back(item->get_name());
        }
    }
}

void Environment::property_changed() {
	// call parent's implementation
    Game_object_common<Environment>::property_changed();
	
}

bool Environment::exit(const std::string &cardinal, const std::string &unused, const Game_object &caller) {
	// check if the exit exists, if so proceed
	if (get_exits().count(cardinal)) {
		std::string &env_name = m_exits[cardinal].at("name");
		
		// check if all enemies are defeated or if the exit environment's name = prev environment (flees)
		if (env_name == get_game_data()->get_prev_environment_name() || get_game_data()->get_enemies().size() == 0) {
			// set new current environment
			get_game_data()->set_current_environment(env_name);
			return true;
		} else {
			auto delegate = get_delegate();
			if (delegate) {
				// send messege to delegate
				delegate->show_messege(get_game_data()->get_enemies().size() > 1 ?  "Enemies are blocking that exit" : "An enemy is blocking that exit");
				return true;
			}
		}
	}
	
	return false;
}



