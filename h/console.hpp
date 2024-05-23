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
	static void handler();

private:
	static Buffer *input;
	static Buffer *output;

	static void IOThreadBody(void *);
};

// busy wait uart
void __putc(char chr);
char __getc();

}

#endif
