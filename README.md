# Nostermus
**Nostermus** is a tiny library for terminal drawing. It provides methods for rendering characters and messages in the terminal, with many utility methods. Nostermus does not require any external library and is compatible for Linux and Windows

This library is being used on [Nocturnus](https://github.com/vaaako/Nocturnus) a Terminal-based Roguelike library


# Compiling
## Windows and Linux
Use the following command:
```sh
make
```

This will build the library as a static library

If you're compiling for Windows from a Linux environment, you can specify the compiler
```sh
make CXX=i686-w64-mingw32-g++
```


# Usage example
```cpp
#include "nostermus.hpp"

int main() {
	nostterm::disable_echoing();

	nostdraw::clear_screen();
	nostdraw::putstring(0, 0, "Press any key to start");

	int x = 14;
	int y = 14;

	char ch;
	while((ch = nostterm::getkey()) != 'q') {
		nostdraw::clear_screen();

		// Handle movement
		switch (ch) {
			case 'd':
				x += 1;
				break;

			case 'a':
				x -= 1;
				break;

			case 's':
				y += 1;
				break;

			case 'w':
				y -= 1;
				break;

			// Warning example
			case 'e':
				// Will only hide if space is pressed
				nostdraw::show_warning("This is a warning example");
				// nostdraw::show_message(char*) is almost the same, but hides if any key is pressed
				break;
		}

		nostcolor::set_color(ANSIColor::YELLOW);
		nostdraw::putchar(x, y, '@');

		// you can set bold and background too
	}
}
```

To see more features, look [`nostermus.hpp`](nostermus.hpp)
