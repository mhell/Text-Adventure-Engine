//
//  game_object_common.h
//  Lab3
//
//  Created by Mattias Hellman on 20/1/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_GAME_OBJECT_COMMON
#define LAB3_GAME_OBJECT_COMMON

#include <iostream>
#include <string>
#include <unordered_map>
#include "json_prototype_pool.h"
#include "game_object.h"
#include "game_data.h"
#include "serializable.h"
#include "event_repository.h"

namespace lab3  {
	
    /**
     * Common implementation class for all game objects. 
     * Interact/deserialize interaction allows for 'lazy deserialization'
     */
    template <typename T>
    class Game_object_common : public JSON_prototype_pool<T>, public Game_object, public Serializable {
    public:
		using String_map = std::unordered_map<std::string, std::string>;
		using String_map_map = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
		using String_map_vec = std::unordered_map<std::string, std::vector<std::string>>;
		
		/**
		 * Gets the name
		 */
        virtual const std::string& get_name() const final override;
        
		/**
		 * Sets the name
		 */
        void set_name(const std::string &name);
        
        /**
         * Instantiates an object from the prototype pool from a name string or a json object
         */
        static std::unique_ptr<T> make(const std::string &name_or_json, std::shared_ptr<Event_repository> event_rep, Game_data *data = nullptr, Game_object *owner = nullptr, const std::string &role = std::string());
        
        /**
         * Point of entry for interaction
         */
        virtual bool interact(const std::string &commands, const Game_object *caller = nullptr) final override;
		
		/**
         * gets the description
         */
        virtual std::string get_description() const final override;
		
		/**
         * Sets the description
         */
		void set_description(const std::string &desc);
        
        /**
         * Sets a owner to this game object
         */
        virtual void set_owner(Game_object *owner) final override;
        
        /**
         * Gets the game data (const version)
         */
		const Game_data* get_game_data() const;
		
		/**
         * Sets the properties
         */
		void set_properties(const std::unordered_map<std::string, int> &properties);
        
        /**
         * Gets the properties
         */
		virtual const std::unordered_map<std::string, int>& get_properties() const override;
		
		/**
		 * Checks if this game object has a property named 'name'
		 */
		virtual int get_property(const std::string &name, bool &success) const final override;
		
		/**
		 * Makes this object 'react' to a given property
		 */
		virtual bool expose_to_property(const std::string &property, int property_level, const Game_object &caller) final override;
        
        /**
         * Requsts to add a member pointer realtionship of 'role' type (game objects contained in *this* game object)
         * When overriding always call parent's implementation first
         */
        virtual bool request_add_member(const std::string &prototype_name, const std::string &role) override;
        
        /**
         * Removes a member from a member vector and releases it back to corresponding object pool
         * When overriding always call parent's implementation first
         */
        virtual bool request_release_member(const std::string &member, const std::string &role) override;
		
		/**
		 * returns a contained member
		 */
		virtual Game_object* get_member(const std::string &member, const std::string &role = "") const;
		
		/**
         * Call to make a "move"
         */
		virtual void action(unsigned tick) override;
        
        /**
         * Releases 'this' back to the pool and requests removal from its parent
         */
		void destroy();
        
    protected:
        /**
         * Clears and resets member variables
         * When overriding always call parent's implementation first
         */
        virtual void destroy_impl();
        
        /**
         * Informs that the instance has been activated
         * When overriding always call parent's implementation first
         */
        virtual void became_active() override;
        
        /**
         * Informs that the instance has been deactivated
         * When overriding always call parent's implementation first
         */
        virtual void became_inactive() override;
        
        /**
         * When overriding always call parent's implementation first
         */
        virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const override;
        
        /**
         * When overriding always call parent's implementation first
         */
        virtual bool deserialize_impl(const std::string &key, const std::string &value) override;
		
		/**
         * When overriding always call parent's implementation first
         */
		virtual bool interact_impl(const std::string &command, const std::string &value, const std::string &remains, const Game_object &caller);
		
		/**
		 * Forwards an interact call to a member
		 */
		bool forward_interact(const std::string &member, const std::string &commands, const Game_object *caller);
		
		/**
         * When overriding always call parent's implementation first
         */
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const;
		
		/**
         * When overriding always call parent's implementation first
         */
		virtual void action_impl(unsigned tick);
        
        /**
         * Renames this game object
         */
        bool rename(const std::string &name, const std::string &unused, const Game_object &caller);
        
        /**
         * Transform this game object into another game object
         */
        bool transform(const std::string &new_object_name, const std::string &role, const Game_object &caller);
		
		/**
		 * Changes the value of a specified property
		 */
		bool change_property(const std::string &property, const std::string &value, const Game_object &caller);
		
		/**
		 * Inform derived of change. When overriding always call parent's implementation first
		 */
		virtual void property_changed();
		
        /**
         * Sets the game data
         */
        void set_game_data(Game_data *data);
		
        /**
         * Gets the game data
         */
		Game_data* get_game_data();
		
		/**
         * Sets the event repository
         */
        void set_event_repository(std::shared_ptr<Event_repository> event_repository);
		
		/**
         * Gets the event repository
         */
		const std::shared_ptr<Event_repository>& get_event_repository() const;
        
        /**
         * Gets the owner
         */
        virtual Game_object* get_owner() const final override;
        
        /**
         * Sets the role *this* object plays in its parent
         */
        void set_role_in_owner(const std::string &role);
        
        /**
         * Gets the role *this* object plays in its parent
         */
        const std::string& get_role_in_owner() const;
        
        /**
         * Destructor. Protected non-virtual - only use pointers to child-classes
         */
        ~Game_object_common() = default;
        
    private:
        std::string m_name;
		
        Game_object *m_owner;
        
		std::string m_role_in_owner;
        
		std::string m_description;
        
        bool inrandom = 0;
        
		Game_data *m_game_data;
		
		std::unordered_map<std::string, int> m_properties;
		
		std::shared_ptr<Event_repository> m_event_repository;
		
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_reactions;
		
		typedef bool (Game_object_common<T>::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands {
            {"examine", &Game_object_common<T>::examine},
            {"rename", &Game_object_common<T>::rename},
            {"transform", &Game_object_common<T>::transform},
			{"change property", &Game_object_common<T>::change_property}
		};
		
		/**
         * Examines this and/or members by telling the delagate to describe it
         */
		bool examine(const std::string &member, const std::string &remains, const Game_object &caller);
		
        /**
         * Users should use make
         */
        using JSON_prototype_pool<T>::acquire;
		
        /**
         * Instantiates and deserializes an object from a json object string
         */
        static std::unique_ptr<T> make_json(const std::string &object, std::shared_ptr<Event_repository> event_rep, Game_data *data = nullptr, Game_object *owner = nullptr, const std::string &role = std::string());
		
    };
    
}

#include "game_object_common.cpp"

#endif
