//
//  event_repository.cpp
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "event_repository.h"

using namespace lab3;


Event* Event_repository::get_event(const std::string &name) {
	try {
		return m_events.at(name).get();
	} catch (std::exception&) {
		return nullptr;
	}
}

void Event_repository::add_event(std::unique_ptr<Event> event) {
	event->set_active(true);
	m_events[event->get_name()] = std::move(event);
}



// aktivera event i add event