// export switchContext
.global _ZN6kernel6Thread13switchContextEPNS0_7ContextES2_

.text
// void kernel::Thread::switchContext(Context oldC, Context newC);
.type _ZN6kernel6Thread13switchContextEPNS0_7ContextES2_ @function
_ZN6kernel6Thread13switchContextEPNS0_7ContextES2_:
	// a0 - oldC
	// a1 - newC
	/* Context: { ra, sp, sepc, sstatus } */

	// if oldC (a0) is not null
	beqz	a0, oldIsNull
	// save ra and sp of old context 
	sd	ra, 0 * 8(a0)
	sd	sp, 1 * 8(a0)
	// use registers ra and sp to read sepc and sstatus
	csrr	ra, sepc
	csrr	sp, sstatus
	// store sepc and sstatus of old context
	sd	ra, 2 * 8(a0)
	sd	sp, 3 * 8(a0)

oldIsNull:
	// read saved sepc and sstatus of new context
	ld	sp, 3 * 8(a1)
	ld	ra, 2 * 8(a1)
	// write those values to system registers
	csrw	sstatus, sp
	csrw	sepc, ra
	// restore saved sp and ra of new context
	ld	sp, 1 * 8(a1)
	ld	ra, 0 * 8(a1)

	ret
