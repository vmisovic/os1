#include "../h/console.hpp"
#include "../lib/hw.h"
#include "../h/thread.hpp"
#include "../h/sys_regs.hpp"
#include "../h/print.hpp"

namespace kernel {

Buffer *Console::input = nullptr;

bool Console::initalized = false;

void Console::Init() {
	if (initalized) return;

	input = new Buffer(CONSOLE_BUFFER_SIZE);

	initalized = true;
}

void Console::Destroy() {
	delete input;
}

void Console::put(char c) {
	//output->put(c);
	__putc(c);
}

char Console::get() {
	return input->get();
}

void Console::handler() {
	if (!initalized) return;
	uint8 *status_reg = (uint8*)CONSOLE_STATUS;
	uint8 *rx_reg = (uint8*)CONSOLE_RX_DATA;
	while (*status_reg & CONSOLE_RX_STATUS_BIT)
		input->put(*rx_reg);
}

// busy wait uart
void __putc(char chr) {
	uint8 *status_reg = (uint8*)CONSOLE_STATUS;
	uint8 *tx_reg = (uint8*)CONSOLE_TX_DATA;
	// cekaj dok nije spreman
	while ((*status_reg & CONSOLE_TX_STATUS_BIT) == 0);
	*tx_reg = chr;
}

// busy wait uart
char __getc() {
	uint8 *status_reg = (uint8*)CONSOLE_STATUS;
	uint8 *rx_reg = (uint8*)CONSOLE_RX_DATA;
	// cekaj dok nije spreman
	while ((*status_reg & CONSOLE_RX_STATUS_BIT) == 0);
	return *rx_reg;
}

}
