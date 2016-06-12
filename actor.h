//
//  actor.h
//  Lab3
//
//  Created by Mattias Hellman on 16/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_ACTOR
#define LAB3_ACTOR

#include <iostream>
#include "game_object_common.h"
#include "item.h"

namespace lab3  {
    
    //class Item;

    /**
     * Abstract base class of all actors.
     */
    class Actor : public Game_object_common<Actor> {
    public:
        virtual ~Actor() = default;
        
        virtual const std::string& get_root() const final override;
        
        virtual void destroy_impl() override;
        
        virtual bool request_add_member(const std::string &prototype_name, const std::string &role) override;
		
		virtual bool request_release_member(const std::string &member, const std::string &role) override;
		
		virtual Game_object* get_member(const std::string &member, const std::string &role) const override;
        
        virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const override;
        
        virtual bool deserialize_impl(const std::string &key, const std::string &value) override;
		
		virtual bool interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) override;
        
        virtual void became_active() override;
        
        virtual void became_inactive() override;
        
        bool is_playable() const;
        
        void set_playable(bool state);
        
        const std::vector<std::unique_ptr<Item>>& get_items() const;
        
        void add_item(std::unique_ptr<Item> item);
		
		std::unique_ptr<Item> remove_item(const std::string &name);
		
		int get_health() const;
		
	protected:
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const override;
		
		virtual void action_impl(unsigned tick) override;
		
		virtual void property_changed();
		
		bool check_health();
        
    private:
        static const std::string ROOT;
        
        bool m_playable = false;
		
        std::vector<std::string> m_health;
        
        std::vector<std::unique_ptr<Item>> m_items;
		
		std::vector<std::string> m_actions;
		
		std::unordered_map<std::string, std::string> m_user_commands;
		
		typedef bool (Actor::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands{
            {"drop", &Actor::drop},
            {"pick up", &Actor::pick_up}};
    
        /**
         * Drops an object to the current environment. item = "all" drops all items
         */
        bool drop(const std::string &item, const std::string &unused, const Game_object &caller);
        
        /**
         * Picks up an item from the current environment. Works iteratively on nested containers
         */
        bool pick_up(const std::string &first_item, const std::string &rest_items, const Game_object &caller);
		
		/**
         * searches the environment for items with property and consumes it if found
         */
		bool search_consume(const std::string &property, const std::string &unused, const Game_object &caller);
		
		/**
         * searches the environment for items with property and picks it up if found
         */
		bool search_pick_up(const std::string &property, const std::string &unused, const Game_object &caller);
		
		/**
         * searches for an actor with a property and attacks it using item
         */
		bool hunt_consume(const std::string &property, const std::string &item, const Game_object &caller);
		
		/**
         * self-defends itself by attacking it's latest attacker with item
         */
		bool defence(const std::string &item, const std::string &unused, const Game_object &caller);
       
    };
    
}

#endif
