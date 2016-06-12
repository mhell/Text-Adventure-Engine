//
//  item.cpp
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "item.h"
#include "environment.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Item::ROOT = "Item";


#pragma mark - Construction/destruction

bool Item::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Game_object_common<Item>::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Item::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Game_object_common<Item>::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Item::destroy_impl() {
    // call parent's implementation
    Game_object_common<Item>::destroy_impl();
    
    // destroy members
    
    // clear member vectors/maps
    
    // reset values
	
}


#pragma mark - Getters/setters

const std::string& Item::get_root() const {
    return ROOT;
}

Game_object* Item::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Game_object_common<Item>::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}

void Item::set_degradables(const std::vector<std::string> &degradables) {
    m_degradables = degradables;
}

const std::vector<std::string> & Item::get_degradables() const {
    return m_degradables;
}


#pragma mark - Serializing

void Item::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Game_object_common<Item>::serialize_impl(data);
	
}

bool Item::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Game_object_common<Item>::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "degradables") {
            m_degradables.emplace_back(value);
        } else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void Item::action_impl(unsigned tick) {
	// call parent implementation
	Game_object_common<Item>::action_impl(tick);
	
}


#pragma mark - Interacting

bool Item::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Game_object_common<Item>::interact_impl(command, value, remains, caller)) {
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

void Item::became_active() {
    // call parent's implementation
    Game_object_common<Item>::became_active();
    
    // activate all members
    
}

void Item::became_inactive() {
    // call parent's implementation
    Game_object_common<Item>::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

bool Item::on(const std::string &target, const std::string &unused, const Game_object &caller) {
	// search current environments actor members
    Game_object *member = get_game_data()->get_current_environment()->get_member(target, "actor");
    
    // search current environments item members
    if (!member)
        member = get_game_data()->get_current_environment()->get_member(target, "item");
    
	if (member) {
		bool result(0);
		// expose target to this items all propertys
		for (auto &property : get_properties()) {
			result += member->expose_to_property(property.first, property.second, caller);
            // degrade property value if it is degradable
            if (count(m_degradables.begin(), m_degradables.end(), property.first)) {
                change_property(property.first, "-1", caller);
            }
		}
		
        // send messege to delegate if no reaction
		if (!result) {
			get_delegate()->show_messege("No reaction occurred");
		}
		
		return true;
	} else {
		// send messege to delegate telling item not found
		auto delegate = get_delegate();
		if (delegate) {
			delegate->show_messege("Target '" + target + "' not found.");
			return true;
		}
	}
	
	return false;
}

bool Item::with(const std::string &target, const std::string &unused, const Game_object &caller) {
    // search owner's item members
    Game_object *member = caller.get_member(target, "item");
    
    if (member) {
        bool result(0);
        // expose target to this items all propertys
        for (auto &property : get_properties()) {
            result += member->expose_to_property(property.first, property.second, caller);
        }
        
        // send messege to delegate if no reaction
        if (!result) {
            get_delegate()->show_messege("No reaction occurred");
        }
        
        return true;
    } else {
        // send messege to delegate telling item not found
        auto delegate = get_delegate();
        if (delegate) {
            delegate->show_messege("Target '" + target + "' not found.");
            return true;
        }
    }
    
    return false;
}

void Item::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Game_object_common<Item>::examine_impl(singles, mapables, lists);
    
    // add degradable information to properties
    try {
        auto &properties = mapables.at("Properties");
        for (auto &deg_prop : get_degradables()) {
            try {
                auto &prop_value = properties.at(deg_prop);
                properties[deg_prop] = prop_value + " (deg)";
            } catch (std::out_of_range&) {
                // do nothing
            }
        }
    } catch (std::out_of_range&) {
        // do nothing
    }
}

void Item::property_changed() {
	// call parent's implementation
    Game_object_common<Item>::property_changed();
	
}


