//
//  environmental.h
//  Lab3
//
//  Created by Mattias Hellman on 4/3/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_ENVIRONMENTAL
#define LAB3_ENVIRONMENTAL

#include <iostream>
#include "item.h"

namespace lab3  {
    
    /**
     * Class describing a default environmental item
     */
    class Environmental : public Item {
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
        static const std::string TYPE;
        
        static std::unique_ptr<Item> factory();
        
        static bool registered;
        
        virtual bool add_item(std::unique_ptr<Item> item) final override;
        
        virtual std::unique_ptr<Item> remove_item(const std::string &name) final override;
        
        virtual Item* get_item(const std::string &name) const final override;
        
        typedef bool (Environmental::*Command_ptr)(const std::string&, const std::string&, const Game_object&);
        
        std::unordered_map<std::string, Command_ptr> m_commands{ {"activate exit", &Environmental::activate_exit} };
        
        /**
         * Add an exit to the environment
         */
        bool activate_exit(const std::string &cardinal, const std::string &unused, const Game_object &caller);
    };
}

#endif
