//
//  world.h
//  Lab3
//
//  Created by Mattias Hellman on 21/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_WORLD
#define LAB3_WORLD

#include <iostream>
#include "game_object_common.h"
#include "environment.h"

namespace lab3  {
    
    //class Environment;

    /**
     * The world "map". Contains environments
     */
    class World : public Game_object_common<World>, public Game_data {
    public:
        virtual ~World() = default;
        
        virtual const std::string& get_root() const final override;
        
        virtual const std::string& get_type() const final override;
        
        virtual void destroy_impl() override;
        
        virtual bool request_add_member(const std::string &prototype_name, const std::string &role) final override;
		
		virtual bool request_release_member(const std::string &member, const std::string &role) final override;
		
		virtual Game_object* get_member(const std::string &member, const std::string &role) const override;
        
        virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const final override;
        
        virtual bool deserialize_impl(const std::string &key, const std::string &value) final override;
		
		virtual bool interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller) final override;

        virtual void became_active() final override;
        
        virtual void became_inactive() final override;
        
        virtual Environment* get_current_environment() const final override;
		
        const std::string& get_current_environment_name() const;
        
        virtual void set_current_environment(const std::string &name) final override;
		
        virtual const std::string& get_prev_environment_name() const final override;
		
		virtual Environment* get_environment(const std::string &name) const final override;
		
		virtual const std::vector<Actor*>& get_playables() const final override;
		
		virtual const std::vector<Actor*>& get_enemies() const final override;
		
		virtual Actor* get_playable(const std::string &name) final override;
		
		virtual Actor* get_enemy(const std::string &name) final override;
        
        unsigned get_time() const;
        
        void set_time(unsigned time);
		
		virtual const std::string& get_time_string() const final override;
		
		virtual const std::string& get_long_time_string() const final override;
        
        const std::vector<std::unique_ptr<Environment>>& get_environments() const;
        
        void add_environment(std::unique_ptr<Environment> environment);
		
		virtual void update() final override;
		
	protected:
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const final override;

		virtual void action_impl(unsigned tick) final override;
		
		virtual void property_changed();
        
    private:
		typedef bool (World::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands;
		
        static const std::string ROOT;
        
        static const std::string TYPE;

        static std::unique_ptr<World> factory();
        
        static bool registered;
		
		static std::vector<std::string> m_time_strings;
		
		static std::vector<std::string> m_long_time_strings;
        
        unsigned m_time = 0;
        
        std::string m_current_environment;
		
        std::string m_prev_environment;
        
        std::vector<std::unique_ptr<Environment>> m_environments;
		
		mutable std::vector<Actor*> m_playables;
		
        mutable std::vector<Actor*> m_enemies;
        
        std::vector<std::string> m_defaults;
		
		mutable bool m_playables_updated = false;
		
		void update_actors() const;
    };
    
}

#endif

