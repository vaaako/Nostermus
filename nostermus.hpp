#ifndef NOSTERMUS_H
#define NOSTERMUS_H

#include <cstdint>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

// Types
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;



#ifdef _WIN32
	enum class ANSIColor : uint8 {
		BLACK        = 0,
		DARK_RED     = 4, // Foreground = Esc[30m / Background = Esc[40
		DARK_GREEN   = 2,
		DARK_YELLOW  = 6,
		DARK_BLUE    = 1, 
		DARK_MAGENTA = 5,
		DARK_CYAN    = 3,
		DARK_WHITE   = 7, // Foreground = Esc[37m / Background = Esc[47m

		GRAY    = 8,  // Foreground = Esc[90m / Background = Esc[100m
		RED     = 12,
		GREEN   = 10,
		YELLOW  = 14,
		BLUE    = 9,
		MAGENTA = 13,
		CYAN    = 11,
		WHITE   = 15  // Foreground = Esc[97m / Background = Esc[107m
	};

	// Bold         = Esc[1m
	// Underline    = Esc[4m
	// No underline = Esc[24m
	// Reverse text = Esc[7m
	// Not reversed = Esc[27m

	enum class ANSIBackground : uint8 {
		BLACK        = 40,
		DARK_RED     = 41,
		DARK_GREEN   = 42,
		DARK_YELLOW  = 43,
		DARK_BLUE    = 44, 
		DARK_MAGENTA = 45,
		DARK_CYAN    = 46,
		DARK_WHITE   = 47,

		GRAY    = 100,
		RED     = 101,
		GREEN   = 102,
		YELLOW  = 103,
		BLUE    = 104,
		MAGENTA = 105,
		CYAN    = 106,
		WHITE   = 107  // Foreground = Esc[97m / Background = Esc[107m
	};
#else
	enum class ANSIColor {
		BLACK  = 30,
		DARK_RED     = 31,
		DARK_GREEN   = 32,
		DARK_YELLOW  = 33,
		DARK_BLUE    = 34,
		DARK_MAGENTA = 35,
		DARK_CYAN    = 36,
		DARK_WHITE   = 37,

		GRAY    = 90,
		RED     = 91,
		GREEN   = 92,
		YELLOW  = 93,
		BLUE    = 94,
		MAGENTA = 95,
		CYAN    = 96,
		WHITE   = 97,
	};

	enum class ANSIBackground : uint8 {
		BLACK  = 40,
		RED    = 41,
		GREEN  = 42,
		YELLOW = 43,
		BLUE   = 44,
		PURPLE = 45,
		CYAN   = 46,
		WHITE  = 47
	};
#endif

// enum Key : uint8 {
// 	VK_A = 'a',
// 	VK_a = 'a',
// 	VK_B = 'B'
// };


// struct Termstruct {
// 	uint8* buffer; // Chars location
// 	uint16 width;
// 	uint16 height;
// };



namespace nostterm {
	// Changes the terminal title
	inline void set_terminal_title(const char* title) {
	#ifdef _WIN32
		SetConsoleTitleA(title);
	#else
		std::cout << "\033]0;" << title << "\007" << std::flush;
	#endif
	}

	// Get terminal width
	uint16 term_col();

	// Get terminal height
	uint16 term_row();

	// Check if coords is whithin terminal bounds
	inline bool inbounds(const uint16 x, const uint16 y) {
		return (x < nostterm::term_col() && y < nostterm::term_row());
	}

	// Disable echoing the key pressed
	void disable_echoing(const bool enable = true);

	// Hide cursor for being draw
	void hide_cursor();

	// Show cursor
	void show_cursor();

	// Move cursor to some location on terminal.
	// This is used before drawing something on a location
	void move_cursor(const uint16 x, const uint16 y);

	// Wait for key without echoing
	void waitkey();

	// Wait for key without echoing and return it
	uint8 getkey();
}


namespace nostcolor {
	// Set color for next draw
	void set_color(const ANSIColor color);

	// Set color for next draw
	void set_bold(const ANSIColor color);

	// Set color for next draw
	void set_background(const ANSIBackground color);

	// Reset the color for next draw
	void reset_color();
}

namespace nostdraw {
	// Clear the screen entirely
	void clear_screen();

	// Clear the line under the cursor
	void clear_line(const uint16 x, const uint16 y);

	// Print a character without pushing it to the buffer
	void printchar(const uint16 x, const uint16 y, const char ch);

	// Put a single char in some location
	void putchar(const uint16 x, const uint16 y, const char ch);

	// Put a list of chars in a row.
	// Place all the characters in the buffer
	void putchars(const uint16 x, const uint16 y, const char* chars);

	// Put a string in some location.
	// This does not place the string in the buffer
	void putstring(const uint16 x, const uint16 y, const char* message);

	// Show a message in some location and wait for any key
	void show_message(const uint16 x, const uint16 y, const char* message);

	// Show a message at top that only hides if press space
	void show_warning(const char* message);
}


#endif
