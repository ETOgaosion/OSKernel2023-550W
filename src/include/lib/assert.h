#pragma once

#include <lib/stdio.h>

/* clang-format off */
static inline void _panic(const char* file_name,int lineno, const char* func_name)
{
    printk("Assertion failed at %s in %s:%d\n\r",
           func_name,file_name,lineno);
    for(;;);
}

#define assert(cond)                                 \
    {                                                \
        if (!(cond)) {                               \
            _panic(__FILE__, __LINE__,__FUNCTION__); \
        }                                            \
    }


static inline void panic(char *s)
{
    printk("panic: ");
    printk(s);
    printk("\n");
    assert(0);
}

/* clang-format on */