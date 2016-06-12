//
//  view.cpp
//  Lab3
//
//  Created by Mattias Hellman on 6/6/2014.
//  Copyright (c) 2014 Mattias Hellman. All rights reserved.
//

#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "view.h"
#include "utils.h"

using namespace lab3;


View::View(int width, int height, bool center, int offset_x, int offset_y, bool scroll) {
	clear();
	refresh();
	
	if (center) {
		offset_x = (width != 0 ? ((COLS - width) / 2) : 0)  + offset_x;
		offset_y = (height != 0 ? ((LINES - height) / 2) : 0) + offset_y;
	}
	
    m_win = newwin(height, width, offset_y, offset_x);
	
	scrollok(m_win, scroll);
	
    refresh();
}

void View::render_table(const std::vector<std::string> &cols, int attribute, int width, int padding, bool new_line) const {
	std::string line;
	bool first_column = true;
	int cur_col_width, cur_col_lines;
	int prev_col_width = 0;
	int prev_col_lines = 1;
	int start_x = getcurx(m_win);
	int offset_x = start_x;
	int max_lines = 0;
	
	// for each column...
	for (auto &col : cols) {
		offset_x += first_column ? 0 : prev_col_width + padding;
		first_column = false;
		cur_col_width = width;
		cur_col_lines = 0;
		std::stringstream ss(col);
		
		// for each line in the column		(returerna dessa i render_text? räkna in alla radbryt också)
		while (!ss.eof()) {
			// get the line
			std::getline(ss, line);
			
			// increment lines
			++cur_col_lines;
			
			// calculate line length by removing tags
			std::string stripped_line = std::regex_replace(line, std::regex("[^\\\\]<.+?>|\\\\(?=<)|\\\\(?=>)"), "");
			int line_length = (int)stripped_line.length();
			
			// if width not set save the line's length if longer than previously longest of this column
			if (!width && line_length > cur_col_width)
				cur_col_width = line_length;
		}
		
		// make row-break if the columns gets wider than the window
		if (offset_x+cur_col_width > getmaxx(m_win)) {
			// move far enough in y to get past the largest column
			wmove(m_win, getcury(m_win) + (max_lines - prev_col_lines), getcurx(m_win));
			waddstr(m_win, "\n\n");
			
			offset_x = start_x;
			prev_col_lines = 1;
			max_lines = 0;
		}
		
		// set max_lines if this column was larger than previously largest
		if (cur_col_lines > max_lines) max_lines = cur_col_lines;
		
		// print the text
		render_text(col, attribute, true, width, offset_x, getcury(m_win) - (prev_col_lines-1));
		
		// update previous col width value
		prev_col_width = cur_col_width;
		
		// update previous col lines value
		prev_col_lines = cur_col_lines;
	}
	
	// add optional line break
	if (new_line) {
		// move in line of the longest column, then add linebreak
		wmove(m_win, getcury(m_win) + (max_lines - cur_col_lines), getcurx(m_win));
		waddch(m_win, '\n');
	} else {
		wmove(m_win, getcury(m_win), offset_x + cur_col_width);
	}
}

void View::render_text(const std::string &text, int attribute, bool tags, int width, int offset_x, int offset_y) const {
	if (!width) width = getmaxx(m_win); // default value
	std::string line, word;
	bool capitalize = true;
	int line_length = getcurx(m_win);
	std::stringstream line_ss(text);
	
	// move cursor
	if (offset_x != -1) wmove(m_win, getcury(m_win), offset_x);
	if (offset_y != -1) wmove(m_win, offset_y, getcurx(m_win));
	
	// check if should do linebreak on last/only line
	bool endbreak = std::regex_match(text, std::regex("[\\s\\S]*\n"));
	
	// enable attribute
	wattron(m_win, attribute);
	
	// for each line
	while (!line_ss.eof()) {
		std::getline(line_ss, line);
		std::stringstream word_ss(line);
		
		// for each word
		while (!word_ss.eof()) {
			std::getline(word_ss, word, ' ');
			bool enable_bold = false, disable_bold = false;
			bool enable_underline = false, disable_underline = false;
			
			// capitalize first letter in a sentence
			if (capitalize) Utils::capitalize_word(word);
			
			// check if next word should be capitalized
			if (word.back() == '.') {
				capitalize = true;
			} else {
				capitalize = false;
			}
			
			if (tags) {
				// if begins with <b>; remove <b> and set enable_bold
				std::regex exp("^(<b>)");
				auto old_word_size = word.size();
				word = std::regex_replace(word, exp, "");
				if (word.size() != old_word_size) enable_bold = true;
				
				// if ends with </b>; remove </b> and set disable_bold
				exp = "(?:</b>(.)?)$";
				old_word_size = word.size();
				word = std::regex_replace(word, exp, "$1");
				if (word.size() != old_word_size) disable_bold = true;
				
				// if begins with <u>; remove <u> and set enable_undeline
				exp = "^(<u>)";
				old_word_size = word.size();
				word = std::regex_replace(word, exp, "");
				if (word.size() != old_word_size) enable_underline = true;
				
				// if ends with </u>; remove </u> and set disable_underline
				exp = "(?:</u>(.)?)$";
				old_word_size = word.size();
				word = std::regex_replace(word, exp, "$1");
				if (word.size() != old_word_size) disable_underline = true;
				
				// remove tags
				exp = "\\\\(?=<)|\\\\(?=>)";
				word = std::regex_replace(word, exp, "");
			} else {
				// remove tagss
				std::regex exp("<b>|</b>|<u>|</u>|\\\\(?=<)|\\\\(?=>)");
				word = std::regex_replace(word, exp, "");
			}
            
            // calculate line length. Add 1 for space when not last word
            line_length += (word_ss.peek() != EOF ? word.size() + 1 : word.size());
            
            // add linebreak if line length >= width
			if (line_length >= width) {
                word = "\n" + word;
                // recalculate line length
                line_length = (int)(word_ss.peek() != EOF ? word.size() + 1 : word.size());
			}
            
			// enable attributes
			if (enable_bold) wattron(m_win, A_BOLD);
			if (enable_underline) wattron(m_win, A_UNDERLINE);
			
			// move cursor to input offset_x when linebreak resets x to 0
			if (getcurx(m_win) == 0) wmove(m_win, getcury(m_win), offset_x);
			
			// print the word
			waddstr(m_win, word.c_str());
			
			// disable attributes
			if (disable_bold) wattroff(m_win, A_BOLD);
			if (disable_underline) wattroff(m_win, A_UNDERLINE);
			
			// re-add spaces
			if (word_ss.peek() != EOF) waddch(m_win, ' ');
		}
		
		// re-add linebreak & reset line length
        if (line_ss.peek() != EOF || endbreak) {
            waddch(m_win, '\n');
            line_length = 0;
        }
	}
	
	wrefresh(m_win);
	
	wattroff(m_win, attribute);
}

void View::render_text_from_file(const std::string& file_path, bool auto_format, bool tags) const {
	std::ifstream ifs(file_path);
    
    if (ifs.good()) {
        std::string line;
        std::getline(ifs, line, '\0');
        
        if (auto_format) {
            render_text(line, A_NORMAL, tags);
        } else {
            waddstr(m_win, line.c_str());
        }
        
        ifs.close();
    } else {
        throw std::runtime_error("Error loading text file " + file_path);
    }
	
	waddch(m_win, '\n');
	
	wrefresh(m_win);
}

std::string View::prompt_user(const std::string &message) const {
	// output promt
	wattron(m_win, A_BOLD);
	waddstr(m_win, (message).c_str());
	wrefresh(m_win);
	wattroff(m_win, A_BOLD);
	
	// get input
	char input[180];
	wgetstr(m_win, input);
	
	return std::string(input);
}

void View::clear_screen() const {
	wclear(m_win);
	wrefresh(m_win);
}

View::~View() {
	delwin(m_win);
}
