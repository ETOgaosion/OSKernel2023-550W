#include <asm/regs.h>
#include <os/pcb.h>
#include <os/signal.h>

regs_context_t signal_context_buf;
switchto_context_t signal_switch_buf;

sigaction_table_t sig_table[NUM_MAX_SIGNAL];

/**
 * @brief send signal to appointed process
 * @param signum the sig id
 * @param pcb the target pcb
 */
void k_send_signal(int signum, pcb_t *pcb) {
    if (signum == 0) {
        return;
    }
    uintptr_t sig_value = (1lu << (signum - 1));
    if ((sig_value & pcb->sig_mask) != 0) {
        pcb->sig_pend |= sig_value;
    } else {
        pcb->sig_recv |= sig_value;
    }
    if (pcb->status == TASK_BLOCKED) {
        // log(0, "receiver is unblocked", pcb->status);
        // printk("%s was unblocked\n", pcb->name);
        k_pcb_unblock(&pcb->list, &ready_queue, UNBLOCK_TO_LIST_STRATEGY);
    }
}

/**
 * @brief trans pending sign to unpending
 * @param pcb the pcb
 * @return no return
 */
void trans_pended_signal_to_recv(pcb_t *pcb) {
    if (pcb->sig_pend != 0) {
        uint64_t prev_recv = pcb->sig_recv;
        pcb->sig_recv |= (pcb->sig_pend & ~(pcb->sig_mask));
        pcb->sig_pend &= ~(prev_recv ^ pcb->sig_recv);
    }
}

/**
 * @brief change the pcb mask and trans pended signal to unpended
 * @param pcb the pcb
 * @param newmask the mask
 * @return no return
 */
void change_signal_mask(pcb_t *pcb, uint64_t newmask) {
    pcb->sig_mask = newmask;
    // tmp no set
    trans_pended_signal_to_recv(pcb);
}

void k_signal_handler() {
    if ((*current_running)->status == TASK_EXITED || (*current_running)->sig_recv == 0 || (*current_running)->handling_signal) {
        return;
    }
    (*current_running)->handling_signal = true;
    for (uint8_t i = 0; i < NUM_SIG; i++) {
        if (ISSET_SIG(i + 1, *current_running)) {
            // 1. clear this signal
            CLEAR_SIG(i + 1, *current_running);
            // 2. change mask
            (*current_running)->prev_mask = (*current_running)->sig_mask;
            change_signal_mask(*current_running, (*current_running)->sigactions[i].sa_mask.sig[0]);
            // 3. goto handler
            if ((*current_running)->sigactions[i].sa_handler == SIG_DFL) {
                switch (i + 1) {
                case SIGKILL:
                case SIGSEGV:
                case SIGALRM:
                    (*current_running)->handling_signal = false;
                    sys_exit(-(i + 1));
                    break;
                case SIGCHLD:
                case SIGCANCEL:
                    (*current_running)->handling_signal = false;
                    break;
                default:
                    break;
                }
                change_signal_mask(*current_running, (*current_running)->prev_mask);
            } else if ((*current_running)->sigactions[i].sa_handler != SIG_IGN) {
                k_memcpy((uint8_t *)&signal_context_buf, (const uint8_t *)(*current_running)->save_context, sizeof(regs_context_t));
                k_memcpy((uint8_t *)&signal_switch_buf, (const uint8_t *)(*current_running)->switch_context, sizeof(switchto_context_t));
                // context
                signal_context_buf.regs[reg_ra] = (reg_t)(USER_STACK_ADDR - SIZE_RESTORE);
                signal_context_buf.regs[reg_a0] = (reg_t)(i + 1);
                signal_context_buf.regs[reg_a2] = (reg_t)(SIGNAL_HANDLER_ADDR - sizeof(ucontext_t));
                signal_context_buf.sepc = (reg_t)(*current_running)->sigactions[i].sa_handler;
                // switch
                uint64_t offset = sizeof(regs_context_t) + sizeof(switchto_context_t);
                signal_switch_buf.regs[switch_reg_sp] = pcb->kernel_sp - offset;
                (*current_running)->kernel_sp -= offset;
                (*current_running)->save_context = (regs_context_t *)((uint64_t)(*current_running)->save_context - offset);
                (*current_running)->switch_context = (switchto_context_t *)((uint64_t)(*current_running)->switch_context - offset);
                enter_signal_trampoline(&signal_context_buf, &signal_switch_buf);
            }
            break;
        }
    }
}

long sys_rt_sigaction(int signum, const sigaction_t *act, sigaction_t *oldact, size_t sigsetsize) {
    if (signum > NUM_SIG) {
        return -EINVAL;
    }
    sigaction_t *sig = &(*current_running)->sigactions[signum - 1];
    if (oldact) {
        k_memcpy((uint8_t *)oldact, (const uint8_t *)sig, sizeof(sigaction_t));
    }
    if (act) {
        k_memcpy((uint8_t *)sig, (const uint8_t *)act, sizeof(sigaction_t));
    }
    return 0;
}

long sys_rt_sigprocmask(int how, sigset_t *set, sigset_t *oset, size_t sigsetsize) {
    if (oset) {
        k_memcpy((uint8_t *)&oset->sig[0], (const uint8_t *)&(*current_running)->sig_mask, sizeof(sigset_t));
    }
    if (!set) {
        return oset->sig[0];
    }
    switch (how) {
    case SIG_BLOCK:
        (*current_running)->sig_mask |= set->sig[0];
        break;
    case SIG_UNBLOCK:
        (*current_running)->sig_mask &= ~(set->sig[0]);
    case SIG_SETMASK:
        (*current_running)->sig_mask = set->sig[0];

    default:
        break;
    }
    return 0;
}

long sys_rt_sigreturn() {
    change_signal_mask((*current_running), (*current_running)->prev_mask);
    (*current_running)->handling_signal = false;
    uint64_t offset = (sizeof(regs_context_t) + sizeof(switchto_context_t));
    (*current_running)->kernel_sp += offset;
    (*current_running)->save_context = (regs_context_t *)(((uint64_t)(*current_running)->save_context) + offset);
    (*current_running)->switch_context = (switchto_context_t *)(((uint64_t)(*current_running)->switch_context) + offset);
    exit_signal_trampoline();
    return 0;
}