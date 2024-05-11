#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP

#include "../lib/hw.h"

namespace kernel {

struct Registers;

extern void interruptWrapper();
void interruptHandler(Registers *saved);
void interruptInit();
void userEcallHandler(Registers *saved);
void systemEcallHandler(Registers *saved);

struct Registers {
//x0 zero Hard-wired zero —
	uint64 zero;
//x1 ra Return address Caller
	uint64 ra;
//x2 sp Stack pointer Callee
	uint64 sp;
//x3 gp Global pointer —
	uint64 gp;
//x4 tp Thread pointer —
	uint64 tp;
//x5–7 t0–2 Temporaries Caller
	uint64 t0;
	uint64 t1;
	uint64 t2;
//x8 s0/fp Saved register/frame pointer Callee
	uint64 s0;
//x9 s1 Saved register Callee
	uint64 s1;
//x10–11 a0–1 Function arguments/return values Caller
	uint64 a0;
	uint64 a1;
//x12–17 a2–7 Function arguments Caller
	uint64 a2;
	uint64 a3;
	uint64 a4;
	uint64 a5;
	uint64 a6;
	uint64 a7;
//x18–27 s2–11 Saved registers Callee
	uint64 s2;
	uint64 s3;
	uint64 s4;
	uint64 s5;
	uint64 s6;
	uint64 s7;
	uint64 s8;
	uint64 s9;
	uint64 s10;
	uint64 s11;
//x28–31 t3–6 Temporaries Caller
	uint64 t3;
	uint64 t4;
	uint64 t5;
	uint64 t6;
};

}

#endif
