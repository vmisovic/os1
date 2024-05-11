#ifndef SYS_REGS_HPP
#define SYS_REGS_HPP

#include "../lib/hw.h"

namespace kernel {

void terminate();

uint64 read_sstatus();
uint64 read_sip();
uint64 read_sie();
uint64 read_sepc();
uint64 read_scause();
uint64 read_stvec();

void write_sstatus(uint64 sstatus);
void write_sip(uint64 sip);
void write_sie(uint64 sie);
void write_sepc(uint64 sepc);
void write_scause(uint64 scause);
void write_stvec(uint64 stvec);

static const uint64 SSIE = (1ULL << 1);
static const uint64 SEIE = (1ULL << 9);
static const uint64 SSIP = (1ULL << 1);
static const uint64 SEIP = (1ULL << 9);
static const uint64 SPP  = (1ULL << 8);

inline void terminate() {
	const uint32 stopCode = 0x5555;
	volatile uint32 *stopAddr = (uint32*)0x100000;
	*stopAddr = stopCode;
}

inline uint64 read_sstatus() {
	uint64 sstatus = 0;
	__asm__ __volatile__ ("csrr %[sstatus], sstatus" : [sstatus]"=r"(sstatus));
	return sstatus;
}

inline uint64 read_sip() {
	uint64 sip = 0;
	__asm__ __volatile__ ("csrr %[sip], sip" : [sip]"=r"(sip));
	return sip;
}

inline uint64 read_sie() {
	uint64 sie = 0;
	__asm__ __volatile__ ("csrr %[sie], sie" : [sie]"=r"(sie));
	return sie;
}

inline uint64 read_sepc() {
	uint64 sepc = 0;
	__asm__ __volatile__ ("csrr %[sepc], sepc" : [sepc]"=r"(sepc));
	return sepc;
}

inline uint64 read_scause() {
	uint64 scause = 0;
	__asm__ __volatile__ ("csrr %[scause], scause" : [scause]"=r"(scause));
	return scause;
}

inline uint64 read_stvec() {
	uint64 stvec = 0;
	__asm__ __volatile__ ("csrr %[stvec], stvec" : [stvec]"=r"(stvec));
	return stvec;
}

inline void write_sstatus(uint64 sstatus) {
	__asm__ __volatile__ ("csrw sstatus, %[sstatus]" : : [sstatus]"r"(sstatus));
}

inline void write_sip(uint64 sip) {
	__asm__ __volatile__ ("csrw sip, %[sip]" : : [sip]"r"(sip));
}

inline void write_sie(uint64 sie) {
	__asm__ __volatile__ ("csrw sie, %[sie]" : : [sie]"r"(sie));
}

inline void write_sepc(uint64 sepc) {
	__asm__ __volatile__ ("csrw sepc, %[sepc]" : : [sepc]"r"(sepc));
}

inline void write_scause(uint64 scause) {
	__asm__ __volatile__ ("csrw scause, %[scause]" : : [scause]"r"(scause));
}

inline void write_stvec(uint64 stvec) {
	__asm__ __volatile__ ("csrw stvec, %[stvec]" : : [stvec]"r"(stvec));
}

}

#endif
