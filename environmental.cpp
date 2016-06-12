//
//  environmental.cpp
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "environmental.h"
#include "environment.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Environmental::TYPE = "Environmental";

bool Environmental::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Environmental::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Item::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Environmental::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Item::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Environmental::destroy_impl() {
    // call parent's implementation
    Item::destroy_impl();
    
    // destroy members, clear member vectors/maps and reset values
    
}


#pragma mark - Getters/setters

const std::string& Environmental::get_type() const {
    return TYPE;
}

Game_object* Environmental::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Item::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}

bool Environmental::add_item(std::unique_ptr<Item> item) {
    return false;
}

std::unique_ptr<Item> Environmental::remove_item(const std::string &name) {
    return nullptr;
}

Item* Environmental::get_item(const std::string &name) const {
    return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Item> Environmental::factory() {
    return std::unique_ptr<Environmental>(new Environmental());
}

void Environmental::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Item::serialize_impl(data);
    
}

bool Environmental::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Item::deserialize_impl(key, value)) {
        return true;
    } else {
		// if found return true, if not found return false
        
        return false;
    }
}


#pragma mark - Actions

void Environmental::action_impl(unsigned tick) {
	// call parent implementation
	Item::action_impl(tick);
	
}


#pragma mark - Interacting

bool Environmental::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
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

void Environmental::became_active() {
    // call parent's implementation
    Item::became_active();
    
    // activate all members
    
}

void Environmental::became_inactive() {
    // call parent's implementation
    Item::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

bool Environmental::activate_exit(const std::string &cardinal, const std::string &unused, const Game_object &caller) {
    auto delegate = get_delegate();
    if (cardinal.length()) {
        // add exit to environment
        const Game_data *game_data = get_game_data();
        if (game_data) {
            game_data->get_current_environment()->activate_exit(cardinal);
        }
        
        // send messege to delegate telling item not found/unspecified
        if (delegate) {
            delegate->show_messege("An exit " + cardinal + " appeared");
        }
        return true;
    }
    
    return false;
}

void Environmental::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Item::examine_impl(singles, mapables, lists);
	
}

void Environmental::property_changed() {
	// call parent's implementation
    Item::property_changed();
	
}


