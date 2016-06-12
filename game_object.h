//
//  game_object.h
//  Lab3
//
//  Created by Mattias Hellman on 7/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_IGAME_OBJECT
#define LAB3_IGAME_OBJECT

#include <iostream>
#include <string>
#include <memory>
#include "game_delegate.h"

namespace lab3  {
    
    /**
     * Interface class representing a game object
     */
    class Game_object {
    public:
        virtual const std::string& get_root() const = 0;
        virtual const std::string& get_type() const = 0;
        virtual const std::string& get_name() const = 0;
        virtual std::string get_description() const = 0;
        
        virtual bool interact(const std::string &commands, const Game_object *caller) = 0;
        
        virtual bool request_add_member(const std::string &prototype_name, const std::string &role) = 0;
        virtual bool request_release_member(const std::string &member, const std::string &role) = 0;
		
		virtual Game_object* get_member(const std::string &member, const std::string &role) const = 0;
		
        virtual void set_owner(Game_object* owner) = 0;
        virtual Game_object* get_owner() const = 0;
		
		virtual const std::unordered_map<std::string, int>& get_properties() const = 0;
		virtual int get_property(const std::string &name, bool &success) const = 0;
		
		virtual bool expose_to_property(const std::string &property, int property_level, const Game_object &caller) = 0;
		
		virtual void action(unsigned tick) = 0;
        
		void set_delegate(Game_delegate* delegate);
		Game_delegate* get_delegate() const;
		
	private:
        static Game_delegate *m_delegate;
		
		Game_object& operator=(const Game_object&); // prevent slicing
        
    };
    
}


#endif
