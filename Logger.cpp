#include "Logger.h"
#include <iostream>
#include <fstream>
#include <filesystem>

void Logger::Log(const std::string& filename, const std::string& message) {
    std::filesystem::path logDirPath("Logs");
    try {
        if (!std::filesystem::exists(logDirPath)) {
            std::filesystem::create_directories(logDirPath);
        }

        std::filesystem::path logFilePath = logDirPath / (filename + ".log");

        std::ofstream logFile(logFilePath, std::ios::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            logFile.close();
        }
        else {
            std::cerr << "Error opening log file: " << logFilePath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating log directory: " << logDirPath << ". " << e.what() << std::endl;
    }
}
