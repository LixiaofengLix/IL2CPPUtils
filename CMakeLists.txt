cmake_minimum_required(VERSION 3.5)
project(Il2cppUtils)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

# 添加可执行文件
add_executable(Il2cppUtils Main.cpp Il2CppMetadataUtils.cpp FileLog.hpp)

# 如果您使用的是 C++17 及以上，将上面的行替换为以下内容
# set(CMAKE_CXX_STANDARD 17)

# 如果您需要链接其他库，可以使用以下命令添加链接
# target_link_libraries(BinaryFileLoading your_library_name)