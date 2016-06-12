//
//  actor.cpp
//  Lab3
//
//  Created by Mattias Hellman on 16/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#include "actor.h"
#include "item.h"
#include "environment.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Actor::ROOT = "Actor";


#pragma mark - Construction/destruction

bool Actor::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Game_object_common<Actor>::request_add_member(prototype_name, role)) {
        return true;
    } else {
        if (role == "item") {
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

bool Actor::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Game_object_common<Actor>::request_release_member(member, role)) {
		return true;
	} else {
		// find, release back to corresponding pool, erase from member vector
		if (role == "item") {
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

void Actor::destroy_impl() {
    // call parent's implementation
    Game_object_common<Actor>::destroy_impl();
    
    // destroy members
    while (m_items.begin() != m_items.end()) {
        m_items[0]->destroy();
    }
    
    // clear member vectors/maps
    m_items.clear();
	m_health.clear();
	m_actions.clear();
	m_user_commands.clear();
    
    // reset values
    m_playable = false;
	
	// inform game_data of change
	Game_data *data = get_game_data();
	if (data) {
		data->update();
	}
}


#pragma mark - Getters/setters

const std::string& Actor::get_root() const {
    return ROOT;
}

bool Actor::is_playable() const {
    return m_playable;
}

void Actor::set_playable(bool state) {
    m_playable = state;
	
	// inform game_data of change
	Game_data *data = get_game_data();
	if (data) {
		data->update();
	}
}

const std::vector<std::unique_ptr<Item>>& Actor::get_items() const {
    return m_items;
}

void Actor::add_item(std::unique_ptr<Item> item) {
    m_items.emplace_back(std::move(item));
}

std::unique_ptr<Item> Actor::remove_item(const std::string &name) {
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

int Actor::get_health() const {
	int result = 1;
	bool success;
	for (auto &prop : m_health) {
		int value = get_property(prop, success);
		if (success) result *= value; // ***
	}
	return result;
}

bool Actor::check_health() {
	// zero or negative health
	if (get_health() <= 0) {
		// send messege to delegate
		auto delegate = get_delegate();
		if (delegate) delegate->show_messege(get_name() + " died");
        
        // drop all items
        interact("drop : all");
		
		// destroy this (release to pool)
		destroy();
		
		return false;
	// actor still has health
	} else {
		return true;
	}
}

Game_object* Actor::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Game_object_common<Actor>::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		// search items
		if (!role.length() || Utils::to_lower(role) == "item") {
			auto it_items = std::find_if(m_items.begin(), m_items.end(), [&member](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(member);});
			if (it_items != m_items.end()) {
				return it_items->get();
			}
		}
	}
	
	return nullptr;
}


#pragma mark - Serializing

void Actor::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Game_object_common<Actor>::serialize_impl(data);
    
    // add every value to data as a strings
    std::ostringstream convert;
    convert << is_playable();
    data.emplace("playable", convert.str());
	//convert.str("");
    //convert.clear();
	
	// for every map: convert the map to a json object and add it to the data
	data.emplace("usercommands", JSON_tools::to_json_object(m_user_commands));
    
    // for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
    std::vector<std::string> vec;
    for (auto &item : m_items) {
        vec.emplace_back(item->serialize());
	}
    data.emplace("items", JSON_tools::to_json_array(vec));
	
	vec.clear();
    for (auto &event : m_actions) {
		vec.emplace_back(get_event_repository()->get_event(event)->serialize());
	}
    data.emplace("actions", JSON_tools::to_json_array(vec));
	
    data.emplace("health", JSON_tools::to_json_array(m_health));
}

bool Actor::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Game_object_common<Actor>::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "playable") {
            set_playable(std::stoi(value));
        } else if (key == "items") {
            add_item(Item::make(value, get_event_repository(), get_game_data(), this, "item"));
        } else if (key == "health") {
           m_health.emplace_back(value);
		} else if (key == "actions") {
			// make the event
			auto event = Event::make(value);
			
			// insert name into m_actions map
			m_actions.emplace_back(event->get_name());
			
			// add event to the event repository
			get_event_repository()->add_event(std::move(event));
		} else if (key == "usercommands") {
			if (JSON_tools::is_json_object(value)) {
				// convert object to std map
				std::unordered_map<std::string, std::string> string_map;
				JSON_tools::to_std_map(value, string_map);
				
				// insert as <string, string> into m_user_commands
				for(const auto& pair : string_map) {
					m_user_commands.emplace(pair.first, pair.second);
				}
			}
		} else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void Actor::action_impl(unsigned tick) {
	// call parent implementation
	Game_object_common<Actor>::action_impl(tick);
	
	get_delegate()->show_messege(get_name());
}


#pragma mark - Interacting

bool Actor::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Game_object_common<Actor>::interact_impl(command, value, remains, caller)) {
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

void Actor::became_active() {
    // call parent's implementation
    Game_object_common<Actor>::became_active();
    
    // activate all members
    for (auto &item : m_items) {
        item->set_active(true);
    }
}

void Actor::became_inactive() {
    // call parent's implementation
    Game_object_common<Actor>::became_inactive();
    
    // deactivate all members
    for (auto &item : m_items) {
        item->set_active(false);
    }
}


#pragma mark - Command functions

bool Actor::drop(const std::string &item, const std::string &unused, const Game_object &caller) {
    auto delegate = get_delegate();
    if (!item.length()) {
        delegate->show_messege("No item specified");
    } else {
        Game_data *data = get_game_data();
        // drop all items
        if (Utils::to_lower(item) == "all") {
            if (get_items().size() && data) {
                for (auto &it : m_items) {
                    // change owner
                    it->set_owner(data->get_current_environment());
                    // add the item to the current environment
                    data->get_current_environment()->add_item(std::move(it));
                }
                // clear member vector
                m_items.clear();
                // send messege to delegate
                if (delegate) {
                    delegate->show_messege(get_name() + " dropped all items");
                }
            }
            return true;
        // try to find the item
        } else {
            auto it = std::find_if(m_items.begin(), m_items.end(), [&item](const std::unique_ptr<Item> &item_obj){return Utils::to_lower(item_obj->get_name()) == Utils::to_lower(item);});
            if (it != m_items.end()) {
                if (data) {
                    // send messege to delegate
                    if (delegate) {
                        delegate->show_messege(get_name() + " dropped " + (*it)->get_name());
                    }
                    // change owner
                    (*it)->set_owner(data->get_current_environment());
                    // add the item to the current environment
                    data->get_current_environment()->add_item(std::move(*it));
                    // erase from member vector
                    m_items.erase(it);
                    return true;
                }
            } else {
                // send messege to delegate telling item not found/unspecified
                if (delegate) {
                    delegate->show_messege("Item '" + item + "' not found on " + get_name());
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Actor::pick_up(const std::string &first_item, const std::string &rest_items, const Game_object &caller) {
    auto delegate = get_delegate();
    if (!first_item.length()) {
        delegate->show_messege("No item specified");
    } else {
        Game_data *data = get_game_data();
        if (data) {
            std::string item_name;
            std::unique_ptr<Item> taken_item;
            
            // iterative pick-up from container if container(s) specified
            if (rest_items.length()) {
                auto container = data->get_current_environment()->get_item(first_item); // the first container
                std::stringstream containers_ss(rest_items);
                std::string item_to_pickup, container_name = first_item;
                std::getline(containers_ss, item_to_pickup, ':');
                
                while (!containers_ss.eof()) {
                    if (container) {
                        // get current item (is a container)
                        container = container->get_item(Utils::trim(item_to_pickup));
                        container_name = item_to_pickup;
                        // get next container/item name
                        std::getline(containers_ss, item_to_pickup, ':');
                    } else {
                        // send messege to delegate telling container not found
                        if (delegate) {
                            delegate->show_messege("Item '" + container_name + "' not found");
                            return true;
                        }
                        return false;
                    }
                }
                
                if (!container) {
                    // send messege to delegate telling container not found
                    if (delegate) {
                        delegate->show_messege("Item '" + container_name + "' not found");
                        return true;
                    }
                    return false;
                }
                
                item_name = Utils::trim(item_to_pickup);
                taken_item = container->remove_item(item_name);
            } else {
                item_name = first_item;
                taken_item = data->get_current_environment()->remove_item(first_item);
            }
 
            // set new owner and add item to member vector if recieved
            if (taken_item) {
                taken_item->set_owner(this);
                add_item(std::move(taken_item));
                
                // send messege to delegate
                if (delegate) {
                    delegate->show_messege(get_name() + " picked up " + item_name);
                    return true;
                }
            } else {
                // send messege to delegate telling item not found/unspecified
                if (delegate) {
                    delegate->show_messege("Item '" + item_name + "' not found");
                    return true;
                }
            }
        }
    }
        
    return false;
}

bool Actor::search_consume(const std::string &property, const std::string &unused, const Game_object &caller) {
	std::cout << "hej";
	
	return true;
}

bool Actor::search_pick_up(const std::string &property, const std::string &unused, const Game_object &caller) {
	std::cout << "hej";
	
	return true;
}

bool Actor::hunt_consume(const std::string &property, const std::string &item, const Game_object &caller) {
	std::cout << "hej";
	
	return true;
}

void Actor::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Game_object_common<Actor>::examine_impl(singles, mapables, lists);
	
	// add health
	std::string health_props;
	//for (auto &property : m_health) {
	for (auto it = m_health.begin(); it != m_health.end(); ++it) {
		health_props += *it + (it+1 !=  m_health.end() ? " * " : "");
	}
	std::stringstream convert;
	convert << get_health();
	singles["Health"] = convert.str() + " (" + health_props + ")";
	
	// add items
	for (auto &item : get_items()) {
		lists["Items"].emplace_back(item->get_name());
	}
	
	// add actions
	for (auto &action : m_actions) {
		lists["Actions"].emplace_back(get_event_repository()->get_event(action)->get_description());
	}
	
	// add user commands and remove actions if playable
	if (is_playable()) {
		for (auto &command : m_user_commands) {
			mapables["Commands"].emplace(command.first, command.second);
		}
		
		lists.erase("Actions");
	}
}

void Actor::property_changed() {
	// call parent's implementation
    Game_object_common<Actor>::property_changed();
	
	check_health();
}


