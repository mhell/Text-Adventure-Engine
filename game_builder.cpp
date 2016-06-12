//
//  Game_builder.cpp
//  Lab3
//
//  Created by Mattias Hellman on 18/02/14.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <unordered_map>
#include "game_builder.h"
#include "json_tools.h"
#include "environment.h"
#include "actor.h"
#include "item.h"
#include "weather.h"

namespace {
	// map of add_prototype functions (one for each root type)
	const std::unordered_map<std::string, void(*)(const std::string&)> add_prototype_func {
		{"World", World::add_prototype},
		{"Environment", Environment::add_prototype},
		{"Actor", Actor::add_prototype},
		{"Item", Item::add_prototype},
		{"Weather", Weather::add_prototype},
		{"Event", Event::add_prototype}
	};
}

using namespace lab3;


std::unique_ptr<World> Game_builder::load(const std::string &file_path) {
    std::ifstream ifs(file_path);
    
    if (ifs) {
        std::string data_string;
        std::getline(ifs, data_string, '\0');
        ifs.close();
        
        // add all object definitions as prototypes
        add_prototypes(data_string);
         
        // make world
        return World::make(data_string, std::make_shared<Event_repository>());
    } else {
        throw std::runtime_error("Error loading file " + file_path);
    }
}

void Game_builder::add_prototypes(const std::string &data) {
    // get all objects from the json definition
    std::vector<std::string> objects;
    JSON_tools::get_objects(data, objects);
    
    // save each json object as a prototype
    for (auto obj : objects) {
	    // get root name
        std::string root = JSON_tools::find_value_in_object(obj, "root");

        // add the prototype to correct class's prototype pool (according to 'root' value)
		// don't add if there is no root specified, i.e. non class object
		if (root.length()) {
            add_prototype_func.at(root)(obj);
        }
    }
}

void Game_builder::save(const World &world, const std::string &file_path) {
    std::ofstream ofs(file_path);
    ofs << world.serialize();
    ofs.close();    
}








