#include "../h/interrupt.hpp"
#include "../h/sys_regs.hpp"
#include "../h/ecall_codes.h"
#include "../h/scheduler.hpp"
#include "../h/thread.hpp"
#include "../h/semaphore.hpp"
#include "../h/memory.hpp"
#include "../h/print.hpp"
#include "../h/console.hpp"

// used in new and delete operators
kernel::Thread::Mode currentMode = kernel::Thread::Mode::SYSTEM;

namespace kernel {

void interruptInit() {
	printString("Init stvec\n", PRINT_INFO);
	write_stvec((uint64)interruptWrapper);
}

void interruptEnable() {
	write_sstatus(read_sstatus() | SSIE);
}

void interruptDisable() {
	write_sstatus(read_sstatus() & ~SSIE);
}

void interruptHandler(volatile Registers *saved) {
	currentMode = Thread::Mode::SYSTEM;
	volatile uint64 scause = read_scause();
	volatile uint64 bnt = (uint64)(scause >> 63);
	volatile uint64 val = (uint64)(scause & 0xff);
	volatile uint64 sepc = read_sepc();
	volatile uint64 sstatus = read_sstatus();

	volatile int code;
	if (bnt) {
		// Spoljasnji prekidi
		switch (val) {
		case 1: // Softverski prekid iz 3. rezima - TAJMER
			Scheduler::tick(); // checks sleeping threads
			Thread::running->timerCounter++;
			if (Thread::running->timerCounter >= DEFAULT_TIME_SLICE) {
				printString("timer switch\n", PRINT_TIMER_SWITCH);
				Thread::dispatch();
			}
			break;
		case 9: // Spoljašnji hardverski prekid - KONZOLA
			code = plic_claim();
			if (code == CONSOLE_IRQ) { // stigao prekid od konzole 
				Console::handler();
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
			printString("OZBILJNA GRESKA\n", PRINT_ERROR);
			printString("scause: ", PRINT_ERROR);
			printHex(read_scause(), PRINT_ERROR);
			printString("\tsepc: ", PRINT_ERROR);
			printHex(read_sepc(), PRINT_ERROR);
			printString("\nUBIJAM NIT\n", PRINT_ERROR);
			Thread::exit();
			break;
		case 8:	// ecall iz korisničkog režima
		case 9:	// ecall iz sistemskog režima
			ecallHandler(saved);
			sepc += 4;
			break;
		}
	}
	currentMode = (sstatus & SPP)? Thread::Mode::SYSTEM : Thread::Mode::USER;
	write_sepc(sepc);
	write_sstatus(sstatus);
	write_sip(read_sip() & ~SSIE);
}

void ecallHandler(volatile Registers *saved) {
	switch (saved->a0) {
	case MEM_ALLOC:
		printString("ecall alloc\n", PRINT_ECALL);
		saved->a0 = (uint64)blkAlloc((size_t)saved->a1);
		break;
	case MEM_FREE:
		printString("ecall free\n", PRINT_ECALL);
		saved->a0 = blkFree((void*)saved->a1);
		break;
	case THREAD_CREATE:
		printString("ecall thread create\n", PRINT_ECALL);
		*(Thread**)saved->a1 = Thread::create(
			(void (*)(void*))saved->a2,
			(void*)saved->a3,
			(void*)saved->a4,
			Thread::Mode::USER
		);
		saved->a0 = (*(Thread**)saved->a1 != nullptr)? 0 : -1;
		break;
	case THREAD_EXIT:
		printString("ecall thread exit\n", PRINT_ECALL);
		Thread::exit();
		saved->a0 = 0;
		break;
	case THREAD_DISPATCH:
		printString("ecall thread dispatch\n", PRINT_DISPATCH);
		Thread::dispatch();
		saved->a0 = 0;
		break;
	case TIME_SLEEP:
		printString("ecall time sleep\n", PRINT_ECALL);
		Thread::putToSleep((time_t)saved->a1);
		saved->a0 = 0;
		break;
	case SEM_OPEN:
		printString("ecall sem open\n", PRINT_ECALL);
		*(Semaphore**)saved->a1 = new Semaphore((unsigned)saved->a2);
		saved->a0 = (*(Semaphore**)saved->a1 != nullptr)? 0 : -1;
		break;
	case SEM_CLOSE:
		printString("ecall sem close\n", PRINT_ECALL);
		delete (Semaphore*)saved->a1;
		saved->a0 = 0;
		break;
	case SEM_WAIT:
		printString("ecall sem wait\n", PRINT_ECALL);
		((Semaphore*)saved->a1)->wait();
		saved->a0 = Thread::running->ret_val;
		break;
	case SEM_SIGNAL:
		printString("ecall sem signal\n", PRINT_ECALL);
		((Semaphore*)saved->a1)->signal();
		saved->a0 = 0;
		break;
	case SEM_TIMEDWAIT:
		printString("ecall sem timedWait\n", PRINT_ECALL);
		((Semaphore*)saved->a1)->timedWait((time_t)saved->a2);
		saved->a0 = Thread::running->ret_val;
		break;
	case SEM_TRYWAIT:
		printString("ecall sem tryWait\n", PRINT_ECALL);
		saved->a0 = ((Semaphore*)saved->a1)->tryWait();
		break;
	case GETC:
		saved->a0 = Console::get();
		break;
	case PUTC:
		Console::put((char)saved->a1);
		break;
	}
}

}
