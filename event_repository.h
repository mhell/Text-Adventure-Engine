//
//  event_repository.h
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_EVENT_REPOSITORY
#define LAB3_EVENT_REPOSITORY

#include <iostream>
#include <unordered_map>
#include <memory>
#include "event.h"

namespace lab3  {
	
    /**
     * Class that holds events
     */
    class Event_repository {
    public:
		/**
		 * Returns specified event or nullptr if not found
		 */
		Event* get_event(const std::string &name);
		
		/**
		 * Inserts by overwriting any existing event with same name
		 */
		void add_event(std::unique_ptr<Event> event);
		
	private:
		std::unordered_map<std::string, std::unique_ptr<Event>> m_events;

	};

}

#endif

