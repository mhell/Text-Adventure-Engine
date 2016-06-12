//
//  test_game_builder.h
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef TEST_BUILDER
#define TEST_BUILDER

#define private public
#define protected public

#include <assert.h>
#include <fstream>
#include "game_builder.h"
#include "actor.h"
#include "item.h"
#include "environment.h"
#include "exterior.h"
#include "game_data.h"

void test_builder() {
   
    /*
    std::cout << Item::json_prototypes().size() << std::endl;
    std::cout << std::get<0>(Item::json_prototypes()["Cutlery"]) << std::endl;
    */
    
    // testa prototyp skapning
    {
        // gamla antal prototyper
        auto orig_num_prototypes = World::num_prototypes() + Actor::num_prototypes() + Item::num_prototypes() + Environment::num_prototypes();
        
        // skapa prototyper
        auto world = Game_builder::load("test.json");
        
        // nytt antal ska vara gamla + 5
        auto new_num_prototypes = World::num_prototypes() + Actor::num_prototypes() + Item::num_prototypes() + Environment::num_prototypes();
        
        assert(new_num_prototypes == orig_num_prototypes +  8);
      
    }
    
    // testa om världen skapades och desarialiserades korrekt
    {
        // ladda värld, aktivera värld ("lazy deserialize")
        auto world = Game_builder::load("test.json");

        world->set_active(true);
        
        assert(world->get_name() == "World");
		
        assert(world->get_playables().size() == 2); // only current_env has been deserialized (Trenches)
		
        assert(world->get_time() == 12);
		
        assert(world->get_current_environment_name() == "Trenches");
        
        assert(world->m_environments.size() == 2);
        
        
        auto &environment = world->get_environments()[0];
        
        assert(environment->get_name() == "Trenches");
		
		assert(environment->get_coordinates() == "14°45′N 107°55′E");
		
		assert(environment->get_description() == "A damp jungle");
		
		assert(((Exterior*)environment.get())->get_weather()->get_name() == "Overcast"); // testa med dynamic cast istället?
        
		bool success;
		assert(((Exterior*)environment.get())->get_weather()->get_property("cold", success) == 10);
		
		assert(environment->get_exits().begin()->second.at("name") == "Enemy trenches");
		
        
        auto &item = environment->get_items()[0];
        
        assert(item->get_name() == "Magic spoon");
        
        
        auto &actor = environment->get_actors()[0];
        
        assert(environment->m_actors.size() == 4);
        
        assert(actor->get_name() == "Giant troll");
		
        assert(actor->get_property("blood", success) == 100);
		
        assert(actor->get_property("strength", success) == 5);
		
        assert(actor->get_health() == 500);
        
		
        auto &actor_item = actor->get_items()[0];
        
        assert(actor_item->get_name() == "Smith & Wesson Of Greater Pain");
        
        assert(actor_item->get_property("weight", success) == 4);
	
		
		assert(actor->m_event_repository->m_events.size() == 3);
		
		assert(actor->m_actions.size() == 1);
		
		assert(actor->m_actions[0] == "Self defence");
		
		
		assert(actor->m_reactions.size() == 1);
		
		auto reaction = actor->m_reactions.begin();
		
		assert(reaction->second["event"] == "Bleed");
        
        
        auto item_reaction = actor_item->m_reactions.begin();
        assert(item_reaction->second["event"] == "Rust firearm");
		
        
        world->destroy();
        
    }
    
    // testa om destroyad läggs i pool, laddas ur pool med icke skräpvärden kvar. Gamla värden ska resettas
    {
        // ladda värld, aktivera värld ("lazy deserialize")
        auto world = Game_builder::load("test.json");
        assert(world->is_active() == false);
        assert(world->m_deserialized == false);
        assert(world->m_environments.size() == 0);
        world->set_active(true);
        
        // kolla värden
        assert(world->get_name() == "World");
        assert(world->is_active() == true);
        assert(world->m_deserialized == true);
        assert(world->m_environments.size() == 2);
        
        // ändra värden
        world->set_name("hej");
        
        // kolla värden
        assert(world->get_name() == "hej");
        
        // destroya värld
        world->destroy();
        
        // ladda värld, aktivera värld (utan att aktivera)
        world = Game_builder::load("test.json");
        
        // kolla värden (inkl vectorer). Bör ha resettats
        assert(world->is_active() == false);
        assert(world->m_deserialized == false);
        assert(world->m_environments.size() == 0);
        //assert(world->get_playables().size() == 0);
        
    }
    
    // testa om världen seralialiseras korrekt
    {
        // ladda värld, aktivera värld ("lazy deserialize")
        auto world = Game_builder::load("test.json");
        world->set_active(true);
        
        // spara värld
        Game_builder::save(*world, "test_save.json");
        
        // ladda sparad värld
        world = Game_builder::load("test_save.json");
        world->set_active(true);
        
    }
  
    std::cout << "Game builder tests ok" << std::endl;
    
}

#endif