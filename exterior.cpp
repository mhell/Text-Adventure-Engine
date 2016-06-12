//
//  Exterior.cpp
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "exterior.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Exterior::TYPE = "Exterior";

bool Exterior::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Exterior::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Environment::request_add_member(prototype_name, role)) {
        return true;
    } else {
        if (role == "weather") {
            auto weather = Weather::make(prototype_name, get_event_repository(), get_game_data(), this, "weather");
            // set active status
            weather->set_active(this->is_active());
            // add member
            set_weather(std::move(weather));
            return true;
        }
    }
    
    return false;
}

bool Exterior::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent's implementation is unable
    if (Environment::request_release_member(member, role)) {
		return true;
	} else {
		// find, erase from member vector, release back to corresponding pool
		if (role == "weather") {
            if (m_weather) {
                Weather::release(std::move(m_weather));
            }
            return true;
        }
        
        return false;
	}
}

void Exterior::destroy_impl() {
    // call parent's implementation
    Environment::destroy_impl();
    
    // destroy members
    if (m_weather) {
        m_weather->destroy();
    }
    
    // clear member vectors/maps
	
    // reset values
	
}


#pragma mark - Getters/setters

const std::string& Exterior::get_type() const {
    return TYPE;
}

const std::unique_ptr<Weather>& Exterior::get_weather() const {
    return m_weather;
}

void Exterior::set_weather(std::unique_ptr<Weather> weather) {
    m_weather = std::move(weather);
}

void Exterior::get_scene_impl(std::unordered_map<std::string, std::string> &keys) const {
	// call parent implementation
	Environment::get_scene_impl(keys);
	
	// add <weatherdesc>
	keys["<weatherdesc>"] = Utils::to_lower(get_weather()->get_description());

}

const std::unordered_map<std::string, int>& Exterior::get_properties() const {
	// combine this environments properties with the weathers properties
	m_combined_properties = Game_object_common<Environment>::get_properties();
	if (m_weather) {
		m_combined_properties.insert(get_weather()->get_properties().begin(), get_weather()->get_properties().end());
    }
		
	return m_combined_properties;
}

Game_object* Exterior::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Environment::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		
	}
	
	return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Environment> Exterior::factory() {
    return std::unique_ptr<Exterior>(new Exterior());
}

void Exterior::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Environment::serialize_impl(data);
    
    // add every value to data as a strings
	
	// for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
	
	// serialize each member and add it to the data map
	data.emplace("weather", (m_weather) ? m_weather->serialize() : "");
}

bool Exterior::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Environment::deserialize_impl(key, value)) {
        return true;
    } else {
        // om hittar returnera true, om ej hitta returnera false
        if (key == "weather") {
			set_weather(Weather::make(value, get_event_repository(), get_game_data(), this, "weather"));
		} else {
            return false;
        }
		
        return true;
    }
}


#pragma mark - Actions

void Exterior::action_impl(unsigned tick) {
	// call parent implementation
	Environment::action_impl(tick);
	
}


#pragma mark - Interacting

bool Exterior::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Environment::interact_impl(command, value, remains, caller)) {
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

void Exterior::became_active() {
    // call parent's implementation
    Environment::became_active();
    
    // activate all members
    if (m_weather)
        m_weather->set_active(true);
}

void Exterior::became_inactive() {
    // call parent's implementation
    Environment::became_inactive();
    
    // deactivate all members
    if (m_weather)
        m_weather->set_active(false);
}


#pragma mark - Command functions

void Exterior::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Environment::examine_impl(singles, mapables, lists);

	// add weather
	if (m_weather)
		singles["Weather"] = m_weather->get_name();
}

void Exterior::property_changed() {
	Environment::property_changed();
	
}

