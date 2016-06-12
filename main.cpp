//
//  main.cpp
//  Lab3
//
//  Created by Mattias Hellman on 17/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#include <iostream>
#include "test_json_prototype_pool.h"
#include "test_game_builder.h"
#include "test_game_object.h"
#include "application.h"
#include "main_menu_controller.h"
#include "view.h"
#include "globals.h"

int main(int argc, const char * argv[])
{
    // assertion tests
    #if 0
        test_json_prototype_pool();
        test_builder();
		test_game_object();
        return 0;
    #endif

    Application app;
	View view(WORLD_WIDTH, WORLD_HEIGHT, true, 0, 1);
	std::unique_ptr<Main_menu_controller> main_menu(new Main_menu_controller(app, view));

	app.push_controller(std::move(main_menu));
	app.run();
	
    return 0;
}

