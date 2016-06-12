//
//  serializable.h
//  Lab3
//
//  Created by Mattias Hellman on 13/8/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_SERIALIZABLE
#define LAB3_SERIALIZABLE

#include <iostream>
#include <unordered_map>
#include <vector>

namespace lab3  {
    
    /**
     * Non-virtual interface for serializing/deserialization. Allows for "lazy deserialization"
     */
    class Serializable {
    public:
		/**
         * Checks to see if this game object is active and able to recieve interact commands
         */
		bool is_active() const;
        
        /**
         * Sets the active state
         */
		void set_active(bool state);
		
		/**
		 * Activate (deserialize) selected keys
		 */
		void activate_selective(const std::vector<std::string> &keys);
	
		/**
         * Serializes an instance into json format
         */
		std::string serialize() const;
        
        /**
         * Deserializes an instance with json
         */
		void deserialize(const std::string &json);
		
		/**
         * Gets wether this object is serialized
         */
        const bool is_deserialized() const;
		
		/**
		 * Resets state
		 */
		void reset();
		
	protected:
		/**
         * Informs that the instance has been activated
         * When overriding always call parent's implementation first
         */
        virtual void became_active() = 0;
        
        /**
         * Informs that the instance has been deactivated
         * When overriding always call parent's implementation first
         */
        virtual void became_inactive() = 0;
        
        /**
         * When overriding always call parent's implementation first
         */
        virtual void serialize_impl(std::unordered_map<std::string, std::string> &data) const = 0;
        
        /**
         * When overriding always call parent's implementation first
         */
        virtual bool deserialize_impl(const std::string &key, const std::string &value) = 0;
	
	private:
        bool m_active = false;
        bool m_deserialized = false;
        std::string m_json_stored;
		
		Serializable& operator=(const Serializable&); // prevent slicing
		
	};

}

#endif
