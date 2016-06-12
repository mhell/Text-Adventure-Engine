//
//  world.cpp
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "World.h"
#include "utils.h"

using namespace lab3;


#pragma mark - Static initializers

const std::string World::ROOT = "World";

const std::string World::TYPE = "World";

bool World::registered = register_default(TYPE, factory);

std::vector<std::string> World::m_time_strings{"Night", "Morning", "Day", "Evening"};

std::vector<std::string> World::m_long_time_strings{"Midnight", "Night", "Early morning", "Morning", "Midday", "Late afternoon", "Early evening", "Late evening"};


#pragma mark - Construction/destruction

bool World::request_add_member(const std::string &prototype_name, const std::string &role) {
    // if parent call return false: add members based on the role argument
    if (Game_object_common<World>::request_add_member(prototype_name, role)) {
        return true;
    } else {
        if (role == "environment") {
            auto env = Environment::make(prototype_name, get_event_repository(), this,  this, "environment");
            // set active status
            env->set_active(this->is_active());
            // add to member vector
            add_environment(std::move(env));
            return true;
        }
    }
    
    return false;
}

bool World::request_release_member(const std::string &member, const std::string &role) {
    // try to find and release only if parent implementation is unable
    if (Game_object_common<World>::request_release_member(member, role)) {
		return true;
	} else {
		// find, release back to corresponding pool, erase from member vector
		if (role == "environment") {
			auto it = std::find_if(m_environments.begin(), m_environments.end(), [&member](const std::unique_ptr<Environment> &env){return Utils::to_lower(env->get_name()) == Utils::to_lower(member);});
			if (it != m_environments.end()) {
				Environment::release(std::move(*it));
				m_environments.erase(it);
                return true;
			}
		}
        
        return false;
	}
}

void World::destroy_impl() {
    // call parent's implementation
    Game_object_common<World>::destroy_impl();

    // destroy members
    while (m_environments.begin() != m_environments.end()) {
        m_environments[0]->destroy();
    }
    
    // clear member vectors/maps (behövs ej - görs av destroy?)
    m_environments.clear();
	m_playables.clear();
	
    // reset values
    m_time = 0;
    m_current_environment.clear();
}


#pragma mark - Getters/setters

const std::string& World::get_root() const {
    return ROOT;
}

const std::string& World::get_type() const {
    return TYPE;
}

unsigned World::get_time() const {
    return m_time;
}

void World::set_time(unsigned time) {
    m_time = time % 24; // 24 hour clock
}

const std::string& World::get_time_string() const {
	return m_time_strings[(double)m_time / 24 * m_time_strings.size()];
}

const std::string& World::get_long_time_string() const {
	return m_long_time_strings[(double)m_time / 24 * m_long_time_strings.size()];
}

Environment* World::get_current_environment() const {
	return get_environment(get_current_environment_name());
}

const std::string& World::get_current_environment_name() const {
    return m_current_environment;
}

void World::set_current_environment(const std::string &name) {
	// if deserialized; toggle active environment
	if (is_deserialized()) {
		auto new_env = get_environment(name);
		auto old_env = get_current_environment();
		
		// transfer playable actors to the new environment
		for (auto playable : get_playables()) {
			new_env->add_actor(old_env->remove_actor(playable->get_name()));
		}
		
		// deactivate old
		get_current_environment()->set_active(false);
		
		// assign new previous environment
		m_prev_environment = m_current_environment;
		
		// assign new current environment
		m_current_environment = name;
		
		// activate new
		new_env->set_active(true);
	} else {
		m_current_environment = name;
	}
}

const std::string& World::get_prev_environment_name() const {
	return m_prev_environment;
}

Environment* World::get_environment(const std::string &name) const {
	auto it = std::find_if(m_environments.begin(), m_environments.end(), [&name](const std::unique_ptr<Environment> &env){return env->get_name() == name;});
	
	if (it != m_environments.end()) {
		return it->get();
	} else {
		throw std::out_of_range("Environment " + name + " not found");
	}
}

const std::vector<std::unique_ptr<Environment>>& World::get_environments() const {
    return m_environments;
}

void World::add_environment(std::unique_ptr<Environment> environment) {
    m_environments.emplace_back(std::move(environment));
}

const std::vector<Actor*>& World::get_playables() const {
	// recalculate if any update to game objects has been made
	if (!m_playables_updated) {
		update_actors();
		m_playables_updated = true;
	}
	return m_playables;
}

const std::vector<Actor*>& World::get_enemies() const {
	// recalculate if any update to game objects has been made
	if (!m_playables_updated) {
		update_actors();
		m_playables_updated = true;
	}
	return m_enemies;
}

Actor* World::get_playable(const std::string &name) {
	auto it = std::find_if(m_playables.begin(), m_playables.end(), [&name](const Actor *actor){return Utils::to_lower(actor->get_name()) == Utils::to_lower(name);});
	if (it != m_playables.end()) {
		return *it;
	}
	
	return nullptr;
}


Actor* World::get_enemy(const std::string &name) {
	auto it = std::find_if(m_enemies.begin(), m_enemies.end(), [&name](const Actor *actor){return Utils::to_lower(actor->get_name()) == Utils::to_lower(name);});
	if (it != m_enemies.end()) {
		return *it;
	}
	
	return nullptr;
}


Game_object* World::get_member(const std::string &member, const std::string &role) const {
	// call parent's implementation
	auto *game_obj = Game_object_common<World>::get_member(member, role);
	
	// return if found in parent's implementation otherwise search in this
	if (game_obj) {
		return game_obj;
	} else {
		// search environments
		if (!role.length() || Utils::to_lower(role) == "environment") {
			auto it = std::find_if(m_environments.begin(), m_environments.end(), [&member](const std::unique_ptr<Environment> &env){return Utils::to_lower(env->get_name()) == Utils::to_lower(member);});
			if (it != m_environments.end()) {
				return it->get();
			}
		}
	}
	
	return nullptr;
}

void World::update() {
	m_playables_updated = false;
}

void World::update_actors() const {
	m_playables.clear();
	m_enemies.clear();
	
	// if current environment is set; copy all playables, else null the vector
	if (get_current_environment_name().length()) {
		auto &actors = get_current_environment()->get_actors();
		
		for (auto &actor : actors) {
			if (actor->is_playable()) {
				m_playables.push_back(actor.get());
			} else {
				m_enemies.push_back(actor.get());
			}
		}		
	}
}


#pragma mark - Serializing

std::unique_ptr<World> World::factory() {
    return std::unique_ptr<World>(new World());
}

void World::serialize_impl(std::unordered_map<std::string, std::string> &data) const {
    // call parent's implementation
    Game_object_common<World>::serialize_impl(data);
    
    // add every property to data as a strings
    std::ostringstream convert;
    convert << get_time();
    data.emplace("time", convert.str());
    data.emplace("current environment", get_current_environment_name());
    
    // for every member vector: serialize elements into a string vector, convert to a json array and add it to the data
    std::vector<std::string> env_vec;
    for (auto &env : m_environments) {
        env_vec.emplace_back(env->serialize());
    }
    data.emplace("environments", JSON_tools::to_json_array(env_vec));
    
    data.emplace("defaults", JSON_tools::to_json_array(m_defaults));
}

bool World::deserialize_impl(const std::string &key, const std::string &value) {
    // run deserialize only if parent class is unable to deserialize 'key'
    if(Game_object_common<World>::deserialize_impl(key, value)) {
        return true;
    } else {
        if (key == "time") {
            set_time(std::stoi(value));
        } else if (key == "environments") {
            add_environment(Environment::make(value, get_event_repository(), this,  this, "environment"));
        } else if (key == "current environment") {
            set_current_environment(value);
        } else if (key == "defaults") {
            m_defaults.push_back(value);
        } else {
            return false;
        }
        
        return true;
    }
}


#pragma mark - Actions

void World::action_impl(unsigned tick) {
	// call parent implementation
	Game_object_common<World>::action_impl(tick);
	
	// increment time
	
	// add current properties to current environment
	
	// propagate action to current environment
	get_current_environment()->action(tick);
}


#pragma mark - Interacting

bool World::interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) {
	// run interact_impl only if parent implementation is unable to find the command
	if (Game_object_common<World>::interact_impl(command, value, remains, caller)) {
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

void World::became_active() {
    // call parent's implementation
    Game_object_common<World>::became_active();
	
	// activate only current environment (if set)
	if (get_current_environment_name().length()) {
		get_current_environment()->set_active(true);
	}
}


void World::became_inactive() {
    // call parent's implementation
    Game_object_common<World>::became_inactive();
    
    // deactivate all members?
    
}


#pragma mark - Command functions

void World::examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const {
	// call parent's implementation
    Game_object_common<World>::examine_impl(singles, mapables, lists);
	
	// add time
	singles["Time"] = get_long_time_string();
	
	// add environments
	for (auto &env : get_environments()) {
		lists["Environments"].emplace_back(env->get_name());
	}
}

void World::property_changed() {
	// call parent's implementation
    Game_object_common<World>::property_changed();

}




