#ifndef ECALL_CODES
#define ECALL_CODES

#define MEM_ALLOC 0x01
#define MEM_FREE 0x02

#define THREAD_CREATE 0x11
#define THREAD_EXIT 0x12
#define THREAD_DISPATCH 0x13
#define THREAD_GETID 0x14

#define SEM_OPEN 0x21
#define SEM_CLOSE 0x22
#define SEM_WAIT 0x23
#define SEM_SIGNAL 0x24
#define SEM_TIMEDWAIT 0x25
#define SEM_TRYWAIT 0x26

#define TIME_SLEEP 0x31

#define GETC 0x41
#define PUTC 0x42

#endif
