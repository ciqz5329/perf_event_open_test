#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <cstdint>

// 声明全局变量 x

// 被调用者保存的寄存器：
//   X19-X28
//   X30 (LR)
//   X29 (FP)
//   V0-V31 (SIMD 寄存器)
//   FPSCR,FPSR (浮点状态寄存器)// void AddProlog()
//// {
////     __asm__ volatile(
////
////         // prolog
////         "stp     x29, x30, [sp, #-16]!"
////         "stp     x19, x20, [sp, #-16]!"
////         "stp     x21, x22, [sp, #-16]!"
////         "stp     x23, x24, [sp, #-16]!"
////         "stp     x25, x26, [sp, #-16]!"
////         "stp     x27, x28, [sp, #-16]!"
////
////         " stp     v0, v1, [sp, #-16]! "
////         " stp     v2, v3, [sp, #-16]! "
////         " stp     v4, v5, [sp, #-16]! "
////         " stp     v6, v7, [sp, #-16]! "
////         " stp     v8, v9, [sp, #-16]! "
////         " stp     v10, v11, [sp, #-16]! "
////         " stp     v12, v13, [sp, #-16]! "
////         " stp     v14, v15, [sp, #-16]! "
////         " stp     v16, v17, [sp, #-16]! "
////         " stp     v18, v19, [sp, #-16]! "
////         " stp     v20, v21, [sp, #-16]! "
////         " stp     v22, v23, [sp, #-16]! "
////         " stp     v24, v25, [sp, #-16]! "
////         " stp     v26, v27, [sp, #-16]! "
////         " stp     v28, v29, [sp, #-16]! "
////         " stp     v30, v31, [sp, #-16]! "
////
////         " vmrs    x0, fpscr "
////         "str     x0, [sp, #-8]! "
////
////         //函数调用
////
////         "dsb sy\n" // 执行数据同步屏障
////         "isb \n"
////
////         // 启动性能计数器
////         "MOV X9, #1\n" // 将值1写入 PMCR_EL0，启用性能计数器
////         "MSR PMCR_EL0, X9\n" // 设置 PMCR_EL0，启用计数器
////
////         "MSR PMCCNTR_EL0, XZR\n " // 清空 PMCCNTR_EL0 寄存器
////         "MRS X9, PMCCNTR_EL0\n" // 读取当前的计数值
////         "Mov X10, X9\n" // 保存初始计数值
////
////         // 执行一些代码（模拟一些处理）
////         "dsb sy\n" // 执行数据同步屏障
////         "isb \n"
////
////         // 再次读取计数器的值
////         "MOV X9, #0\n"
////         "MRS X9, PMCCNTR_EL0\n"
////         "SUB X0, X9, X10\n" // 计算时间差
////
////         // 使用寄存器来加载 x 的地址
////         "ldr x1, =x\n" // 将 x 的地址加载到寄存器 x1 中
////         "str x0, [x1]\n" // 将结果存储到 x 指向的位置
////
////
////         //epilog
////         "ldr     x0, [sp], #8" // 恢复 FPSCR 到 x0
////         "vmsr    fpscr, x0" // 将 FPSCR 恢复
////
////         // 恢复浮点寄存器 v0-v31（按逆序）
////         "ldp     v30, v31, [sp], #16  " // 恢复 v30 和 v31
////         "ldp     v28, v29, [sp], #16   " // 恢复 v28 和 v29
////         "ldp     v26, v27, [sp], #16 " // 恢复 v26 和 v27
////         "ldp     v24, v25, [sp], #16  " // 恢复 v24 和 v25
////         "ldp     v22, v23, [sp], #16 " // 恢复 v22 和 v23
////         "ldp     v20, v21, [sp], #16" // 恢复 v20 和 v21
////         "ldp     v18, v19, [sp], #16  " // 恢复 v18 和 v19
////         "ldp     v16, v17, [sp], #16   " // 恢复 v16 和 v17
////         "ldp     v14, v15, [sp], #16    " // 恢复 v14 和 v15
////         "ldp     v12, v13, [sp], #16  " // 恢复 v12 和 v13
////         "ldp     v10, v11, [sp], #16 " // 恢复 v10 和 v11
////         "ldp     v8, v9, [sp], #16    " // 恢复 v8 和 v9
////         "ldp     v6, v7, [sp], #16   " // 恢复 v6 和 v7
////         "ldp     v4, v5, [sp], #16  " // 恢复 v4 和 v5
////         "ldp     v2, v3, [sp], #16   " // 恢复 v2 和 v3
////         "ldp     v0, v1, [sp], #16 " // 恢复 v0 和 v1
////
////         // 恢复通用寄存器 x19-x28
////         "ldp     x19, x20, [sp], #16    " // 恢复 x19 和 x20
////         "ldp     x21, x22, [sp], #16   " // 恢复 x21 和 x22
////         "ldp     x23, x24, [sp], #16   " // 恢复 x23 和 x24
////         "ldp     x25, x26, [sp], #16  " // 恢复 x25 和 x26
////         "ldp     x27, x28, [sp], #16 " // 恢复 x27 和 x28
////
////         // 恢复帧指针 x29 和链接寄存器 x30
////         "ldp     x29, x30, [sp], #16   " // 恢复 x29 和 x30
////         "ret"
////
////     );
//// }



void AddSerializeInstructionToCodePage()
{
    __asm__ volatile(
        "dsb sy\n" // 执行数据同步屏障
        "isb \n"

        // 启动性能计数器
        "MOV X9, #1\n" // 将值1写入 PMCR_EL0，启用性能计数器
        "MSR PMCR_EL0, X9\n" // 设置 PMCR_EL0，启用计数器

        "MSR PMCCNTR_EL0, XZR\n " // 清空 PMCCNTR_EL0 寄存器
        "MRS X9, PMCCNTR_EL0\n" // 读取当前的计数值
        "Mov X10, X9\n" // 保存初始计数值

        // 执行一些代码（模拟一些处理）
        "dsb sy\n" // 执行数据同步屏障
        "isb \n"

        // 再次读取计数器的值
        "MOV X9, #0\n"
        "MRS X9, PMCCNTR_EL0\n"
        "SUB X0, X9, X10\n" // 计算时间差

        // 使用寄存器来加载 x 的地址
        "ldr x1, =x\n" // 将 x 的地址加载到寄存器 x1 中
        "str x0, [x1]\n" // 将结果存储到 x 指向的位置
        : // 输出操作数部分
        : // 没有显式的输入操作数
        : "x0", "x1", "x9", "x10", "memory" // 声明修改的寄存器
    );
}

uint64_t x; // 声明 x 为外部变量
int main()
{
    // 直接调用 AddSerializeInstructionToCodePage
    //AddSerializeInstructionToCodePage();
 __asm__ volatile(

        // prolog
        "stp     x29, x30, [sp, #-16]!\n"
        "stp     x0, x1, [sp, #-16]!\n"
        "stp     x2, x3, [sp, #-16]!\n"
        "stp     x4, x5, [sp, #-16]!\n"
        "stp     x6, x7, [sp, #-16]!\n"
        "stp     x8, x9, [sp, #-16]!\n"
        "stp     x10, x11, [sp, #-16]!\n"
        "stp     x12, x13, [sp, #-16]!\n"
        "stp     x14, x15, [sp, #-16]!\n"
        "stp     x16, x17, [sp, #-16]!\n"
        "stp     x18, x19, [sp, #-16]!\n"
        "stp     x20, x21, [sp, #-16]!\n"
        "stp     x22, x23, [sp, #-16]!\n"
        "stp     x24, x25, [sp, #-16]!\n"
        "stp     x26, x27, [sp, #-16]!\n"
        "str     x28, [sp, #-8]!\n"
        "sub sp, sp, #8\n" // 为了保持 16 字节对齐

        "str q0  , [sp, #-16]!\n"
        "str q1  , [sp, #-16]!\n"
        "str q2  , [sp, #-16]!\n"
        "str q3  , [sp, #-16]!\n"
        "str q4  , [sp, #-16]!\n"
        "str q5  , [sp, #-16]!\n"
        "str q6  , [sp, #-16]!\n"
        "str q7  , [sp, #-16]!\n"
        "str q8  , [sp, #-16]!\n"
        "str q9  , [sp, #-16]!\n"
        "str q10  , [sp, #-16]!\n"
        "str q11  , [sp, #-16]!\n"
        "str q12  , [sp, #-16]!\n"
        "str q13  , [sp, #-16]!\n"
        "str q14  , [sp, #-16]!\n"
        "str q15  , [sp, #-16]!\n"
        "str q16  , [sp, #-16]!\n"
        "str q17  , [sp, #-16]!\n"
        "str q18  , [sp, #-16]!\n"
        "str q19  , [sp, #-16]!\n"
        "str q20  , [sp, #-16]!\n"
        "str q21  , [sp, #-16]!\n"
        "str q22  , [sp, #-16]!\n"
        "str q23  , [sp, #-16]!\n"
        "str q24  , [sp, #-16]!\n"
        "str q25  , [sp, #-16]!\n"
        "str q26  , [sp, #-16]!\n"
        "str q27  , [sp, #-16]!\n"
        "str q28  , [sp, #-16]!\n"
        "str q29  , [sp, #-16]!\n"
        "str q30  , [sp, #-16]!\n"
        "str q31  , [sp, #-16]!\n"
        //
        "mrs x0, fpsr\n"
        "str x0, [sp, #-16]!\n"
        "mrs x0, FPCR\n"
        "str x0, [sp, #-16]!\n"

        // //
        // //函数调用
        //
        "dsb sy\n" // 执行数据同步屏障
        "isb \n"

        // 启动性能计数器
        "MOV X9, #1\n" // 将值1写入 PMCR_EL0，启用性能计数器
        "MSR PMCR_EL0, X9\n" // 设置 PMCR_EL0，启用计数器

        "MSR PMCCNTR_EL0, XZR\n " // 清空 PMCCNTR_EL0 寄存器
        "MRS X9, PMCCNTR_EL0\n" // 读取当前的计数值
        "Mov X10, X9\n" // 保存初始计数值

        // 执行一些代码（模拟一些处理）
        "dsb sy\n" // 执行数据同步屏障
        "isb \n"

        // 再次读取计数器的值
        "MOV X9, #0\n"
        "MRS X9, PMCCNTR_EL0\n"
        "SUB X0, X9, X10\n" // 计算时间差

        // 使用寄存器来加载 x 的地址
        "ldr x1, =x\n" // 将 x 的地址加载到寄存器 x1 中
        "str x0, [x1]\n" // 将结果存储到 x 指向的位置
        //
        //
        // //epilog
        //
        "ldr x0, [sp], #16\n"   // 恢复 fpcr
        " msr fpcr, x0\n"       // 将 x0 中的值写回 fpcr

        "ldr x0, [sp], #16\n"       // 恢复 fpsr
        "msr fpsr, x0\n"       // 将 x0 中的值写回 fpsr

        // // 恢复浮点寄存器 v0-v31（按逆序）
        "ldr    q31, [sp], #16 \n "
        "ldr    q30, [sp], #16 \n "
        "ldr    q29, [sp], #16 \n "
        "ldr    q28, [sp], #16 \n "
        "ldr    q27, [sp], #16 \n "
        "ldr    q26, [sp], #16 \n "
        "ldr    q25, [sp], #16 \n "
        "ldr    q24, [sp], #16 \n "
        "ldr    q23, [sp], #16 \n "
        "ldr    q22, [sp], #16 \n "
        "ldr    q21, [sp], #16 \n "
        "ldr    q20, [sp], #16 \n "
        "ldr    q19, [sp], #16 \n "
        "ldr    q18, [sp], #16 \n "
        "ldr    q17, [sp], #16 \n "
        "ldr    q16, [sp], #16 \n "
        "ldr    q15, [sp], #16 \n "
        "ldr    q14, [sp], #16 \n "
        "ldr    q13, [sp], #16 \n "
        "ldr    q12, [sp], #16 \n "
        "ldr    q11, [sp], #16 \n "
        "ldr    q10, [sp], #16 \n "
        "ldr    q9, [sp], #16 \n "
        "ldr    q8, [sp], #16 \n "
        "ldr    q7, [sp], #16 \n "
        "ldr    q6, [sp], #16 \n "
        "ldr    q5, [sp], #16 \n "
        "ldr    q4, [sp], #16 \n "
        "ldr    q3, [sp], #16 \n "
        "ldr    q2, [sp], #16 \n "
        "ldr    q1, [sp], #16 \n "
        "ldr    q0, [sp], #16 \n "



        //
        // // 恢复通用寄存器 x19-x28

        "ldr     x28, [sp], #8\n"
        "sub sp, sp, #-8\n" // 为了保持 16 字节对齐
        "ldp     x26, x27, [sp], #16\n"
        "ldp     x24, x25, [sp], #16\n"
        "ldp     x22, x23, [sp], #16\n"
        "ldp     x20, x21, [sp], #16\n"
        "ldp     x18, x19, [sp], #16\n"
        "ldp     x16, x17, [sp], #16\n"
        "ldp     x14, x15, [sp], #16\n"
        "ldp     x12, x13, [sp], #16\n"
        "ldp     x10, x11, [sp], #16\n"
        "ldp     x8, x9, [sp], #16\n"
        "ldp     x6, x7, [sp], #16\n"
        "ldp     x4, x5, [sp], #16\n"
        "ldp     x2, x3, [sp], #16\n"
        "ldp     x0, x1, [sp], #16\n"

        // 恢复帧指针 x29 和链接寄存器 x30
        "ldp     x29, x30, [sp], #16\n "
        //"ret"

    );

    // 打印测试信息
    std::cout << "x = " << x << std::endl;

    return 0;
}
