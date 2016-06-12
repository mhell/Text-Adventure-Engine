//
//  test_game_object.h
//  Lab3
//
//  Created by Mattias Hellman on 7/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_TEST_GAME_OBJECT
#define LAB3_TEST_GAME_OBJECT

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
#include "container.h"

void test_game_object() {
    
    // testa change_property
	{
		// ladda värld, aktivera värld ("lazy deserialize")
		auto world = Game_builder::load("test.json");
		world->set_active(true);
		
		// hämta actorn med properties
		auto environment = world->get_current_environment();
		
		auto &actor = environment->get_actors()[0];
		
		bool success;
		
		assert(actor->get_property("blood", success) == 100);
		
		assert(actor->get_property("strength", success) == 5);
		
		assert(actor->get_health() == 500);
		
		// ändra property värden i actorn
		
		bool found = actor->change_property("blood", "-50", Troll());
		
		assert(found);
		
		assert(actor->get_property("blood", success) == 50);
		
		assert(actor->get_health() == 250);

	}
	
	// testa destroy
	{
		// ladda värld, aktivera värld ("lazy deserialize")
		auto world = Game_builder::load("test.json");
		world->set_active(true);
		
		// hämta environment
		Environment *environment = world->get_current_environment();

		// check member counts
		assert(environment->get_items().size() == 3);
		assert(environment->get_actors().size() == 4);
		
		// destroy the environment (releases back to pool - pointer still valid)
		environment->destroy();
		
		// check new member counts
		assert(environment->get_items().size() == 0);
		assert(environment->get_actors().size() == 0);
		
	}
	
	// testa döda actor (health = 0)
	{
		// ladda värld, aktivera värld ("lazy deserialize")
		auto world = Game_builder::load("test.json");
		world->set_active(true);
		
		// hämta actorn
		auto environment = world->get_current_environment();
		auto &actor = environment->get_actors()[0];
		
		// döda actorn
		auto actor_count = environment->get_actors().size();
		actor->change_property("blood", "-100", Troll());
		assert(environment->get_actors().size() == actor_count - 1);
	
	}
	
	// testa döda actor (negativ health & endast enemies)
	{
		// ladda värld, aktivera värld ("lazy deserialize")
		auto world = Game_builder::load("test.json");
		world->set_active(true);
		
		// hämta actorn
		auto &actor = world->get_enemies()[0];
		
		// döda actorn
		auto enemies_count = world->get_enemies().size();
		actor->change_property("blood", "-101", Troll());
		assert(world->get_enemies().size() == enemies_count - 1);
		
	}
	
	// testa byta rum
	{
		// ladda värld, aktivera värld ("lazy deserialize")
		auto world = Game_builder::load("test.json");
		world->set_active(true);
		
		// hämta current environment
		auto environment = world->get_current_environment();
		
		// get name of exit
		std::string exit_name = environment->get_exits().at("northeast").at("name");
		
		// try to exit with enemies alive
		assert(!environment->interact("exit : northeast"));
		
		// döda fiender
		while (world->get_enemies().size()) {
			world->get_enemies()[0]->destroy();
		}
		
		// try to exit with enemies dead
		assert(environment->interact("exit : northeast"));
		
		// kolla om current_environment byts till vald environment
		assert(world->get_current_environment()->get_name() == exit_name);

	}
 
    // testa drop & pick up & give item
    {
        // ladda värld, aktivera värld ("lazy deserialize")
        auto world = Game_builder::load("test.json");
        world->set_active(true);
        
        // hämta actorn/environment
        auto &actor1 = world->get_playables()[0];
        auto &actor2 = world->get_playables()[1];
        auto environment = world->get_current_environment();
        
        // kolla item list size
        assert(actor1->get_items().size() == 1);
        assert(actor2->get_items().size() == 1);
        assert(environment->get_items().size() == 3);
        
        // hämta items hos actors
        auto item_1 = actor1->get_items()[0].get();
        assert(item_1->get_owner()->get_name() == actor1->get_name());
        
        // droppa item
        actor1->interact("drop : " + item_1->get_name());
        actor2->interact("drop : all");
        assert(item_1->get_owner()->get_name() == environment->get_name());
        
        // kolla item list size
        assert(actor1->get_items().size() == 0);
        assert(actor2->get_items().size() == 0);
        assert(environment->get_items().size() == 5);
        
        // pick up item
        actor1->interact("pick up : " + item_1->get_name());
        assert(actor1->get_items().size() == 1);
        assert(environment->get_items().size() == 4);
        assert(item_1->get_owner()->get_name() == actor1->get_name());
        
        // give item
        actor1->interact("give : " + item_1->get_name() + " : to : " + actor2->get_name());
        assert(actor1->get_items().size() == 0);
        assert(actor2->get_items().size() == 1);
        assert(item_1->get_owner()->get_name() == actor2->get_name());
    }
    
    // testa containers
    {
        // ladda värld, aktivera värld ("lazy deserialize")
        auto world = Game_builder::load("test.json");
        world->set_active(true);
        
        // hämta container
        auto cont_item = world->get_current_environment()->get_member("Treasure chest", "item");
        
        // kolla typ och item list size
        assert(cont_item->get_type() == "Container");
        assert(static_cast<Container*>(cont_item)->get_items().size() == 2);
        
        // testa recursive pick-up
        auto &playable = world->get_playables()[0];
        auto pre_pickup = playable->get_items().size();
        playable->interact("pick up : Treasure chest : Backpack : Firearm");
        assert(playable->get_items().size() == pre_pickup + 1);
        
    }
    
    // testa transform (request_add_member etc)
    
    
    // testa rename
	

	std::cout << "Game object tests ok" << std::endl;
	
}


#endif
