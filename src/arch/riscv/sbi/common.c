#include <asm/common.h>
#include <asm/sbi.h>

void port_write_ch(char ch) { sbi_console_putchar((int)ch); }

void port_write(char *str) { sbi_console_putstr(str); }

int sys_port_read() { return sbi_console_getchar(); }
