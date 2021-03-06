//
//  troll.cpp
//  Lab3
//
//  Created by Mattias Hellman on 18/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#include "troll.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Troll::TYPE = "Troll";

bool Troll::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Troll::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Actor::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Troll::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Actor::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Troll::destroy_impl() {
    // call parent's implementation
    Actor::destroy_impl();
    
    // destroy members, clear member vectors/maps and reset values
    
}


#pragma mark - Getters/setters

const std::string& Troll::get_type() const {
    return TYPE;
}

Game_object* Troll::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Actor::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Actor> Troll::factory() {
    return std::unique_ptr<Troll>(new Troll());
}

void Troll::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Actor::serialize_impl(data);
    
}

bool Troll::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Actor::deserialize_impl(key, value)) {
        return true;
    } else {
		// if found return true, if not found return false
        
        return false;
    }
}


#pragma mark - Actions

void Troll::action_impl(unsigned tick) {
	// call parent implementation
	Actor::action_impl(tick);
	
}


#pragma mark - Interacting

bool Troll::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Actor::interact_impl(command, value, remains, caller)) {
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

void Troll::became_active() {
    // call parent's implementation
    Actor::became_active();
    
    // activate all members
    
}

void Troll::became_inactive() {
    // call parent's implementation
    Actor::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

void Troll::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Actor::examine_impl(singles, mapables, lists);
	
}

void Troll::property_changed() {
	// call parent's implementation
    Actor::property_changed();
	
}


