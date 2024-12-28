# linux perf-open测试

## V1 code

```c++
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>  // 这个头文件更为合适
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>



class Executor {
public:
    // 手动封装 perf_event_open 系统调用
    int perf_event_open(struct perf_event_attr *attr, pid_t pid, int cpu, int group_fd, unsigned long flags)
    {
        return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
    }

    // 打开性能计数器并进行初始设置
    int openPerfCounter() {
        struct perf_event_attr attr;
        memset(&attr, 0, sizeof(attr));
        attr.type = PERF_TYPE_HARDWARE;            // 硬件性能事件
        attr.config = PERF_COUNT_HW_CPU_CYCLES;    // CPU 时钟周期
        attr.size = sizeof(attr);
        attr.disabled = 1;                          // 初始禁用
        attr.exclude_kernel = 1;                    // 排除内核事件
        attr.exclude_hv = 1;                        // 排除虚拟化事件

        int fd = perf_event_open(&attr, 0, -1, -1, 0); // 打开性能计数器，监控当前进程
        if (fd == -1) {
            perror("perf_event_open");
            return -1;
        }

        return fd;
    }

    // 读取性能计数器值
    long long readPerfCounter(int fd) {
        long long count;
        if (read(fd, &count, sizeof(count)) == -1) {
            perror("read");
            return -1;
        }
        return count;
    }

    // 初始化性能计数器（启动计数器）
    void init_fd(int fd) {
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);   // 启动计数器
    }

    // 关闭性能计数器
    void close_fd(int fd) {
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);   // 停止计数器
    }



};

int main() {
    Executor executor;

    // 打开性能计数器
    int fd = executor.openPerfCounter();
    if (fd == -1) return -1;

    executor.init_fd(fd);  // 初始化性能计数器
    int sum = 0;
    // 多次测量
    int x = 1;
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    long long start_c = executor.readPerfCounter(fd);  // 读取开始时间
    for (int i = 0; i < 10000; i++) {
        //if (start_c == -1) return -1;
        // 执行操作
        // 模拟 L1 Cache 访问
            asm volatile (
                "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
                :               // 无输出
                : "m" (x)       // 将 x 的地址传递给汇编代码
                : "w0"
            );
    }
    long long end_c = executor.readPerfCounter(fd);  // 读取结束时间
    //if (end_c == -1) return -1;

    long long time_diff = end_c - start_c;
    //std::cout << "Time measured: " << time_diff << " CPU cycles" << std::endl;
    //sum += time_diff;
    std::cout << "Time measured: " << time_diff/10000 << " CPU cycles" << std::endl;

    // 关闭性能计数器
    executor.close_fd(fd);
    return 0;
}

```

### result

```shell
root@acmachines:/home/acmachines/osiris/perf_event_open_test/build# ./perf_event_open_test 
Time measured: 6 CPU cycles

```

## V2 code

```c++
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>  // 这个头文件更为合适
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>



class Executor {
public:
    // 手动封装 perf_event_open 系统调用
    int perf_event_open(struct perf_event_attr *attr, pid_t pid, int cpu, int group_fd, unsigned long flags)
    {
        return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
    }

    // 打开性能计数器并进行初始设置
    int openPerfCounter() {
        struct perf_event_attr attr;
        memset(&attr, 0, sizeof(attr));
        attr.type = PERF_TYPE_HARDWARE;            // 硬件性能事件
        attr.config = PERF_COUNT_HW_CPU_CYCLES;    // CPU 时钟周期
        attr.size = sizeof(attr);
        attr.disabled = 1;                          // 初始禁用
        attr.exclude_kernel = 1;                    // 排除内核事件
        attr.exclude_hv = 1;                        // 排除虚拟化事件

        int fd = perf_event_open(&attr, 0, -1, -1, 0); // 打开性能计数器，监控当前进程
        if (fd == -1) {
            perror("perf_event_open");
            return -1;
        }

        return fd;
    }

    // 读取性能计数器值
    long long readPerfCounter(int fd) {
        long long count;
        if (read(fd, &count, sizeof(count)) == -1) {
            perror("read");
            return -1;
        }
        return count;
    }

    // 初始化性能计数器（启动计数器）
    void init_fd(int fd) {
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);   // 启动计数器
    }

    // 关闭性能计数器
    void close_fd(int fd) {
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);   // 停止计数器
    }



};

int main() {
    Executor executor;

    // 打开性能计数器
    int fd = executor.openPerfCounter();
    if (fd == -1) return -1;

    executor.init_fd(fd);  // 初始化性能计数器
    int sum = 0;
    // 多次测量
    int x = 1;
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    long long start_c = executor.readPerfCounter(fd);  // 读取开始时间
    for (int i = 0; i < 10000; i++) {
        //if (start_c == -1) return -1;
        // 执行操作
        // 模拟 L1 Cache 访问
            asm volatile (
                "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
                :               // 无输出
                : "m" (x)       // 将 x 的地址传递给汇编代码
                : "w0"
            );
    }
    long long end_c = executor.readPerfCounter(fd);  // 读取结束时间
    //if (end_c == -1) return -1;

    long long time_diff = end_c - start_c;
    //std::cout << "Time measured: " << time_diff << " CPU cycles" << std::endl;
    //sum += time_diff;
    std::cout << "Time measured: " << time_diff/10000 << " CPU cycles" << std::endl;

    // 关闭性能计数器
    executor.close_fd(fd);
    return 0;
}

```



### result

```shell
root@acmachines:/home/acmachines/osiris/perf_event_open_test/build# ./perf_event_open_test 
Time measured: 7 CPU cycles
```



## v3 code

```c++
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>  // 这个头文件更为合适
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>


#define PERF_EVENT_ATTR_SIZE (sizeof(struct perf_event_attr))
struct perf_event_attr create_perf_event_attr(int type, int config) {
    struct perf_event_attr attr;
    memset(&attr, 0, PERF_EVENT_ATTR_SIZE);
    attr.type = type;
    attr.size = PERF_EVENT_ATTR_SIZE;
    attr.config = config;
    attr.disabled = 1;          // 默认禁用
    attr.exclude_kernel = 1;    // 排除内核
    attr.exclude_hv = 1;        // 排除 hypervisor
    return attr;
}

int main() {
    struct perf_event_attr attr_cycles = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    // 打开性能计数器
    int fd_cycles = syscall(__NR_perf_event_open, &attr_cycles, 0, -1, -1, 0);
    if (fd_cycles == -1) {
        perror("perf_event_open");
        return -1;
    }
    //executor.init_fd(fd);  // 初始化性能计数器
    uint64_t sum = 0;
    // 多次测量
    int x = 1;
    uint64_t   cycles;
    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);
    for (int i = 0; i < 10000; i++) {

        ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);
        // 模拟 L1 Cache 访问
            asm volatile (
                "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
                :               // 无输出
                : "m" (x)       // 将 x 的地址传递给汇编代码
                : "w0"
            );
        read(fd_cycles, &cycles, sizeof(cycles));
        std::cout << "Time measured: " << cycles << " CPU cycles" << std::endl;
        sum+=cycles;
    }

    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    std::cout << "Time measured: " << sum/10000 << " CPU cycles" << std::endl;

    // 关闭性能计数器
    close(fd_cycles);
    return 0;
}

```

### result

```shell
Time measured: 131 CPU cycles

```

## v3 code

```cpp
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>  // 这个头文件更为合适
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>


#define PERF_EVENT_ATTR_SIZE (sizeof(struct perf_event_attr))
struct perf_event_attr create_perf_event_attr(int type, int config) {
    struct perf_event_attr attr;
    memset(&attr, 0, PERF_EVENT_ATTR_SIZE);
    attr.type = type;
    attr.size = PERF_EVENT_ATTR_SIZE;
    attr.config = config;
    attr.disabled = 1;          // 默认禁用
    attr.exclude_kernel = 1;    // 排除内核
    attr.exclude_hv = 1;        // 排除 hypervisor
    return attr;
}

int main() {
    struct perf_event_attr attr_cycles = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    // 打开性能计数器
    int fd_cycles = syscall(__NR_perf_event_open, &attr_cycles, 0, -1, -1, 0);
    if (fd_cycles == -1) {
        perror("perf_event_open");
        return -1;
    }
    //executor.init_fd(fd);  // 初始化性能计数器
    uint64_t sum = 0;
    // 多次测量
    int x = 1;
    uint64_t   cycles;

    for (int i = 0; i < 10000; i++) {
        ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);
        ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);
        // 模拟 L1 Cache 访问
            asm volatile (
                "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
                :               // 无输出
                : "m" (x)       // 将 x 的地址传递给汇编代码
                : "w0"
            );
        ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
        read(fd_cycles, &cycles, sizeof(cycles));
        std::cout << "Time measured: " << cycles << " CPU cycles" << std::endl;
        sum+=cycles;
    }

    //ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    std::cout << "Time measured: " << sum/10000 << " CPU cycles" << std::endl;

    // 关闭性能计数器
    close(fd_cycles);
    return 0;
}

```

### result

```shell
Time measured: 116 CPU cycles

```

## v4 code

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/syscall.h>

#define PERF_EVENT_ATTR_SIZE (sizeof(struct perf_event_attr))

// 创建 perf_event_attr 结构
struct perf_event_attr create_perf_event_attr(int type, int config) {
    struct perf_event_attr attr;
    memset(&attr, 0, PERF_EVENT_ATTR_SIZE);
    attr.type = type;
    attr.size = PERF_EVENT_ATTR_SIZE;
    attr.config = config;
    attr.disabled = 1;          // 默认禁用
    attr.exclude_kernel = 1;    // 排除内核
    attr.exclude_hv = 1;        // 排除 hypervisor
    return attr;
}

int main() {
    // 初始化性能事件：L1 Cache 访问、L1 Cache 未命中、CPU 周期
    struct perf_event_attr attr_cycles = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    // 打开性能事件

    int fd_cycles = syscall(__NR_perf_event_open, &attr_cycles, 0, -1, -1, 0);

    if ( fd_cycles == -1) {
        perror("perf_event_open");
        return -1;
    }

    // 启用性能计数器
    int x = 42; // 局部变量
    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);

    // 模拟 L1 Cache 访问
    for (volatile int i = 0; i < 1000000; i++) {
        asm volatile (
            "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
            :               // 无输出
            : "m" (x)       // 将 x 的地址传递给汇编代码
            : "w0"
        );
    }
    // 停止性能计数器
    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    // 读取性能计数器值
    uint64_t  cycles = 0;
    read(fd_cycles, &cycles, sizeof(cycles));
    // 计算 L1 Cache 平均访问时间
    printf("CPU cycles: %" PRIu64 "\n", cycles/1000000);
    // 关闭性能事件
    close(fd_cycles);

    return 0;
}

```

### code

```shell
root@acmachines:/home/acmachines/osiris/perf_event_open_test/build# ./perf_event_open_test 
Time measured: 80182 CPU cycles
L1 Cache misses: 0
L1 Cache miss rate: 0.00%
Average L1 Cache access time: 8.02 cycles

```

