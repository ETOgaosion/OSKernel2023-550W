#pragma once

#include <asm/context.h>
#include <uapi/syscall_number.h>

/* syscall function pointer */
extern long (*syscall[NUM_SYSCALLS])();
void handle_syscall_exc(regs_context_t *regs, uint64_t interrupt, uint64_t cause, uint64_t cpuid);
