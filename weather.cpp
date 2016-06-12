//
//  weather.cpp
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "weather.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Weather::ROOT = "Weather";

const std::string Weather::TYPE = "Weather";

bool Weather::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Weather::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Game_object_common<Weather>::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Weather::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Game_object_common<Weather>::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Weather::destroy_impl() {
	// call parent's implementation
    Game_object_common<Weather>::destroy_impl();

    // destroy members
    
    // clear member vectors/maps
	
    // reset values
    m_obscuring = 0;
}


#pragma mark - Getters/setters

const std::string& Weather::get_root() const {
    return ROOT;
}

const std::string& Weather::get_type() const {
    return TYPE;
}

const int Weather::get_obscuring() const {
	return m_obscuring;
}

void Weather::set_obscuring(int value) {
	m_obscuring = value;
}

Game_object* Weather::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Game_object_common<Weather>::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Weather> Weather::factory() {
    return std::unique_ptr<Weather>(new Weather());
}

void Weather::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Game_object_common<Weather>::serialize_impl(data);
    
    // insert every value to data as a strings
	std::ostringstream convert;
    convert << get_obscuring();
    data.emplace("obscuring", convert.str());
}

bool Weather::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Game_object_common<Weather>::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "obscuring") {
			set_obscuring(std::stoi(value));
        } else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void Weather::action_impl(unsigned tick) {
	// call parent implementation
	Game_object_common<Weather>::action_impl(tick);
	
}


#pragma mark - Interacting

bool Weather::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Game_object_common<Weather>::interact_impl(command, value, remains, caller)) {
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

void Weather::became_active() {
    // call parent's implementation
    Game_object_common<Weather>::became_active();
    
    // activate all members
    
}

void Weather::became_inactive() {
    // call parent's implementation
    Game_object_common<Weather>::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

void Weather::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Game_object_common<Weather>::examine_impl(singles, mapables, lists);
	
}

void Weather::property_changed() {
	// call parent's implementation
    Game_object_common<Weather>::property_changed();
	
}

