//
//  event.cpp
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "event.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Event::TYPE = "Event";

bool Event::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

std::unique_ptr<Event> Event::make(const std::string &name_or_json) {
	std::unique_ptr<Event> object;
	
	// call make_json if name_or_json is a json object
	if (JSON_tools::is_json_object(name_or_json)) {
		// make the prototype object
		object = Event::acquire(JSON_tools::find_value_in_object(name_or_json, "prototype"));
		
		// merge the prototype's definition with the new, then apply the merged definition
		std::string merged = JSON_tools::merge_json(object->serialize(), name_or_json);
		
		// deserialize
		object->deserialize(merged);
	} else {
		// acquire object from prototype pool
		object = Event::acquire(name_or_json); // korrekt skrivet. Se Templates: Calling method of template base class
	}
	
	// pre activate name
	object->activate_selective( {"name"} );
	
	return object;
}

#pragma mark - Serializing

std::unique_ptr<Event> Event::factory() {
    return std::unique_ptr<Event>(new Event());
}

void Event::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent implementation
    
    // add each value to the data map as strings (with std strings as json strings)
    data.emplace("name", get_name());
    data.emplace("description", get_description());
    data.emplace("commands", get_commands());
    data.emplace("prototype", get_type()); // the deserialized object have recieved all data from its prototype (and has become) it's prototype. It's new prototype is therefore the base type e.g. "Troll"
    data.emplace("root", get_type()); // root = type
    
	// for every member map: convert the map to a json object and add it to the data
	
    // for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
	
}

bool Event::deserialize_impl(const std::string &key, const std::string &value) {
	if (key == "name") {
        m_name = value;
	} else if (key == "description") {
		m_description = value;
	} else if (key == "commands") {
		m_commands = value;
	} else {
        return false;
    }
    
    return true;
}

void Event::became_active() {
    // activate all members
    
}

void Event::became_inactive() {
    // deactivate all members
    
}

