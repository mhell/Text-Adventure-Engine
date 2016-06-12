//
//  controller.h
//  Lab3
//
//  Created by Mattias Hellman on 5/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_CONTROLLER
#define LAB3_CONTROLLER

#include <iostream>
#include "application.h"
#include "view.h"

namespace lab3  {
	
	class Controller {
	public:
		Controller(Application& app, View &view);
		
		const View& get_view() const;
		
		bool virtual update(const std::string &commands) = 0;
		
		void became_primary();
		
		void left_primary();
		
	protected:
		Application& get_application();
		
		View& get_view();
		
		virtual void became_primary_impl() = 0;
		
		virtual void left_primary_impl() = 0;
		
		virtual void re_became_primary() = 0;
		
	private:
		Application &m_app;
		View &m_view;
		bool m_been_primary = false;
		
	};
	
}


#endif