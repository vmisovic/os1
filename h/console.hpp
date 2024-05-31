#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "../h/buffer.hpp"

namespace kernel {

#define CONSOLE_BUFFER_SIZE 10000

class Console {
public:
	static void Init();
	static void Destroy();

	static void put(char c);
	static char get();
	static void handle_input();
	static void handle_output();

private:
	static Buffer *input;
	static Buffer *output;

	static bool initalized;
};

// busy wait uart
void __putc(char chr);
char __getc();

}

#endif
