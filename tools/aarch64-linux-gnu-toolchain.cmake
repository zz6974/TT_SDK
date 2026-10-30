# aarch64-linux-gnu-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 指定交叉编译器前缀
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

# 设置目标环境根路径 (请替换为你实际的 sysroot 路径)
set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu")
# 或者指定具体的交叉编译工具链目录，例如：
# set(CMAKE_FIND_ROOT_PATH "/opt/gcc-arm-11.2-aarch64-none-linux-gnu")

# 搜索模式设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 清除可能导致冲突的链接器标志
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS "" CACHE STRING "" FORCE)