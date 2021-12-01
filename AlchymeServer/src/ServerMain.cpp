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
    //"C:\Users\Rico\Documents\VisualStudio2019\Projects\Alchyme\AlchymeServer\data\settings.txt"
    //FILE* f = fopen("./data/settings.txt", "rb");

    //namespace fs = std::filesystem;

    //std::cout << fs::current_path() << "\n";

    //if (f) {
    //    std::cout << "file does exist\n";
    //    fclose(f);
    //} else 
    //    std::cout << "file does not exist\n";

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

    /*
    MultiBuffer<64> multi;
    multi.setActiveBlock(6);

    auto str = "My string";

    multi.Write(str); // , strlen(str));

    char buf[16];

    multi.Read(buf, strlen(str) + 1);
    std::cout << "read string: " << buf << "\n";

    return 0;

    std::vector<int> v = { 1, 3, 5, 7, 9, 12, 14, 16, 18, 21, 23, 25, 27,
        29, 32, 34, 36, 38, 41, 43, 45, 47, 49, 52,
        54, 56, 58, 61, 63, 65, 67, 69, 72, 74, 76, 78, 81, 83, 85, 87, 89 };

    try {

        for (auto i : v)
            if (!multi.Write(&i))
                break;
                //throw std::runtime_error("Write error: ");

        int size = multi.offset()/sizeof(int);
        for (auto i = 0; i < size; i++) {
            int o = 0;
            if (!multi.Read<int>(&o))
                break;
                //throw std::runtime_error("Read error: ");
            std::cout << "Read: " << o << std::endl;
        }
    }
    catch (std::exception & e) {
        std::cerr << e.what() << ": " << (int)multi.getError();
    }

    return 0;*/

    std::unordered_map<std::string, std::string> settings;
    if (!loadSettings(settings)) {
        std::cerr << "settings file not found\n";
        return 0;
    }
	Server s(std::move(settings));

	s.StartListening();

	return 0;
}
