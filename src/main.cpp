#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <cstdint>

// 声明全局变量 x


void AddSerializeInstructionToCodePage() {
    __asm__ volatile(
        "dsb sy\n"  // 执行数据同步屏障
        "isb \n"

        // 启动性能计数器
        "MOV X9, #1\n"  // 将值1写入 PMCR_EL0，启用性能计数器
        "MSR PMCR_EL0, X9\n"  // 设置 PMCR_EL0，启用计数器

        "MSR PMCCNTR_EL0, XZR\n " // 清空 PMCCNTR_EL0 寄存器
        "MRS X9, PMCCNTR_EL0\n"  // 读取当前的计数值
        "Mov X10, X9\n"  // 保存初始计数值

        // 执行一些代码（模拟一些处理）
        "dsb sy\n"  // 执行数据同步屏障
        "isb \n"

        // 再次读取计数器的值
        "MOV X9, #0\n"
        "MRS X9, PMCCNTR_EL0\n"
        "SUB X0, X9, X10\n"  // 计算时间差

        // 使用寄存器来加载 x 的地址
        "ldr x1, =x\n"  // 将 x 的地址加载到寄存器 x1 中
        "str x0, [x1]\n"  // 将结果存储到 x 指向的位置
        :  // 输出操作数部分
        :  // 没有显式的输入操作数
        : "x0", "x1", "x9", "x10", "memory"  // 声明修改的寄存器
    );
}
uint64_t x; // 声明 x 为外部变量
int main() {
    // 直接调用 AddSerializeInstructionToCodePage
    //AddSerializeInstructionToCodePage();

    __asm__ volatile(
            "dsb sy\n"  // 执行数据同步屏障
            "isb \n"

            // 启动性能计数器
            "MOV X9, #1\n"  // 将值1写入 PMCR_EL0，启用性能计数器
            "MSR PMCR_EL0, X9\n"  // 设置 PMCR_EL0，启用计数器

            "MSR PMCCNTR_EL0, XZR\n " // 清空 PMCCNTR_EL0 寄存器
            "MRS X9, PMCCNTR_EL0\n"  // 读取当前的计数值
            "Mov X10, X9\n"  // 保存初始计数值

            // 执行一些代码（模拟一些处理）
            // "dsb sy\n"  // 执行数据同步屏障
            // "isb \n"

            // 再次读取计数器的值
            "MOV X9, #0\n"
            "MRS X9, PMCCNTR_EL0\n"
            "SUB X3, X9, X10\n"  // 计算时间差

            "ldr x0, =x\n"        // 将变量 x 的地址加载到 r0
            "str x3, [x0]\n"      // 将 x0 的值存储到 r0 指向的地址（即 x）
            // 使用寄存器来加载 x 的地址
        );
    // 打印测试信息
    std::cout << "x = " << x << std::endl;

    return 0;
}
