//
//  game_object.cpp
//  Lab3
//
//  Created by Mattias Hellman on 20/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include "game_object.h"

using namespace lab3;


#pragma mark - Static initializers

Game_delegate *lab3::Game_object::m_delegate = nullptr;


#pragma mark - Setters/getters

void lab3::Game_object::set_delegate(Game_delegate *delegate) {
	m_delegate = delegate;
}

lab3::Game_delegate* Game_object::get_delegate() const {
	return m_delegate;
}