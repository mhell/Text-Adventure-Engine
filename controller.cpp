//
//  controller.cpp
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "controller.h"

using namespace lab3;


Controller::Controller(Application &app, View &view) : m_app(app), m_view(view) {
}

Application& Controller::get_application() {
	return m_app;
}

const View& Controller::get_view() const {
	return m_view;
}

View& Controller::get_view() {
	return m_view;
}

void Controller::became_primary() {
	if (!m_been_primary) {
		became_primary_impl();
		m_been_primary = true;
	} else {
		re_became_primary();
	}
}

void Controller::left_primary() {
	//get_view()->clear_screen();
	left_primary_impl();
}