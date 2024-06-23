//libil2cpp/utils/FileLog.hpp
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

#if defined(_MSC_VER)
#include <Windows.h>
#define DEBUG_PRINT(szFormat, ...) do { \
    char buffer[256]; \
    sprintf_s(buffer, 256, szFormat, __VA_ARGS__); \
    OutputDebugStringA(buffer); \
} while (0)
#else
#define DEBUG_PRINT(szFormat, ...) 
#endif

class FileLog {
public:
    static FileLog& Out() {
        static FileLog output("out.txt");
        return output;
    }

    void log(const std::string& message) {
        std::stringstream logStream;
        logStream << message << std::endl;
        logFile << logStream.str();
        logFile.flush();
    }

    void log(const char* message) {
        std::stringstream logStream;
        logStream << message << std::endl;
        logFile << logStream.str();
        logFile.flush();
    }

    void fmt(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char buffer[1024]{ 0 };
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        log(buffer);
    }

    FileLog(const FileLog&) = delete;
    FileLog& operator=(const FileLog&) = delete;

private:
    FileLog(const char *filepath) {
        logFile.open(filepath, std::ios::out | std::ios::trunc);
    }

    ~FileLog() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    std::ofstream logFile;
};