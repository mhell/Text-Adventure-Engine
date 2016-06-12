//
//  prototype_pool.cpp
//  Lab3
//
//  Created by Mattias Hellman on 16/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#include "json_prototype_pool.h"
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <exception>
#include <sstream>
#include "JSON_tools.h"
#define m_json_prototypes json_prototypes()
#define m_proto_makers proto_makers()

using namespace lab3;


#pragma mark - Static initializers

template <typename T>
std::unordered_map<std::string, std::list<std::unique_ptr<T>>> JSON_prototype_pool<T>::m_object_pool;

template <typename T>
bool JSON_prototype_pool<T>::m_rand_init = false;


#pragma mark - Implementation

template <typename T>
void JSON_prototype_pool<T>::add_prototype(const std::string &json) {
    // get the object's prototype
    std::string prototype = JSON_tools::find_value_in_object(json, "prototype");
    
    // get the prototype's json definition
    std::string proto_json = json_of(prototype);
    
    // merge the prototype's definition with the object's definition
    std::string merged = JSON_tools::merge_json(proto_json, json);
	
	// Get name from the new definition
	std::string name = JSON_tools::find_value_in_object(json, "name");
	
    // add a new prototype only if a name is given for the new definition
    if (name.length()) {
        std::string type = class_of(JSON_tools::find_value_in_object(merged, "prototype"));
        factory_ptr factory = factory_of(JSON_tools::find_value_in_object(merged, "prototype"));
        
        // add new prototype to map of prototypes (overwrites - use map::insert if overwriting undesired)
        m_json_prototypes[name] = std::make_tuple(std::move(type), merged, std::move(factory));
    }
}

template <typename T>
bool JSON_prototype_pool<T>::register_default(const std::string &type, factory_ptr factory, const std::string &json) {
    // default prototype name = type (i.e. "Troll"). If json not defined, define at least name
    m_json_prototypes.insert({type, make_tuple(type, json.length() ? json : "{\n\t\"name\" : \"" + type + "\"\n}", factory)});
    
    return true;
}

template <typename T>
std::unique_ptr<T> JSON_prototype_pool<T>::acquire(const std::string &prototype_name) {
    // if the argument matches a constant (e.g. RANDOM) use the corresponding method to get a json prototype, then return it serialized
    auto pm_it = m_proto_makers.find(prototype_name);
    if(pm_it != m_proto_makers.end()) {
        auto& prot_tuple = pm_it->second(prototype_name);
        // create an instance with the factory method and deserialize, then return the result
        std::unique_ptr<T> object = std::get<FACTORY>(prot_tuple)();
        object->deserialize(std::get<JSON>(prot_tuple));
        return object;
    }

    // else try to find the prototype (tuple describing the prototype)
    auto jp_it = m_json_prototypes.find(prototype_name);
    
    // if found then use it; if not then use the failsafe policy
    auto& prot_tuple = (jp_it != m_json_prototypes.end()) ? (*jp_it).second : m_proto_makers.at(FAILSAFE)(prototype_name);
    
    // if an instance of the class exists in the pool: deserialize it with the json string, remove it from the pool, then return it
    if (m_object_pool.count(std::get<TYPE>(prot_tuple)) && m_object_pool.at(std::get<TYPE>(prot_tuple)).size()) {
        auto& type_pool = m_object_pool.at(std::get<TYPE>(prot_tuple));
        std::unique_ptr<T> pooled_ptr = std::move(type_pool.back());
        type_pool.pop_back();
        pooled_ptr->deserialize(std::get<JSON>(prot_tuple));
        return pooled_ptr;
    }

    // if not in pool: create an instance with the factory method and deserialize it, then return the result
    std::unique_ptr<T> object = std::get<FACTORY>(prot_tuple)();
    object->deserialize(std::get<JSON>(prot_tuple));
    return object;
}

template <typename T>
void JSON_prototype_pool<T>::release(std::unique_ptr<T> object) {
    m_object_pool[object->get_type()].push_back(std::move(object));
}

template <typename T>
std::tuple<std::string, std::string, typename JSON_prototype_pool<T>::factory_ptr>& JSON_prototype_pool<T>::random_prototype(const std::string &filter) {
    // initialize random seed
    if (!m_rand_init) {
        srand((unsigned)time(NULL));
        m_rand_init = true;
    }
    
    // start value
    auto p_it(m_json_prototypes.begin());
    
    // iterate randomly forward
    std::advance(p_it, rand() % m_json_prototypes.size());
    auto start_pos = p_it;
    
    // repeat until found random spawnable prototype or returned to start pos
    do {
        // get prototype tuple
        auto& prot_tuple = (*p_it).second;
        
        // check if random spawnable prototype found
        if (JSON_tools::find_value_in_object(std::get<JSON>(prot_tuple), "inrandom") == "1") {
            return prot_tuple;
        } else {
            // advance iterator
            ++p_it;
        
            // reset iterator if beyond size of prototypes collection
            if (p_it == m_json_prototypes.end())
                p_it = m_json_prototypes.begin();
        }
    } while (p_it != start_pos);
    
    // no random spawnables found; use failsafe
    return m_proto_makers.at(FAILSAFE)("RANDOM");
}

template <typename T>
std::tuple<std::string, std::string, typename JSON_prototype_pool<T>::factory_ptr>&  JSON_prototype_pool<T>::throw_exception(const std::string &prototype_name) {
    // throw an exception...
    if (!prototype_name.length()) {
        throw std::invalid_argument("Prototype not specified or zero length");
    } else {
        throw std::invalid_argument("Prototype '" + prototype_name + "' not found");
    }
}

template <typename T>
std::size_t JSON_prototype_pool<T>::num_prototypes() {
    return m_json_prototypes.size();
}

template <typename T>
std::size_t JSON_prototype_pool<T>::pool_size(const std::string &type) {
    if (m_object_pool.count(type))
        return m_object_pool.find(type)->second.size();
    else return 0;
}

template <typename T>
typename JSON_prototype_pool<T>::factory_ptr JSON_prototype_pool<T>::factory_of(const std::string &prototype_name) {
    auto pm_it = m_proto_makers.find(prototype_name);
    
    // if argument matches entry in maker function map (e.g. RANDOM)
    if(pm_it != m_proto_makers.end()) {
        auto& prot_tuple = pm_it->second(prototype_name);
        return std::get<FACTORY>(prot_tuple);
    // else try to find the prototype
    } else {
        try {
            auto& prot_tuple = m_json_prototypes.at(prototype_name);
            return std::get<FACTORY>(prot_tuple);
        } catch (std::out_of_range &e) {
            throw std::invalid_argument("Prototype '" + prototype_name + "' not found");
        }
    }
}

template <typename T>
std::string& JSON_prototype_pool<T>::json_of(const std::string &prototype_name) {
    auto pm_it = m_proto_makers.find(prototype_name);
    
    // if argument matches entry in maker function map (e.g. RANDOM)
    if(pm_it != m_proto_makers.end()) {
        auto& prot_tuple = pm_it->second(prototype_name);
        return std::get<JSON>(prot_tuple);
    // else try to find the prototype
    } else {
        try {
            auto& prot_tuple = m_json_prototypes.at(prototype_name);
            return std::get<JSON>(prot_tuple);
        } catch (std::out_of_range &e) {
            throw std::invalid_argument("Prototype '" + prototype_name + "' not found");
        }
    }
}

template <typename T>
std::string& JSON_prototype_pool<T>::class_of(const std::string &prototype_name) {
    auto pm_it = m_proto_makers.find(prototype_name);
    
    // if argument matches entry in maker function map (e.g. RANDOM)
    if(pm_it != m_proto_makers.end()) {
        auto& prot_tuple = pm_it->second(prototype_name);
        return std::get<TYPE>(prot_tuple);
    // else try to find the prototype
    } else {
        try {
            auto& prot_tuple = m_json_prototypes.at(prototype_name);
            return std::get<TYPE>(prot_tuple);
        } catch (std::out_of_range &e) {
            throw std::invalid_argument("Prototype '" + prototype_name + "' not found");
        }
    }
}
    
template <typename T>
std::unordered_map<std::string, std::tuple<std::string, std::string, typename JSON_prototype_pool<T>::factory_ptr>>& JSON_prototype_pool<T>::json_prototypes() {
	// key = name, value = tuple<type, json definition, factory pointer> 
    static std::unordered_map<std::string, std::tuple<std::string, std::string, factory_ptr>> prototype_map;
    return prototype_map;
}

template <typename T>
std::unordered_map<std::string, typename JSON_prototype_pool<T>::Proto_getter_ptr>& JSON_prototype_pool<T>::proto_makers() {
    static std::unordered_map<std::string, Proto_getter_ptr> proto_makers_map{ {RANDOM_PROTOTYPE, random_prototype},
                                                                               {THROW_EXCEPT, throw_exception} };
    return proto_makers_map;
}




