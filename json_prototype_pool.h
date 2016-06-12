//
//  json_prototype_pool.h
//  Lab3
//
//  Created by Mattias Hellman on 16/12/2013.
//  Copyright (c) 2013 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_PROTOTYPE_POOL
#define LAB3_PROTOTYPE_POOL

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <tuple>

namespace lab3  {
    
    const std::string RANDOM_PROTOTYPE = "RANDOM", THROW_EXCEPT = "FAIL", FAILSAFE = THROW_EXCEPT;
    
    /**
     * Abstract base class template for managing interaction between an object pool of
     * type T and prototypes described as JSON-strings.
     * Prototypes makes it possible to "inherit" data.
     * An object pool stores objects instead of deleting them so they can be reused.
     * Template type requirements: name & prototype field in json description, deserialize(string) & get_type() member
     */
    template <typename T>
    class JSON_prototype_pool {
    public:
        /**
         * Pointer type to factory function
         */
        typedef std::unique_ptr<T> (*factory_ptr)();
        
        /**
         * Adds a json object definition to the prototype pool
         */
        static void add_prototype(const std::string &json);
        
        /**
         * Acquire an object from the pool matching the given prototype's name, or deserialize a new if not found
         */
        static std::unique_ptr<T> acquire(const std::string &prototype_name);
        
        /**
         * Releases the specified object back to the pool
         */
        static void release(std::unique_ptr<T> object);
        
        /**
         * Get the factory method of the given prototype
         */
        static factory_ptr factory_of(const std::string &prototype_name);
        
        /**
         * Get the json definirion of the given prototype
         */
        static std::string& json_of(const std::string &prototype_name);
        
        /**
         * Get the class of the given prototype
         */
        static std::string& class_of(const std::string &prototype_name);
        
		/**
         * Get the the number of prototypes stored
         */
        static std::size_t num_prototypes();
        
		/**
         * Get the the number objects in the object pool
         */
        static std::size_t pool_size(const std::string &type);
        
    protected:
        /**
         * Used by concrete child classes to add a (default of class) prototype. A default prototype
         * equals a default-constructor initialized instance and is the most abstract version of a 'type'
         */
        static bool register_default(const std::string &type, factory_ptr factory, const std::string &json = "");
        
        /**
         * Destructor. Protected non-virtual - only use pointers to child-classes
         */
        ~JSON_prototype_pool() = default;
        
    private:
		enum Prototypes_tuple_index {TYPE, JSON, FACTORY};
        
        /**
         * Flag for initializing the random seed only once
         */
        static bool m_rand_init;
		
        /**
         * Static member function that returns a map of prototypes.
         * Used instead of static member object to prevent the "static initialization order fiasco"
         */
        static std::unordered_map<std::string, std::tuple<std::string, std::string, factory_ptr>>& json_prototypes();
        
        /**
         * The object pool. An unordered_map of lists where each list is an object pool of that a specific class type
         */
        static std::unordered_map<std::string, std::list<std::unique_ptr<T>>> m_object_pool;
        
        /**
         * Pointer type to prototype getter function
         */
        typedef std::tuple<std::string, std::string, factory_ptr>& (*Proto_getter_ptr)(const std::string&);
        
        /**
         * Prototype getter function (random creation)
         */
        static std::tuple<std::string, std::string, factory_ptr>& random_prototype(const std::string &filter);
        
        /**
         * Prototype getter function (throws an invalid_argument exception)
         */
        static std::tuple<std::string, std::string, factory_ptr>& throw_exception(const std::string &prototype_name);
        
        /*
         * Static member function that returns a map of functions associated to certain prototype creation policies (random, default)
         * Used instead of static member object to prevent the "static initialization order fiasco"
         */
        static std::unordered_map<std::string, Proto_getter_ptr>& proto_makers();
        
    };
}

#include "json_prototype_pool.cpp"

#endif
