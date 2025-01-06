#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <inttypes.h>
#include <getopt.h>
#include <sched.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>




#define ARMV8_PMCR_E            (1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /* Cycle counter reset */
#define ARMV8_PMCR_D            (1 << 3) /* CCNT counts every 64th cycle */

#define ARMV8_PMUSERENR_EN      (1 << 0) /* EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /* Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /* Event counter read enable */

#define ARMV8_PMCNTENSET_EL0_EN (1 << 31) /* Performance Monitors Count Enable Set register */


void arm_v8_flush(void* address)
{
  asm volatile ("DC CIVAC, %0" :: "r"(address));
  asm volatile ("DSB ISH");
  asm volatile ("ISB");
}


void arm_v8_access_memory(void* pointer)
{
  volatile uint32_t value;
  asm volatile ("LDR %0, [%1]\n\t"
      : "=r" (value)
      : "r" (pointer)
      );
}

void arm_v8_memory_barrier(void)
{
  asm volatile ("DSB SY");
  asm volatile ("ISB");
}

void arm_v8_prefetch(void* pointer)
{
  asm volatile ("PRFM PLDL3KEEP, [%x0]" :: "p" (pointer));
  asm volatile ("PRFM PLDL2KEEP, [%x0]" :: "p" (pointer));
  asm volatile ("PRFM PLDL1KEEP, [%x0]" :: "p" (pointer));
}


uint64_t arm_v8_get_timing(void) {
  uint64_t result = 0;

  asm volatile("MRS %0, PMCCNTR_EL0" : "=r" (result));

  return result;
}

void arm_v8_timing_init(void) {
  uint32_t value = 0;

  /* Enable Performance Counter */
  asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
  value |= ARMV8_PMCR_E; /* Enable */
  value |= ARMV8_PMCR_C; /* Cycle counter reset */
  value |= ARMV8_PMCR_P; /* Reset all counters */
  value &=(~ARMV8_PMCR_D);
  asm volatile("MSR PMCR_EL0, %0" : : "r" (value));

  // /* Enable cycle counter register */
  // asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
  // value |= ARMV8_PMCNTENSET_EL0_EN;
  // asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value));
}

void arm_v8_timing_terminate(void){
  uint32_t value = 0;
  uint32_t mask = 0;

  /* Disable Performance Counter */
  asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
  mask = 0;
  mask |= ARMV8_PMCR_E; /* Enable */
  mask |= ARMV8_PMCR_C; /* Cycle counter reset */
  mask |= ARMV8_PMCR_P; /* Reset all counters */
  asm volatile("MSR PMCR_EL0, %0" : : "r" (value & ~mask));

  // /* Disable cycle counter register */
  // asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
  // mask = 0;
  // mask |= ARMV8_PMCNTENSET_EL0_EN;
  // asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value & ~mask));
}

 void arm_v8_reset_timing(void) {
  uint32_t value = 0;
  asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
  value |= ARMV8_PMCR_C; /* Cycle counter reset */
  asm volatile("MSR PMCR_EL0, %0" : : "r" (value));
}

int main(){

    int x =10;
    //x = x + 1;
    arm_v8_timing_init();
    arm_v8_access_memory(&x);
    uint64_t delta;
      asm volatile(
       "DSB sy\n"             // 数据同步屏障
       "ISB\n"                // 指令同步屏障
       //"LDR %w[x], [%[addr]]\n"
       "DC CIVAC, %[addr]\n"
       "DSB sy\n"             // 数据同步屏障
        "ISB\n"                // 指令同步屏障
       "MRS X15, PMCCNTR_EL0\n"  // 读取 PMCR_EL0 到 X15
       "DSB sy\n"             // 数据同步屏障
       "ISB\n"                // 指令同步屏障
       "LDR %w[x], [%[addr]]\n" // 从内存加载数据到寄存器
       "DSB sy\n"             // 数据同步屏障
       "ISB\n"                // 指令同步屏障
       "MRS X14, PMCCNTR_EL0\n"  // 再次读取 PMCR_EL0 到 X14
       "DSB sy\n"             // 数据同步屏障
       "ISB\n"                // 指令同步屏障
       "SUB %[delta], X14, X15\n" // 计算 X14 - X15 并存储到 delta
       "DSB sy\n"             // 数据同步屏障
      "ISB\n"                // 指令同步屏障
      : [delta] "=r"(delta)  // 输出操作数
      : [addr] "r"(&x), [x] "r"(x) // 输入操作数
      : "x14", "x15", "memory" // 告诉编译器 X14, X15 和内存可能被修改
   );



    // arm_v8_memory_barrier();
    // uint64_t time = arm_v8_get_timing();
    // arm_v8_memory_barrier();
    //
    // arm_v8_access_memory(&x);
    //
    // arm_v8_memory_barrier();
    // uint64_t time2 = arm_v8_get_timing();
    // arm_v8_memory_barrier();
    //
    // uint64_t delta =  time2 - time;
    printf("delta: %lu\n", delta);
    //arm_v8_timing_terminate();


return 0;
}