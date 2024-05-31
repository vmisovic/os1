#include "../h/console.hpp"
#include "../lib/hw.h"
#include "../h/thread.hpp"
#include "../h/sys_regs.hpp"
#include "../h/print.hpp"

namespace kernel {

Buffer *Console::input = nullptr;
Buffer *Console::output = nullptr;

bool Console::initalized = false;

static uint8 *status_reg;
static uint8 *tx_reg;
static uint8 *rx_reg;

void Console::Init() {
	if (initalized) return;
	status_reg = (uint8*)CONSOLE_STATUS;
	tx_reg = (uint8*)CONSOLE_TX_DATA;
	rx_reg = (uint8*)CONSOLE_RX_DATA;

	input = new Buffer(CONSOLE_BUFFER_SIZE);
	output = new Buffer(CONSOLE_BUFFER_SIZE);

	initalized = true;
}

void Console::Destroy() {
	// empty output buffer
	while (output->getCnt() > 0)
		__putc(output->get());
	delete input;
	delete output;
}

void Console::put(char c) {
	output->put(c);
}

char Console::get() {
	return input->get();
}

void Console::handle_input() {
	if (!initalized || input->getCnt() >= CONSOLE_BUFFER_SIZE)
		return;
	while (*status_reg & CONSOLE_RX_STATUS_BIT)
		input->put(*rx_reg);
}

void Console::handle_output() {
	while ((*status_reg & CONSOLE_TX_STATUS_BIT) && output->getCnt() > 0)
		*tx_reg = output->get();
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
