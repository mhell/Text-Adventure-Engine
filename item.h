//
//  item.h
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_ITEM
#define LAB3_ITEM

#include <iostream>
#include "game_object_common.h"

namespace lab3  {
    
    /**
     * Abstract base class of all items.
     */
    class Item : public Game_object_common<Item> {
    public:
        virtual ~Item() = default;
        
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
        
        virtual bool add_item(std::unique_ptr<Item> item) = 0;
        
        virtual std::unique_ptr<Item> remove_item(const std::string &name) = 0;
        
        virtual Item* get_item(const std::string &name) const = 0;
        
        void set_degradables(const std::vector<std::string> &degradables);
        
        const std::vector<std::string>& get_degradables() const;
		
	protected:
		virtual void examine_impl(String_map &singles, String_map_map &mapables, String_map_vec &lists) const override;
		
		virtual void action_impl(unsigned tick) override;
		
		virtual void property_changed();
        
    private:
        static const std::string ROOT;
        
        std::vector<std::string> m_degradables;
		
		typedef bool (Item::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
		
		std::unordered_map<std::string, Command_ptr> m_commands{
            {"on", &Item::on},
            {"with", &Item::with}};
		
		/**
         * Apply object on a target actor or item. Searches first actors, then if not found, items in the environment
         */
		bool on(const std::string &target, const std::string &unused, const Game_object &caller);
        
        /**
         * Apply object on a target item in callers item list.
         */
        bool with(const std::string &target, const std::string &unused, const Game_object &caller);
        
    };
    
}

#endif
