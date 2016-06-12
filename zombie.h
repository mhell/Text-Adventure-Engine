//
//  zombie.h
//  Lab3
//
//  Created by Mattias Hellman on 18/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_ZOMBIE
#define LAB3_ZOMBIE

#include <iostream>
#include "actor.h"

namespace lab3  {
    
    /**
     * Class describing a default zombie actor
     */
    class Zombie : public Actor {
    public:
        virtual const std::string& get_type() const final override;
        
        virtual void destroy_impl() final override;
        
        virtual bool request_add_member(const std::string &prototype_name, const std::string &role) final override;
		
		virtual bool request_release_member(const std::string &member, const std::string &role) final override;
		
		virtual Game_object* get_member(const std::string &member, const std::string &role) const final override;
        
        virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const final override;
        
        virtual bool deserialize_impl(const std::string &key, const std::string &value) final override;
		
		virtual bool interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) final override;
        
        virtual void became_active() final override;
        
        virtual void became_inactive() final override;
		
	protected:
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const final override;
		
		virtual void action_impl(unsigned tick) final override;
		
		virtual void property_changed();
        
    private:
		typedef bool (Zombie::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands;
		
        static const std::string TYPE;
        
        static std::unique_ptr<Actor> factory();
        
        static bool registered;
        
    };
}

#endif
