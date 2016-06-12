//
//  misc.cpp
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "misc.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Misc::TYPE = "Misc";

bool Misc::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Misc::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Item::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Misc::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Item::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Misc::destroy_impl() {
    // call parent's implementation
    Item::destroy_impl();
    
    // destroy members, clear member vectors/maps and reset values
    
}


#pragma mark - Getters/setters

const std::string& Misc::get_type() const {
    return TYPE;
}

Game_object* Misc::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Item::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}

bool Misc::add_item(std::unique_ptr<Item> item) {
    return false;
}

std::unique_ptr<Item> Misc::remove_item(const std::string &name) {
    return nullptr;
}

Item* Misc::get_item(const std::string &name) const {
    return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Item> Misc::factory() {
    return std::unique_ptr<Misc>(new Misc());
}

void Misc::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Item::serialize_impl(data);
    
}

bool Misc::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Item::deserialize_impl(key, value)) {
        return true;
    } else {
		// if found return true, if not found return false
        
        return false;
    }
}


#pragma mark - Actions

void Misc::action_impl(unsigned tick) {
	// call parent implementation
	Item::action_impl(tick);
	
}


#pragma mark - Interacting

bool Misc::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Item::interact_impl(command, value, remains, caller)) {
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

void Misc::became_active() {
    // call parent's implementation
    Item::became_active();
    
    // activate all members
    
}

void Misc::became_inactive() {
    // call parent's implementation
    Item::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

void Misc::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Item::examine_impl(singles, mapables, lists);
	
}

void Misc::property_changed() {
	// call parent's implementation
    Item::property_changed();
	
}


