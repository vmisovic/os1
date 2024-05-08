#include "../h/console.hpp"
#include "../lib/hw.h"

void __putc(char chr) {
	uint8 *status_reg = (uint8*)CONSOLE_STATUS;
	uint8 *tx_reg = (uint8*)CONSOLE_TX_DATA;
	// cekaj dok nije spreman
	// guraj u bafer
	while ((*status_reg & CONSOLE_TX_STATUS_BIT) == 0);
	*tx_reg = chr;
}

char __getc() {
	uint8 *status_reg = (uint8*)CONSOLE_STATUS;
	uint8 *rx_reg = (uint8*)CONSOLE_RX_DATA;
	// cekaj dok nije spreman
	while ((*status_reg & CONSOLE_RX_STATUS_BIT) == 0);
	// stap kanap resenje
	return *rx_reg;
}
