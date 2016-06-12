//
//  environment.h
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_ENVIRONMENT
#define LAB3_ENVIRONMENT

#include <iostream>
#include "game_object_common.h"
#include "actor.h"
#include "item.h"

namespace lab3  {

    /**
     * Abstract base class of all Environments.
     */
    class Environment : public Game_object_common<Environment> {
    public:
        virtual ~Environment() = default;
        
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
		
		const std::string& get_artwork() const;
        
        void set_artwork(const std::string &artwork_path);
		
		const std::string& get_coordinates() const;
        
        void set_coordinates(const std::string &coordinates);
		
		std::string get_scene() const;
        
        void set_scene(const std::string &scene);
		
		const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> get_exits() const;
		
		void set_exits(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &exits);
        
        bool activate_exit(const std::string &cardinal);
        
        bool hide_exit(const std::string &cardinal);
   
        const std::vector<std::unique_ptr<Actor>>& get_actors() const;
        
        void add_actor(std::unique_ptr<Actor> actor);
		
		std::unique_ptr<Actor> remove_actor(const std::string &name);
        
        const std::vector<std::unique_ptr<Item>>& get_items() const;
        
        void add_item(std::unique_ptr<Item> item);
		
        std::unique_ptr<Item> remove_item(const std::string &name);
        
        Item* get_item(const std::string &name) const;
		
	protected:
		virtual void get_scene_impl(std::unordered_map<std::string, std::string> &keys) const;
		
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const override;
		
		virtual void action_impl(unsigned tick) override;
		
		virtual void property_changed();
    
    private:
        static const std::string ROOT;
		
		std::string m_artwork;
		
		std::string m_coordinates;
		
		std::string m_scene;
		
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_exits;
        
        std::vector<std::unique_ptr<Actor>> m_actors;
        
        std::vector<std::unique_ptr<Item>> m_items;
		
		typedef bool (Environment::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands{ {"exit", &Environment::exit} };
		
		bool exit(const std::string &cardinal, const std::string &unused, const Game_object &caller);
    };
    
}

#endif
