#pragma once

/* from linux/arch/riscv/include/asm/cmpxchg.h */

#include <common/types.h>

static inline uint32_t atomic_swap(uint32_t val, ptr_t mem_addr) {
    uint32_t ret;
    __asm__ __volatile__("amoswap.w.aqrl %0, %2, %1\n" : "=r"(ret), "+A"(*(uint32_t *)mem_addr) : "r"(val) : "memory");
    return ret;
}

static inline uint64_t atomic_swap_d(uint64_t val, ptr_t mem_addr) {
    uint64_t ret;
    __asm__ __volatile__("amoswap.d.aqrl %0, %2, %1\n" : "=r"(ret), "+A"(*(uint64_t *)mem_addr) : "r"(val) : "memory");
    return ret;
}

static inline int fetch_add(volatile void *obj, int arg) {
    uint32_t ret;
    __asm__ __volatile__("amoadd.w.aqrl %0, %2, %1\n" : "=r"(ret), "+A"(*(uint32_t *)obj) : "r"(arg) : "memory");
    return ret;
}

static inline uint32_t atomic_load(volatile uint32_t *obj) {
    uint32_t arg = UINT32_MAX;
    uint32_t ret;
    __asm__ __volatile__("amoand.w.aqrl %0, %2, %1\n" : "=r"(ret), "+A"(*(uint32_t *)obj) : "r"(arg) : "memory");
    return ret;
}

/* if *mem_addr == old_val, then *mem_addr = new_val, else return *mem_addr */
static inline uint32_t atomic_cmpxchg(uint32_t old_val, uint32_t new_val, ptr_t mem_addr) {
    uint32_t ret;
    register unsigned int __rc;
    __asm__ __volatile__("0:    lr.w %0, %2\n"
                         "    bne  %0, %z3, 1f\n"
                         "    sc.w.rl %1, %z4, %2\n"
                         "    bnez %1, 0b\n"
                         "    fence rw, rw\n"
                         "1:\n"
                         : "=&r"(ret), "=&r"(__rc), "+A"(*(uint32_t *)mem_addr)
                         : "rJ"(old_val), "rJ"(new_val)
                         : "memory");
    return ret;
}

/* if *mem_addr == old_val, then *mem_addr = new_val, else return *mem_addr */
static inline uint64_t atomic_cmpxchg_d(uint64_t old_val, uint64_t new_val, ptr_t mem_addr) {
    uint64_t ret;
    register unsigned int __rc;
    __asm__ __volatile__("0:    lr.d %0, %2\n"
                         "    bne  %0, %z3, 1f\n"
                         "    sc.d.rl %1, %z4, %2\n"
                         "    bnez %1, 0b\n"
                         "    fence rw, rw\n"
                         "1:\n"
                         : "=&r"(ret), "=&r"(__rc), "+A"(*(uint64_t *)mem_addr)
                         : "rJ"(old_val), "rJ"(new_val)
                         : "memory");
    return ret;
}