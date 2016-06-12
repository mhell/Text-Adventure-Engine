//
//  Serializable.cpp
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "serializable.h"
#include "json_tools.h"

using namespace lab3;


bool Serializable::is_active() const {
    return m_active;
}

void Serializable::set_active(bool state) {
    m_active = state;
	
    if (m_active) {
        // deserialize if needed. Uses stored definition
        if (!m_deserialized) {
            deserialize(m_json_stored);
        }
        
        // call virtual implementation
        became_active();
    } else {
        // call virtual implementation
        became_inactive();
    }
}

void Serializable::activate_selective(const std::vector<std::string> &keys) {
	// do nothing if all already deserialized
	if (!m_deserialized) {
		// deserialize selected keys
		for (const std::string &key : keys) {
			deserialize_impl(key, JSON_tools::find_value_in_object(m_json_stored, key));
		}
	}
}

std::string Serializable::serialize() const {
	// if not yet deserialized; return the m_json_stored string
	if (!m_deserialized) {
		return m_json_stored;
    // else call virtual implementation to populate a data map, then convert and return it as a json object
	} else {
		std::unordered_map<std::string, std::string> data;
		serialize_impl(data);
		return JSON_tools::to_json_object(data);
	}
}

void Serializable::deserialize(const std::string &json) {
    // deserialize if active, else store the json (to be used as argument when activated)
    if (is_active()) {
        // convert object into a map
        std::unordered_map<std::string, std::string> map;
        JSON_tools::to_std_map(json, map);
        
        // call virtual implementation for all key-value entrys in the map
        for (auto entry : map) {
			const std::string &key = entry.first;
            std::string &value = entry.second;
            
            // if value is an array; convert into a c++ vector and send each entry separately
            if (JSON_tools::is_json_array(value)) {
                std::vector<std::string> vec = JSON_tools::to_std_vector(value);
                for (auto &elem : vec) {
                    deserialize_impl(key, elem);
				}
            // else if object or regular member
			} else {
                deserialize_impl(key, value);
            }
        }
        
        m_deserialized = true;
    } else {
		// store json for deserialization when activated
        m_json_stored = json;
    }
}

const bool Serializable::is_deserialized() const {
	return m_deserialized;
}

void Serializable::reset() {
    m_active = false;
    m_deserialized = false;
}


