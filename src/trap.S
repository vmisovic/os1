// extern interruptHandler
.extern _ZN6kernel16interruptHandlerEPNS_9RegistersE
// export interruptWrapper
.global _ZN6kernel16interruptWrapperEv

.align 4
.section .text
.type _ZN6kernel16interruptWrapperEv @function
_ZN6kernel16interruptWrapperEv:
	addi sp, sp, -8 * 32
	.irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	sd x\index, \index * 8(sp)
	.endr

	// call interruptHandler with sp as pointer to saved registers
	mv a0, sp
	call _ZN6kernel16interruptHandlerEPNS_9RegistersE

	.irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	ld x\index, \index * 8(sp)
	.endr
	addi sp, sp, 8 * 32

	sret
