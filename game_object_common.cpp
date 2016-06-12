//
//  game_object_common.cpp
//  Lab3
//
//  Created by Mattias Hellman on 20/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "game_object_common.h"
#include "json_tools.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Setters/getters

template <typename T>
const std::string& Game_object_common<T>::get_name() const {
    return m_name;
}

template <typename T>
void Game_object_common<T>::set_name(const std::string &name) {
    m_name = name;
}

template <typename T>
void Game_object_common<T>::set_properties(const std::unordered_map<std::string, int> &properties) {
	m_properties = properties;
}

template <typename T>
const std::unordered_map<std::string, int>& Game_object_common<T>::get_properties() const {
	return m_properties;
}

template <typename T>
std::string Game_object_common<T>::get_description() const {
    // replace all occurrences of <name> with the objects name in the description string
	return JSON_tools::replace(m_description, "<name>", get_name());
}

template <typename T>
void Game_object_common<T>::set_description(const std::string &desc) {
    m_description = desc;
}

template <typename T>
Game_object* Game_object_common<T>::get_member(const std::string &member, const std::string &role) const {
	// search in this...
	
	return nullptr;
}

template <typename T>
int Game_object_common<T>::get_property(const std::string &name, bool &success) const {
	auto it = get_properties().find(name);
	
	if(it != get_properties().end()) {
		success = true;
		return it->second;
	}
	
	success = false;
	return 0;
}


#pragma mark - Construction/destruction

template <typename T>
std::unique_ptr<T> Game_object_common<T>::make(const std::string &name_or_json, std::shared_ptr<Event_repository> event_rep, Game_data *data, Game_object *owner, const std::string &role) {
	// call make_json if name_or_json is a json object
	if (JSON_tools::is_json_object(name_or_json)) {
		return make_json(name_or_json, std::move(event_rep), data, owner, role);
	} else {
		// acquire object from prototype pool
		auto instance = Game_object_common<T>::acquire(name_or_json); // korrekt skrivet. Se Templates: Calling method of template base class
		
		// assign event repository
		instance->set_event_repository(std::move(event_rep));
		
		// assign data class
		instance->set_game_data(data);
		
		// assign owner
		instance->set_owner(owner);
		
		// set role of owner
		instance->set_role_in_owner(role);
		
		return instance;
	}
}

template <typename T>
std::unique_ptr<T> Game_object_common<T>::make_json(const std::string &json, std::shared_ptr<Event_repository> event_rep, Game_data *data, Game_object *owner, const std::string &role) {
    // make the prototype object
    auto object = make(JSON_tools::find_value_in_object(json, "prototype"), std::move(event_rep), data, owner, role);
    
    // merge the prototype's definition with the new, then apply the merged definition
    std::string merged = JSON_tools::merge_json(object->serialize(), json);
	
	// deserialize
    object->deserialize(merged);
	
	// pre activate name and description
	object->activate_selective( {"name", "description"} );
    
    return object;
}

template <typename T>
bool Game_object_common<T>::request_add_member(const std::string &prototype_name, const std::string &role) {
    // add members based on the role argument
    
    return false;
}

template <typename T>
bool Game_object_common<T>::request_release_member(const std::string &member, const std::string &role) {
    // release members based on the role argument
    
	// no member found
	return false;
}

template <typename T>
void Game_object_common<T>::destroy() {
	// call virtual implementation
    destroy_impl();
	
	// call owner and request release of this unique_ptr back to the pool
	Game_object* owner = get_owner();
    if (owner) {
		owner->request_release_member(get_name(), get_role_in_owner());
	}
}

template <typename T>
void Game_object_common<T>::destroy_impl() {
    // destroy members
    
    // clear member vectors/maps
	m_properties.clear();
	m_reactions.clear();
	
	// reset values
    m_name.clear();
	m_description.clear();
	Serializable::reset();
}


#pragma mark - Serializing

template <typename T>
void Game_object_common<T>::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent implementation
    
    // add each value to the data map as strings (with std strings as json strings)
    data.emplace("name", get_name());
    data.emplace("description", get_description());
    data.emplace("prototype", get_type()); // the deserialized object have recieved all data from its prototype (and has become) it's prototype. It's new prototype is therefore the base type e.g. "Troll"
    data.emplace("root", get_root());
    data.emplace("inrandom", inrandom ? "1" : "0");
    
	// for every map: convert the map to a json object and add it to the data
	data.emplace("properties", JSON_tools::to_json_object(m_properties));
	
	//----------
	// Reactions
	//----------
	std::unordered_map<std::string, std::string> reactions_map;

	// for each reaction
	for(const auto& reaction_pair : m_reactions) {
		std::unordered_map<std::string, std::string> reaction_map;
		
		// for each entry in a reaction
		for(const auto& entry_pair : reaction_pair.second) {
			// the event - get it and convert to json object
			if (entry_pair.first == "event") {
				reaction_map.emplace(entry_pair.first, m_event_repository->get_event(entry_pair.second)->serialize());
			} else {
				reaction_map.emplace(entry_pair.first, entry_pair.second);
			}
		}
		
		// insert the reactions in the reactions map
		reactions_map.emplace(reaction_pair.first, JSON_tools::to_json_object(reaction_map));

	}
	
	// insert the reactions map into data converted to json object
	data.emplace("reactions", JSON_tools::to_json_object(reactions_map));
	
	//--------------
	// Reactions end
	//--------------
	
}

template <typename T>
bool Game_object_common<T>::deserialize_impl(const std::string &key, const std::string &value) {
	if (key == "name") {
        set_name(value);
	} else if (key == "description") {
		set_description(value);
	} else if (key == "properties") {
		if (JSON_tools::is_json_object(value)) {
			// convert object to std map
			std::unordered_map<std::string, std::string> string_map;
			JSON_tools::to_std_map(value, string_map);
			
			// insert as <string, int> into m_properties
			for(const auto& pair : string_map) {
				m_properties.emplace(pair.first, std::stoi(pair.second));
			}
		}
	} else if (key == "reactions") {
		// convert object to std map
		std::unordered_map<std::string, std::string> reaction_map;
		JSON_tools::to_std_map(value, reaction_map);
		
		// iterate through the reaction map (pair.first = a property, pair.second = map<key(event/multiplier), string_value>)
		for(const auto& pair : reaction_map) {
			std::unordered_map<std::string, std::string> reaction_value;
			
			// convert pair.second into a map<sting, string>
			std::unordered_map<std::string, std::string> value_map;
			JSON_tools::to_std_map(pair.second, value_map);
			
			// itarate through the value_map and insert into reaction_value (pair.first = (event/multiplier), pair.second = string_value)
			for(const auto& pair : value_map) {
				if (pair.first == "event") {
					// make the event
					auto event = Event::make(pair.second);
					
					// insert name into reaction_value map (at key "event")
					reaction_value.emplace(pair.first, event->get_name());
					
					// add event to the event repository
					m_event_repository->add_event(std::move(event));
				} else if (pair.first == "multiplier") {
					reaction_value.emplace(pair.first, pair.second);
				}
			}
			
			// insert reaction key and reaction value into m_properties
			m_reactions.emplace(pair.first, reaction_value);
        }
    } else if (key == "inrandom") {
        inrandom = value == "0" ? false : true;
	} else {
        return false;
    }
    
    return true;
}


#pragma mark - Actions

template <typename T>
void Game_object_common<T>::action(unsigned tick) {
	// ???
	
	// call virtual implementation
	action_impl(tick);
	
}

template <typename T>
void Game_object_common<T>::action_impl(unsigned tick) {
	// call parent implementation
	
}


#pragma mark - Interacting

template <typename T>
bool Game_object_common<T>::interact(const std::string &commands, const Game_object *caller) {
	// activation required for interaction
	/*if (!is_active()) {
		return false;
	}*/
	
	// parse command 
	std::stringstream ss(commands);
	std::string command, value, remains;
	std::getline(ss, command, ':');
	std::getline(ss, value, ':');
	std::getline(ss, remains, '\0');
	
	// make this the caller if none specified
	if (!caller)
		caller = this;
	
	// call virtual implementation
	return interact_impl(Utils::trim(command), Utils::trim(value), Utils::trim(remains), *caller);
}

template <typename T>
bool Game_object_common<T>::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	auto it = m_commands.find(command);
	
	// run the command if found
	if(it != m_commands.end()) {
		return (this->*(it->second))(Utils::trim(value), Utils::trim(remains), caller);
	}
	
	// no commands recognized
    return false;
}

template <typename T>
bool Game_object_common<T>::forward_interact(const std::string &member, const std::string &commands, const Game_object *caller) {
	auto *game_object = get_member(member);
	
	// if member found; call interact on it with updated caller
	if (game_object) {
		return game_object->interact(commands, game_object);
	} else {
		return false;
	}
}

template <typename T>
void Game_object_common<T>::became_active() {
    // activate all members
    
}

template <typename T>
void Game_object_common<T>::became_inactive() {
    // deactivate all members?
    
}

template <typename T>
bool Game_object_common<T>::expose_to_property(const std::string &property, int property_level, const Game_object &caller) {
	// try to find if any reaction is available for the property else return false
	auto it = m_reactions.find(property);
	if (it!= m_reactions.end()) {
		try {
			// get the reaction command
			std::string commands = m_event_repository->get_event(it->second.at("event"))->get_commands();
			std::stringstream commands_ss(commands);
            std::string command;
            
            // run each command in commands
            while (!commands_ss.eof()) {
                std::getline(commands_ss, command, ',');
                
                // get the product (property_level * multiplier at caller, or only property_level if the reaction has no multiplier)
                int product;
                if (it->second.count("multiplier")) {
                    bool success;
                    product = (property_level * caller.get_property(it->second.at("multiplier"), success));
                } else {
                    product = property_level;
                }
                
                // replace <posproduct> in the command with the triggering property's level * caller's multiplier value
                // and <negproduct> with the negative of product
                std::stringstream convert;
                convert << product;
                command = JSON_tools::replace(command, "<posproduct>", convert.str());
                convert.str("");
                convert.clear();
                convert << -product;
                command = JSON_tools::replace(command, "<negproduct>", convert.str());
                
                // run the command on this
                interact(command, &caller);
            }
            
            // all commands sucessfully run
            return true;
        } catch (std::out_of_range&) {
			// an at failed, do nothing
		}
	}
	
	return false;
}


#pragma mark - Command functions

template <typename T>
bool Game_object_common<T>::examine(const std::string &member, const std::string &remains, const Game_object &caller) {
	// if member argument given, find and run command only on it
	if (member.length()) {
		return forward_interact(member, "examine : " + remains, &caller);
    // else examine this
	} else {
		auto delegate = get_delegate();
		if (delegate) {
			// call virtual implementation to get type specific attributes
			String_map singles;
			String_map_map mapables;
			String_map_vec lists;
			examine_impl(singles, mapables, lists);
			
			// inform delegate of examination request
			delegate->show_examination(caller, *this, singles, mapables, lists);
			
			// forward examine on all members
			//forward_interact_all("examine", caller); ?
			
			return true;
		}
	}
	
	return false;
}

template <typename T>
void Game_object_common<T>::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// add properties
	std::ostringstream convert;
	for (auto &property : get_properties()) {
		convert << property.second;
        mapables["Properties"].emplace(property.first, convert.str());
		convert.str("");
		convert.clear();
	}
	
	// add reactions
	for (auto &reaction : m_reactions) {
		try {
			std::string description = m_event_repository->get_event(reaction.second.at("event"))->get_description();
			
			description = JSON_tools::replace(description, "<property>", reaction.first);
			
            try {
                description = JSON_tools::replace(description, "<multiplier>", reaction.second.at("multiplier"));
            } catch (std::out_of_range&) {
                // an at failed, do nothing
            }
			
			mapables["Reactions"].emplace(reaction.first, description);
		} catch (std::out_of_range&) {
			// an at failed, do nothing
		}
	}
}

template <typename T>
bool Game_object_common<T>::transform(const std::string &new_object_name, const std::string &role, const Game_object &caller) {
    bool result = false;
    
    // request add member at owner
    Game_object* owner = get_owner();
    if (owner) {
        result = owner->request_add_member(new_object_name, role);
    }
    
    if (result) {
        // messege delegate
        auto delegate = get_delegate();
        if (delegate) {
            delegate->show_messege(get_name() + " became " + new_object_name);
        }
        
        // destroy this (release to pool)
        destroy();
        
        return true;
    }
    
    return false;
}

template <typename T>
bool Game_object_common<T>::rename(const std::string &name, const std::string &unused, const Game_object &caller) {
    std::string old_name = get_name();
    
    // set new name
    set_name(name);
    
    // messege delegate
    auto delegate = get_delegate();
    if (delegate) {
        delegate->show_messege(old_name + " became " + get_name());
    }
    
    return true;
}

template <typename T>
bool Game_object_common<T>::change_property(const std::string &property, const std::string &value, const Game_object &caller) {
	try {
		int int_value = std::stoi(value);
        auto &prop_value = m_properties.at(property);
        
        // negative properties unwanted
        if (prop_value <= 0) {
            return false;
        }
        
        // change property value
        prop_value += int_value;
		
		// messege delegate
		auto delegate = get_delegate();
		if (delegate) {
            delegate->show_messege(get_name() + (int_value < 0 ? " lost " + value.substr(1) : (int_value == 0 ? " lost " : " gained ") + value) + " " + property);
		}
		
		// call virtual implementation
		property_changed();
	} catch (std::exception&) {
		return false;
	}
	
	return true;
}

template <typename T>
void Game_object_common<T>::property_changed() {
	
}


#pragma mark - Class communication

template <typename T>
void Game_object_common<T>::set_owner(Game_object *owner) {
    m_owner = owner;
}

template <typename T>
Game_object* Game_object_common<T>::get_owner() const {
	return m_owner;
}

template <typename T>
void Game_object_common<T>::set_role_in_owner(const std::string &role) {
    m_role_in_owner = role;
}

template <typename T>
const std::string& Game_object_common<T>::get_role_in_owner() const {
    return m_role_in_owner;
}

template <typename T>
void Game_object_common<T>::set_game_data(Game_data *data) {
    m_game_data = std::move(data);
}

template <typename T>
const Game_data* Game_object_common<T>::get_game_data() const {
    return m_game_data;
}

template <typename T>
Game_data* Game_object_common<T>::get_game_data() {
    return m_game_data;
}

template <typename T>
void Game_object_common<T>::set_event_repository(std::shared_ptr<Event_repository> event_repository) {
	m_event_repository = std::move(event_repository);
}

template <typename T>
const std::shared_ptr<Event_repository>& Game_object_common<T>::get_event_repository() const {
	return m_event_repository;
}







