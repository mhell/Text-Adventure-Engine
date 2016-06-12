//
//  test_json_prototype_pool.h
//  Lab3
//
//  Created by Mattias Hellman on 5/2/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_TEST_JSON_PROTOTYPE_POOL_H
#define LAB3_TEST_JSON_PROTOTYPE_POOL_H

#define private public
#define protected public

#include <assert.h>
#include "actor.h"
#include "world.h"
#include "zombie.h"
#include "troll.h"
#include "game_object.h"

void test_json_prototype_pool() {
    
    // testa acquire
    {
        std::shared_ptr<World> world = World::acquire("World");
        world->set_active(true); // lazy deserialize, måste aktiveras
        assert(world->get_name() == "World");
        
        // testa att orginal ej ändras (är endast en json sträng...)
        world->set_name("Jan");
        world = World::acquire("World");
        world->set_active(true);
        assert(world->get_name() == "World");
    }
    
    // testa acquire med felaktigt namn. Ska kasta exception
    {
        try {
            std::shared_ptr<Actor> actor = Actor::acquire("no_exist");
            assert(false);
        } catch (std::exception&) {
            // should fail
            assert(true);
        }
    }
    
    // testa release to pool, sen hämta ur pool
    {
        std::unique_ptr<Actor> actor = Actor::acquire("Troll");
        
        assert(Actor::pool_size("Troll") == 0);
        Actor::release(std::move(actor));
        assert(Actor::pool_size("Troll") == 1); // sparad i poolen
        actor = Actor::acquire("Troll"); // hämtad ur poolen
        assert(Actor::pool_size("Troll") == 0);
    }

    // testa skapa ny prototyp
    {
        /*
        std::size_t num_protos = Actor::num_prototypes();
        
        // add new prototype
        Actor::add_prototype(JSONJSON);
        
        assert(Actor::num_prototypes() == num_protos+1);
        
        // add a new prototype med samma name (ska då ej läggas till)
        Actor::add_prototype(std::make_shared<Troll>("Big_troll"));
        
        assert(Actor::num_prototypes() == num_protos+1);
        */
    }
    
    // testa RANDOM creation
    {
        // lägg till random spawnable prototyp
        std::string inrandom = "{\n"
            "\t\"inrandom\" : 1,\n"
            "\t\"prototype\" : \"Troll\",\n"
            "\t\"root\" : \"Actor\",\n"
            "\t\"name\" : \"Troll\",\n"
        "},";
        
        Actor::add_prototype(inrandom);
        
        std::shared_ptr<Actor> actor = Actor::acquire("RANDOM"); // Actor::acquire("Random"); ska kasta exception
        
        assert(actor->get_name() == "Troll" || actor != nullptr);
    }
    
    std::cout << "JSON prototype pool tests ok" << std::endl;
    
}

#endif
