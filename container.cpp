//
//  container.cpp
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "container.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string Container::TYPE = "Container";

bool Container::registered = register_default(TYPE, factory);


#pragma mark - Construction/destruction

bool Container::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Item::request_add_member(prototype_name, role)) {
        return true;
    } else {
        if (role == "item") {
            auto item = Item::make(prototype_name, get_event_repository(), get_game_data(), this, "item");
            // set active status
            item->set_active(this->is_active());
            // add to member vector
            add_item(std::move(item));
            return true;
        }
    }
    
    return false;
}

bool Container::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Item::request_release_member(member, role)) {
        return true;
    } else {
        // find, release back to corresponding pool, erase from member vector
        if (role == "item") {
            auto it = std::find_if(m_items.begin(), m_items.end(), [&member](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(member);});
            if (it != m_items.end()) {
                Item::release(std::move(*it));
                m_items.erase(it);
                return true;
            }
        }
        
        return false;
    }
}

void Container::destroy_impl() {
    // call parent's implementation
    Item::destroy_impl();
    
    // destroy members
    while (m_items.begin() != m_items.end()) {
        m_items[0]->destroy();
    }
    
    // clear member vectors/maps (behövs ej - görs av destroy?)
    m_items.clear();
    
    // reset values
    
}


#pragma mark - Getters/setters

const std::string& Container::get_type() const {
    return TYPE;
}

Game_object* Container::get_member(const std::string &member, const std::string &role) const {
    // call parent's implementation
    auto *game_obj = Item::get_member(member, role);
    
    // return if found in parent's implementation otherwise search in this
    if (game_obj) {
        return game_obj;
    } else {
        // search items
        if (!role.length() || Utils::to_lower(role) == "item") {
            auto it_items = std::find_if(m_items.begin(), m_items.end(), [&member](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(member);});
            if (it_items != m_items.end()) {
                return it_items->get();
            }
        }
    }
    
    return nullptr;
}

const std::vector<std::unique_ptr<Item>>& Container::get_items() const {
    return m_items;
}

bool Container::add_item(std::unique_ptr<Item> item) {
    m_items.emplace_back(std::move(item));
    return true;
}

std::unique_ptr<Item> Container::remove_item(const std::string &name) {
    auto it = std::find_if(m_items.begin(), m_items.end(), [&name](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(name);});
    if (it != m_items.end()) {
        // transfer ownership
        auto ptr = std::move(*it);
        
        // remove from vector
        m_items.erase(it);
        
        return ptr;
    }
    
    return nullptr;
}

Item* Container::get_item(const std::string &name) const {
    auto it = std::find_if(m_items.begin(), m_items.end(), [&name](const std::unique_ptr<Item> &item){return Utils::to_lower(item->get_name()) == Utils::to_lower(name);});
    if (it != m_items.end()) {
       return it->get();
    }
    
    return nullptr;
}


#pragma mark - Serializing

std::unique_ptr<Item> Container::factory() {
    return std::unique_ptr<Container>(new Container());
}

void Container::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Item::serialize_impl(data);
    
    // for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
    std::vector<std::string> vec;
    for (auto &item : m_items) {
        vec.emplace_back(item->serialize());
    }
    data.emplace("items", JSON_tools::to_json_array(vec));
}

bool Container::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Item::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "items") {
            add_item(Item::make(value, get_event_repository(), get_game_data(), this, "item"));
        } else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void Container::action_impl(unsigned tick) {
    // call parent implementation
    Item::action_impl(tick);
    
}


#pragma mark - Interacting

bool Container::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
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

void Container::became_active() {
    // call parent's implementation
    Item::became_active();
    
    // activate all members
    for (auto &item : m_items) {
        item->set_active(true);
    }
}

void Container::became_inactive() {
    // call parent's implementation
    Item::became_inactive();
    
    // deactivate all members
    for (auto &item : m_items) {
        item->set_active(false);
    }
}


#pragma mark - Command functions

void Container::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
    // call parent's implementation
    Item::examine_impl(singles, mapables, lists);
    
    // add items
    for (auto &item : get_items()) {
        lists["Items"].emplace_back(item->get_name());
    }
}

void Container::property_changed() {
    // call parent's implementation
    Item::property_changed();
    
}


