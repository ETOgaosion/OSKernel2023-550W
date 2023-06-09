#pragma once

#include <common/types.h>
#include <os/ipc.h>

#define FS_KERNEL_ADDR 0xffffffc084000000lu

#define START_BLOCK 2000
#define MEM_SIZE 32
#define PAGE_SIZE 4096 // 4K
#define INIT_KERNEL_STACK 0xffffffc081000000lu
#define FREEMEM (INIT_KERNEL_STACK + PAGE_SIZE * 10)
#define FREEMEMK 0xffffffc083000000lu
#define USER_STACK_ADDR 0xf00010000lu

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_GROWSDOWN 0X01000000
#define PROT_GROWSUP 0X02000000

#define MAP_FILE 0
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0X02
#define MAP_FAILED ((void *)-1)

#define MAP_KERNEL 1
#define MAP_USER 2

typedef uint64_t PTE;

typedef struct shmid_ds {
    ipc_perm_t shm_perm;         /* operation perms */
    int shm_segsz;               /* size of segment (bytes) */
    kernel_old_time_t shm_atime; /* last attach time */
    kernel_old_time_t shm_dtime; /* last detach time */
    kernel_old_time_t shm_ctime; /* last change time */
    kernel_ipc_pid_t shm_cpid;   /* pid of creator */
    kernel_ipc_pid_t shm_lpid;   /* pid of last operator */
    unsigned short shm_nattch;   /* no. of current attaches */
    unsigned short shm_unused;   /* compatibility */
    void *shm_unused2;           /* ditto - used by DIPC */
    void *shm_unused3;           /* unused */
} shmid_ds_t;

typedef struct mmap_arg_struct {
    unsigned long addr;
    unsigned long len;
    unsigned long prot;
    unsigned long flags;
    unsigned long fd;
    unsigned long offset;
} mmap_arg_struct_t;

extern ptr_t memCurr;
extern int diskpg_num;
extern uint64_t diskpg[1000];

void k_mm_move_to_disk(uint64_t pg_kva, uint64_t user_va);
void k_mm_get_back_disk(uint64_t va, uint64_t new_addr);
void k_mm_en_invalid(uint64_t pa_kva, PTE *pgdir);
ptr_t k_mm_alloc_mem(int numPage, uint64_t user_va);
ptr_t k_mm_alloc_page(int numPage);
void k_mm_fork_pgtable(PTE *dest_pgdir, PTE *src_pgdir);
void k_mm_fork_page_helper(uintptr_t va, PTE *destpgdir, PTE *srcpgdir);
uint64_t k_mm_get_kva_from_va(uint64_t va, PTE *pgdir);
long k_mm_getpa(uint64_t va);
void k_mm_map_kp(uint64_t va, uint64_t pa, PTE *pgdir);

void k_mm_free_page(ptr_t baseAddr, int numPage);
void *k_mm_malloc(size_t size);
void k_mm_share_pgtable(PTE *dest_pgdir, PTE *src_pgdir);
uintptr_t k_mm_alloc_page_helper(uintptr_t va, PTE *pgdir);
long k_mm_shm_page_get(int key);
long k_mm_shm_page_dt(uintptr_t addr);
void k_mm_map(uint64_t va, uint64_t pa, PTE *pgdir);
void k_mm_getback_page(int pid);
PTE *k_mm_get_kva(PTE entry);
void k_mm_cancel_pg(PTE *pgdir);
uint64_t k_mm_alloc_newva();

long sys_shmget(key_t key, size_t size, int flag);
long sys_shmctl(int shmid, int cmd, shmid_ds_t *buf);
long sys_shmat(int shmid, char *shmaddr, int shmflg);
long sys_shmdt(char *shmaddr);

long sys_brk(unsigned long brk);

long sys_swapon(const char *specialfile, int swap_flags);
long sys_swapoff(const char *specialfile);

long sys_mprotect(unsigned long start, size_t len, unsigned long prot);
long sys_msync(unsigned long start, size_t len, int flags);
long sys_mlock(unsigned long start, size_t len);
long sys_munlock(unsigned long start, size_t len);

long sys_madvise(unsigned long start, size_t len, int behavior);