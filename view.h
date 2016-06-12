//
//  view.h
//  Lab3
//
//  Created by Mattias Hellman on 6/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#ifndef LAB3_VIEW
#define LAB3_VIEW

#include <iostream>
#include <ncurses.h>

namespace lab3  {
	
	class View {
	public:
		View(int width, int height, bool center = true, int offset_x = 0, int offset_y = 0, bool scroll = true);
		
		/**
		 * Outputs three columns of texts with the space "padding" inbetween. Strings can contain linebreaks.
		 * The same linenumber of every colums is rendered on the same row.
		 */
		void render_table(const std::vector<std::string> &cols, int attribute = A_NORMAL, int width = 0, int padding = 11, bool new_line = true) const;
		
		/**
		 * Outputs a line of text
		 */
		void render_text(const std::string &text, int attribute = A_NORMAL, bool tags = false, int width = 0, int offset_x = -1, int offset_y = -1) const;
		
		/**
		 * Loads a file containing ascii art (text) and outputs it one line at a time
		 */
		void render_text_from_file(const std::string& file_path, bool auto_format = false, bool tags = false) const;
		
		/**
		 * Promts the user for input
		 */
		std::string prompt_user(const std::string &message = "\nCommand: ") const;
		
		/**
		 * clears the screen of all its content
		 */
		void clear_screen() const;
		
		~View();
		
	private:
		WINDOW* m_win;
		
	};
	
}

#endif
