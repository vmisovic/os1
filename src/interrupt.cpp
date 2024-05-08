#include "../h/interrupt.hpp"
#include "../h/sys_regs.hpp"
#include "../h/ecall_codes.h"
#include "../h/thread.hpp"
#include "../h/memory.hpp"
#include "../h/print.hpp"

Thread::Mode currentMode = Thread::Mode::SYSTEM;

void interruptInit() {
	write_stvec((uint64)interruptWrapper);
	write_sstatus(read_sstatus() | SSIE);
}

void interruptHandler(Registers *saved) {
	currentMode = Thread::Mode::SYSTEM;
	uint64 scause = read_scause();
	uint64 bnt = (uint64)(scause >> 63);
	uint64 val = (uint64)(scause & 0xff);

	int code;
	if (bnt) {
		// Spoljasnji prekidi
		switch (val) {
		case 1: // Softverski prekid iz 3. rezima - TAJMER
			Thread::running->timerCounter++;
			if (Thread::running->timerCounter >= DEFAULT_TIME_SLICE) {
				printString("timer switch\n");
				Thread::dispatch();
			}
			break;
		case 9: // Spoljašnji hardverski prekid - KONZOLA
			code = plic_claim();
			if (code == CONSOLE_IRQ) { // stigao prekid od konzole 
				;;
			}
			else { // prekid od drugih uredjaja
				;;
			}
			plic_complete(code);
			break;
		}
	} else {
		// Softverski prekidi
		switch (val) {
		case 2:
		case 5:
		case 7:
			printString("OZBILJAN ZAJEB\n");
			printString("scause: ");
			printHex(read_scause());
			printString("\tsepc: ");
			printHex(read_sepc());
			Thread::running->finished = true;
			Thread::dispatch();
			break;
		case 8:	// ecall iz korisničkog režima
			printString("ecall korisnicki rezim\n");
			write_sepc(read_sepc() + 4);
			userEcallHandler(saved);
			break;
		case 9:	// ecall iz sistemskog režima
			printString("ecall sistemski rezim\n");
			write_sepc(read_sepc() + 4);
			systemEcallHandler(saved);
			break;
		}
	}
	currentMode = Thread::running->mode;
	write_sip(read_sip() & ~SSIE);
}

void userEcallHandler(Registers *saved) {
	switch (saved->a0) {
	case MEM_ALLOC:
		printString("ecall alloc\n");
		printString("HOCU DA VIDIM OVO SAAD!!!1\n");
		saved->a0 = (uint64)blkAlloc((size_t)saved->a1);
		break;
	case MEM_FREE:
		printString("kurac free\n");
		printString("HOCU DA VIDIM OVO SAAD!!!1\n");
		/*for printing heap memory of freed segment
		for (int i = -2; i < 8; i++) {
			print_hex((uint64)((uint64*)reg->a1 + i));
			__putc('\t');
			print_hex(*((uint64*)reg->a1 + i));
			__putc('\n');
		}*/
		saved->a0 = blkFree((void*)saved->a1);
		// ako nije nula onda je greska, nista nije freeovano
		break;
	case THREAD_CREATE:
		// a1 handle*
		// a2 thread_routine
		// a3 arg*
		// a4 stack*
		*(Thread**)saved->a1 = new Thread(
			(void (*)(void*))saved->a2,
			(void*)saved->a3,
			(void*)saved->a4,
			Thread::Mode::USER
		);
		saved->a0 = (*(Thread**)saved->a1 != nullptr)? 0 : -1;
		printHex((uint64)*(Thread**)saved->a1);
		break;
	case THREAD_EXIT:
		printString("thread exit\n");
		Thread::running->finished = true;
		delete Thread::running;
		Thread::running = nullptr;
		Thread::dispatch();
		break;
	case THREAD_DISPATCH:
		printString("thread dispatch\n");
		Thread::dispatch();
		printString("\n");
		break;
	case GETC:
		saved->a0 = __getc();
		break;
	case PUTC:
		__putc((char)saved->a1);
		break;
	}
}

void systemEcallHandler(Registers *saved) {
	switch (saved->a0) {
	case THREAD_EXIT:
		printString("thread exit\n");
		Thread::running->finished = true;
		Thread::dispatch();
		break;
	case THREAD_DISPATCH:
		printString("thread dispatch\n");
		Thread::dispatch();
		printString("\n");
		break;
	}
}
