# ARM-v8 L1 Cache测试

## Perf-open接口-code

```c++
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
#include <iostream>
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
    struct perf_event_attr attr_cache_ref = create_perf_event_attr(PERF_TYPE_HW_CACHE,
        PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16));
    struct perf_event_attr attr_cache_miss = create_perf_event_attr(PERF_TYPE_HW_CACHE,
        PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16));
    // 打开性能计数器
    int fd_cache_ref = syscall(__NR_perf_event_open, &attr_cache_ref, 0, -1, -1, 0);
    int fd_cache_miss = syscall(__NR_perf_event_open, &attr_cache_miss, 0, -1, -1, 0);
    int fd_cycles = syscall(__NR_perf_event_open, &attr_cycles, 0, -1, -1, 0);

    if (fd_cache_ref == -1 || fd_cache_miss == -1 || fd_cycles == -1) {
        perror("perf_event_open");
        return -1;
    }
    //executor.init_fd(fd);  // 初始化性能计数器
    uint64_t sum = 0;
    // 多次测量
    int x = 1;
    uint64_t cache_ref = 0, cache_miss = 0, cycles = 0;
    //初始化性能计数器
    ioctl(fd_cache_miss, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);

    for (int i = 0; i < 10000; i++) {

        // 模拟 L1 Cache 访问
        asm volatile (
            "ldr w0, %0\n"  // 加载 x 的值到寄存器 w0
            :               // 无输出
            : "m" (x)       // 将 x 的地址传递给汇编代码
            : "w0"
        );

        //sum+=cycles;
    }

    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_DISABLE, 0);
    read(fd_cycles, &cycles, sizeof(cycles));
    read(fd_cache_ref, &cache_ref, sizeof(cache_ref));
    read(fd_cache_miss, &cache_miss, sizeof(cache_miss));
    std::cout << "Time measured: " << cycles << " CPU cycles" << std::endl;
    printf("L1 Cache ref: %" PRIu64 "\n", cache_ref);
    printf("L1 Cache misses: %" PRIu64 "\n", cache_miss);
    double avg_access_time = (double)cycles / cache_ref;
    double miss_rate = (double)cache_miss / cache_ref * 100;
    //ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    printf("L1 Cache miss rate: %.2f%%\n", miss_rate);
    printf("Average L1 Cache access time: %.2f cycles\n", avg_access_time);


    // 关闭性能计数器
    //close(fd_cycles);
    close(fd_cache_ref);
    close(fd_cache_miss);
    close(fd_cycles);
    return 0;
}
```

### 结果

```shell
root@acmachines:/home/acmachines/osiris/perf_event_open_test/build# ./perf_event_open_test 
Time measured: 80182 CPU cycles
L1 Cache ref: 40030
L1 Cache misses: 0
L1 Cache miss rate: 0.00%
Average L1 Cache access time: 2.00 cycles
```

