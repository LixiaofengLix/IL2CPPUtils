@echo off

rem 设置变量
set SOURCE_DIR=.
set BUILD_DIR=build
set EXECUTABLE_NAME=Il2cppUtils.exe

rem 创建构建目录
if not exist "%BUILD_DIR%" mkdir %BUILD_DIR%

rem 运行 CMake 生成项目文件
cd %BUILD_DIR%
cmake ..
cmake --build .

rem 复制可执行文件到根目录
copy /Y Debug\%EXECUTABLE_NAME% ..

rem 返回到源目录
cd ..

echo Build and copy completed.

@REM echo Run %EXECUTABLE_NAME%
@REM %EXECUTABLE_NAME%
@REM pause