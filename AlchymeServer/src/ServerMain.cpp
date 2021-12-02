// AlchymeServer
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include "Server.hpp"
//#include "FastBuffer.hpp"

INITIALIZE_EASYLOGGINGPP

void initLogger() {
    el::Configurations loggerConfiguration;
    //el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%startTime", std::bind(getTimeSinceProgramStart)));
    //std::string format = "%s [%startTime][%level][%thread][%fbase]: %msg";

    // https://github.com/amrayn/easyloggingpp#datetime-format-specifiers
    // [%fbase:L%line]
    std::string format = "[%datetime{%H:%m:%s}] [%thread thread/%level]: %msg";
    loggerConfiguration.set(el::Level::Info, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Error, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Fatal, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Warning, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Debug, el::ConfigurationType::Format, format);
    el::Helpers::setThreadName("main");
    el::Loggers::reconfigureAllLoggers(loggerConfiguration);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    //el::Loggers::file
    LOG(INFO) << "Logger is configured";
}

bool loadSettings(std::unordered_map<std::string, std::string>& settings) {
    std::ifstream file;

    file.open("C:\\Users\\Rico\\Documents\\VisualStudio2019\\Projects\\Alchyme\\AlchymeServer\\data\\settings.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t index = line.find(':');

            std::string key = line.substr(0, index);
            std::string value = line.substr(index + 2);
            settings.insert({ key, value });
        }

        file.close();
        return true;
    }
    else {
        return false;
        // else, generate things from scratch
    }
}

int main() {
    initLogger();

    std::unordered_map<std::string, std::string> settings;
    if (!loadSettings(settings)) {
        std::cerr << "settings file not found\n";
        return 0;
    }
	Server s;

	s.Run();

	return 0;
}
