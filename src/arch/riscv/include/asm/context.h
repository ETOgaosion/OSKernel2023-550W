#pragma once

#include <common/types.h>

/* used to save register infomation */
typedef struct regs_context {
    /* Saved main processor registers.*/
    reg_t regs[32];

    /* Saved special registers. */
    reg_t sstatus;
    reg_t sepc;
    reg_t sbadaddr;
    reg_t scause;
    reg_t sscratch;
} regs_context_t;

/* used to save register infomation in switch_to */
typedef struct switchto_context {
    /* Callee saved registers.*/
    reg_t regs[14];
} switchto_context_t;
