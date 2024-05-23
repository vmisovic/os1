#include "../h/console.hpp"
#include "../lib/hw.h"
#include "../h/thread.hpp"
#include "../h/sys_regs.hpp"

namespace kernel {

Buffer *Console::input = nullptr;
Buffer *Console::output = nullptr;

static uint8 *status_reg;
static uint8 *tx_reg;
static uint8 *rx_reg;

void Console::Init() {
	status_reg = (uint8*)CONSOLE_STATUS;
	tx_reg = (uint8*)CONSOLE_TX_DATA;
	rx_reg = (uint8*)CONSOLE_RX_DATA;

	input = new Buffer(CONSOLE_BUFFER_SIZE);
	output = new Buffer(CONSOLE_BUFFER_SIZE);
	Thread::create(IOThreadBody, nullptr, nullptr, Thread::Mode::SYSTEM);
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

bool isRXready() { return *status_reg & CONSOLE_RX_STATUS_BIT; }
bool isTXready(int buf_cnt) {
	return (buf_cnt > 0) && (*status_reg & CONSOLE_TX_STATUS_BIT);
}

void Console::handler() {
	while (isRXready() || isTXready(output->getCnt())) {
		if (isRXready())
			input->put(*rx_reg);
		if (isTXready(output->getCnt()))
			*tx_reg = output->get();
	}
}

void Console::IOThreadBody(void *) {
	write_sstatus(read_sstatus() & ~SSIE); // interrupt disable

	while (true) {
		Console::handler();
		Thread::yield();
	}
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
