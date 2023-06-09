#pragma once

/* K_ROUNDing; only works for n = power of two */
#define K_ROUND(a, n) (((((uint64_t)(a)) + (n)-1)) & ~((n)-1))
#define K_ROUNDDOWN(a, n) (((uint64_t)(a)) & ~((n)-1))

#define k_wrap(x, len) ((x) & ~(len))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int k_min(int a, int b);