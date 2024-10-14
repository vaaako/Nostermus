#include "nostermus.hpp"
#include <cstring>

#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

uint16 nostterm::term_col() {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return static_cast<uint16>((csbi.srWindow.Right - csbi.srWindow.Left) + 1);
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
#endif
}

uint16 nostterm::term_row() {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return static_cast<uint16>((csbi.srWindow.Bottom - csbi.srWindow.Top) + 1);
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
#endif
}

void nostterm::disable_echoing(const bool enable) {
#ifdef _WIN32
	return;
#else
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO); // Disable line buffering and echo

	if(enable) {
		term.c_lflag &= ~(ICANON | ECHO);
	} else {
		term.c_lflag |= (ICANON | ECHO);
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}


// Hides the cursor
void nostterm::hide_cursor() {
#ifdef _WIN32
	static HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorinfo;

	// Get the current cursor info first
	if(!GetConsoleCursorInfo(stdhandle, &cursorinfo)) {
		return;
	}

	cursorinfo.bVisible = FALSE;
	SetConsoleCursorInfo(stdhandle, &cursorinfo);
#else
	std::cout << "\033[?25l" << std::flush;
#endif
}

// Shows the cursor
void nostterm::show_cursor() {
#ifdef _WIN32
	static HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorinfo;

	// Get the current cursor info first
	if(!GetConsoleCursorInfo(stdhandle, &cursorinfo)) {
		return;
	}

	cursorinfo.bVisible = TRUE;
	SetConsoleCursorInfo(stdhandle, &cursorinfo);
#else
	std::cout << "\033[?25h" << std::flush;
#endif
}

void nostterm::move_cursor(const uint16 x, const uint16 y) {
	if(!nostterm::inbounds(x, y)) {
		return;
	}

#ifdef _WIN32
	static const HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();

	SetConsoleCursorPosition(stdhandle, { (SHORT)x, (SHORT)y });
#else
	std::cout << "\033[" << y + 1 << ";" << x + 1 << "H"; // Linux terminal init offscreen
#endif
}

void nostterm::waitkey() {
#ifdef _WIN32
	_getch(); // Read character without echoing
#else
	// Assuming echoing is disabled
	getchar();
#endif
}

uint8 nostterm::getkey() {
#ifdef _WIN32
	return _getch(); // Read character without echoing
#else
	// Assuming echoing is disabled
	uint8 ch = getchar();
	return ch;
#endif
}





// COLORS

void nostcolor::set_color(const ANSIColor color) {
#ifdef _WIN32
	static const HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(stdhandle, &csbi);
	SetConsoleTextAttribute(stdhandle, (csbi.wAttributes & 0xFFF0) | (WORD)static_cast<uint8>(color));
#else
	std::cout << "\033[0;" << static_cast<int>(color) << "m";
#endif
}


void nostcolor::set_bold(const ANSIColor color) {
#ifdef _WIN32
	HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(stdhandle, &csbi);
	SetConsoleTextAttribute(stdhandle, (csbi.wAttributes & 0xFFF0) | (WORD)static_cast<uint8>(color));
#else
	std::cout << "\033[1;" << static_cast<int>(color) << "m";
#endif
}


void nostcolor::set_background(const ANSIBackground color) {
#ifdef _WIN32
	HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(stdhandle, &csbi);
	SetConsoleTextAttribute(stdhandle, (csbi.wAttributes & 0xFFF0) | (WORD)static_cast<uint8>(color));
#else
	std::cout << "\033[" << static_cast<int>(color) << "m";
#endif
}


void nostcolor::reset_color() {
#ifdef _WIN32

	static const HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(stdhandle, &csbi);
	SetConsoleTextAttribute(stdhandle, (csbi.wAttributes & 0xFFF0) | (WORD)7);
#else
	std::cout << "\033[0;0m";
#endif
}



// DRAWING

void nostdraw::clear_screen() {
#ifdef _WIN32
	static const HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	const COORD topleft = {0, 0};
	DWORD charswritten;

	// Clear buffer
	std::cout.flush();

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(!GetConsoleScreenBufferInfo(stdhandle, &csbi)) {
		return;
	}

	const DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

	// Spam space
	FillConsoleOutputCharacter(stdhandle, (TCHAR)' ', length, topleft, &charswritten);

	// Reset every char (clear background)
	FillConsoleOutputAttribute(stdhandle, csbi.wAttributes, length, topleft, &charswritten);

	// Move cursor back to origin
	SetConsoleCursorPosition(stdhandle, topleft);
#else
	std::cout << "\033[2J\033[1;1H" << std::flush;
#endif
}

void nostdraw::clear_line(const uint16 x, const uint16 y) {
	nostterm::move_cursor(x, y);

#ifdef _WIN32
	static const HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get console screen
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(stdhandle, &csbi)) {
		return; // Error, ignore
	}

	// Get nuumber of columns
	DWORD chars_written;
	COORD startPos = csbi.dwCursorPosition;
	startPos.X = 0; // Start clearing from the line beginning

	// Fill the line with spaces to clear it
	FillConsoleOutputCharacter(stdhandle, ' ', csbi.dwSize.X, startPos, &chars_written);

	// Set the cursor back to the start of the line
	SetConsoleCursorPosition(stdhandle, startPos);
#else
	std::cout << "\033[2K" << std::flush;
#endif
}


void nostdraw::printchar(const uint16 x, const uint16 y, const char ch) {
	if(nostterm::inbounds(x, y)) {
		nostterm::move_cursor(x, y);
		std::cout << ch;
	}
}

// Put a single char in some location
void nostdraw::putchar(const uint16 x, const uint16 y, const char ch) {
	if(nostterm::inbounds(x, y)) {
		// this->buffer[x+y*this->width] = ch;
		nostterm::move_cursor(x, y);
		std::cout << ch;
	}
	// this->reset_color();
}

void nostdraw::putchars(const uint16 x, const uint16 y, const char* chars) {
	int mlength = std::strlen(chars);

	for(int i = 0; i < mlength; i++) {
		int nx = x + i;
		nostdraw::putchar(nx, y, chars[i]);
	}
}

// Show a message in some location and wait for any key
void nostdraw::show_message(const uint16 x, const uint16 y, const char* message) {
	nostterm::move_cursor(x, y);
	std::cout << message << std::flush;
	// this->reset_color();
	nostterm::waitkey();
}

// Put a string in some location
void nostdraw::putstring(const uint16 x, const uint16 y, const char* message) {
	nostterm::move_cursor(x, y);
	nostdraw::clear_line(x, y); // Clear whatever is in this line before
	std::cout << message << std::flush;
	// this->reset_color();
}


void nostdraw::show_warning(const char* message) {
	const uint16 term_width = nostterm::term_col();
	const char* more = " --MORE--";
	const uint16 morelen = std::strlen(more);

	if(std::strlen(message) + morelen > 10) {
		std::cerr << "Warning: message is larger than terminal width." << std::endl;
		return;
	}

	char buffer[term_width + 1]; // message + " --MORE--"
	std::strncpy(buffer, message, term_width - morelen); // Copy message, leave space for " --MORE--"
	buffer[term_width - morelen] = '\0'; // Null-terminate before concatenating
	std::strcat(buffer, more); // Append " --MORE--"

#ifdef _WIN32
	nostdraw::putstring(0, 0, buffer);

	while(_getch() != ' ') {
		// loop until space is pressed
	}
#else
	// Assuming echoing is disabled
	nostdraw::putstring(0, 0, buffer);

	char ch;
	do {
		ch = getchar();
	} while (ch != ' '); // Wait until space is pressed
#endif

	// Hide warning
	nostdraw::clear_line(0, 0);
}
