//
//  human.cpp
//  Lab3
//
//  Created by Mattias Hellman on 18/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#include "human.h"
#include "environment.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Human::TYPE = "Human";

bool Human::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Human::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Actor::request_add_member(prototype_name, role)) {
        return true;
    }
    
    return false;
}

bool Human::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Actor::request_release_member(member, role)) {
		return true;
	} else {
		// if found return true, if not found return false
        
        return false;
	}
}

void Human::destroy_impl() {
    // call parent's implementation
    Actor::destroy_impl();
    
    // destroy members, clear member vectors/maps and reset values
    
}


#pragma mark - Getters/setters

const std::string& Human::get_type() const {
    return TYPE;
}

Game_object* Human::get_member(const std::string &member, const std::string &role) const {
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

std::unique_ptr<Actor> Human::factory() {
    return std::unique_ptr<Human>(new Human());
}

void Human::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Actor::serialize_impl(data);
    
}

bool Human::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Actor::deserialize_impl(key, value)) {
        return true;
    } else {
		// if found return true, if not found return false
        
        return false;
    }
}


#pragma mark - Actions

void Human::action_impl(unsigned tick) {
	// call parent implementation
	Actor::action_impl(tick);
	
}


#pragma mark - Interacting

bool Human::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
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

void Human::became_active() {
    // call parent's implementation
    Actor::became_active();
    
    // activate all members
    
}

void Human::became_inactive() {
    // call parent's implementation
    Actor::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

bool Human::use(const std::string &item, const std::string &remains, const Game_object &caller) {
    auto delegate = get_delegate();
    if (!item.length()) {
        delegate->show_messege("No item specified");
    } else {
        // try to find the item
        auto &m_items = get_items();
        auto it = std::find_if(m_items.begin(), m_items.end(), [&item](const std::unique_ptr<Item> &item_obj){return Utils::to_lower(item_obj->get_name()) == Utils::to_lower(item);});
        if (it != m_items.end()) {
            // call interact on found item with "remains" as command and this as the caller. Add self as target none specified
            return (*it)->interact(remains.length() ? remains : "on : " + get_name(), this);
        } else {
            // send messege to delegate telling item not found/unspecified
            if (delegate) {
                delegate->show_messege("Item '" + item + "' not found on " + get_name());
            }
            return true;
        }
    }
    
    return false;
}

bool Human::give(const std::string &item, const std::string &remains, const Game_object &caller) {
    auto delegate = get_delegate();
    if (!item.length()) {
        delegate->show_messege("No item specified");
    } else {
        // try to take the item
        auto taken_item = remove_item(item);
        
        if (taken_item) {
            // get target (from "to : target")
            std::string target = JSON_tools::replace(remains, "to\\s?:\\s?", "");
            
            // try to find the target among playables
            auto &actors = get_game_data()->get_playables();
            auto it = std::find_if(actors.begin(), actors.end(), [&target](const Actor *actor_obj){return Utils::to_lower(actor_obj->get_name()) == Utils::to_lower(target);});

            // target found and is playable
            if (it != actors.end() && (*it)->is_playable()) {
                // change owner
                taken_item->set_owner(*it);
                // add item to target
                (*it)->add_item(std::move(taken_item));
                // send messege to delegate
                if (delegate) {
                    delegate->show_messege(get_name() + " gave " + item + " to " + (*it)->get_name());
                }
                return true;
            // target not found
            } else {
                // send messege to delegate
                if (delegate) {
                    delegate->show_messege("Playable '" + target + "' not found");
                }
                // put back item
                add_item(std::move(taken_item));
                return true;
            }
        } else {
            // send messege to delegate telling item not found/unspecified
            if (delegate) {
                delegate->show_messege("Item '" + item + "' not found on " + get_name());
            }
            return true;
        }
    }
    
    return false;
}

void Human::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Actor::examine_impl(singles, mapables, lists);
	
}

void Human::property_changed() {
	// call parent's implementation
    Actor::property_changed();
	
}


