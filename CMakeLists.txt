cmake_minimum_required(VERSION 3.10)
project(perf_event_open_test)

# 设置 C 和 C++ 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# 指定目标系统和架构
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 设置 Clang 编译器
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O0 -g -fno-inline -fno-builtin -fno-vectorize -fno-slp-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g -fno-inline -fno-builtin -fno-vectorize -fno-slp-vectorize")

# 启用汇编语言支持
enable_language(ASM)

# 添加汇编文件
set(SOURCES
        src/main.cpp
        src/ic_iallu_trigger.s  # 确保路径和文件名正确
)

# 显式指定汇编文件的编译规则
set_source_files_properties(src/ic_iallu_trigger.s PROPERTIES LANGUAGE ASM)

# 添加可执行文件
add_executable(perf_event_open_test ${SOURCES})

# 设置目标属性
target_compile_options(perf_event_open_test PRIVATE
        -fno-omit-frame-pointer  # 保留帧指针，方便调试
        -fstack-protector-strong # 启用栈保护
)

# 添加链接选项
target_link_options(perf_event_open_test PRIVATE
        -rdynamic  # 支持动态符号表，方便调试
)