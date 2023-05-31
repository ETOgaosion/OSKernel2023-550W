#include <asm/sbi.h>
#include <os/lock.h>
#include <os/pcb.h>
#include <os/smp.h>

spin_lock_t kernel_lock;

void k_smp_init() {
    k_spin_lock_init(&kernel_lock);
}

void k_wakeup_other_hart() {
    sbi_send_ipi(NULL);
    __asm__ __volatile__("csrw sip, zero\n\t");
}

void k_lock_kernel() {
    k_spin_lock_acquire(&kernel_lock);
}

void k_unlock_kernel() {
    k_spin_lock_release(&kernel_lock);
}

pcb_t *volatile *k_get_current_running() {
    return get_current_cpu_id() ? &current_running1 : &current_running0;
}