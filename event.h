//
//  event.h
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_EVENT
#define LAB3_EVENT

#include <iostream>
#include "serializable.h"
#include "json_prototype_pool.h"

namespace lab3  {
	
    /**
     * Class describing an event
     */
    class Event : public JSON_prototype_pool<Event>, public Serializable {
    public:
		/**
         * Instantiates an object from the prototype pool from a name string or a json object
         */
		static std::unique_ptr<Event> make(const std::string &name_or_json);
		
		virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const final override;
        
        virtual bool deserialize_impl(const std::string &key, const std::string &value) final override;
		      
        virtual void became_active() final override;
        
        virtual void became_inactive() final override;
		
		const std::string& get_type() const;
		
		const std::string& get_name() const;
		
        std::string get_description() const;
		
		const std::string& get_multiplier() const;
		
		const std::string& get_commands() const;
	
	private:
        static const std::string TYPE;
		
		std::string m_name;
		
		std::string m_description;
		
		std::string m_commands;
        
        static bool registered;
		
		static std::unique_ptr<Event> factory();
		
		/**
         * Users should use make
         */
        using JSON_prototype_pool<Event>::acquire;
	};
	
	
	#pragma mark - inline declarations
	
	inline const std::string& Event::get_type() const {
		return TYPE;
	}
	
	inline const std::string& Event::get_name() const {
		return m_name;
	}
	
	inline std::string Event::get_description() const {
		return m_description;
	}
	
	inline const std::string& Event::get_commands() const {
		return m_commands;
	}
}

#endif
