//
//  Game_builder.h
//  Lab3
//
//  Created by Mattias Hellman on 18/02/14.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3__GAME_BUILDER
#define LAB3__GAME_BUILDER

#include <iostream>
#include <memory>
#include "world.h"

namespace lab3 {
    
    class Game_builder {
    public:
        static std::unique_ptr<World> load(const std::string &file_path);
        
        static void save(const World &world, const std::string &file_path);
        
    private:
        static void add_prototypes(const std::string &data);
        
    };
    
}

#endif
