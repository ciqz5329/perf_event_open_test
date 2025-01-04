#include <stdio.h>

// 声明汇编函数
extern "C" void trigger_ic_iallu();

int main() {
    printf("Calling trigger_ic_iallu from inline assembly...\n");

    // 内联汇编调用 trigger_ic_iallu
    asm volatile(
        "bl trigger_ic_iallu"  // 调用 trigger_ic_iallu 函数
        :
        :
        : "x0", "x1", "x2", "x8", "x19", "memory"
    );

    printf("Done!\n");
    return 0;
}