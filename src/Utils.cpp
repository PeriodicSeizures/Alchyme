#include "Utils.h"

namespace Utils {

    std::thread::id main_id = std::this_thread::get_id();

    void initLogger() {
        el::Configurations loggerConfiguration;
        //el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%startTime", std::bind(getTimeSinceProgramStart)));
        //std::string format = "%s [%startTime][%level][%thread][%fbase]: %msg";

        // https://github.com/amrayn/easyloggingpp#datetime-format-specifiers
        // [%fbase:L%line]
        std::string format = "[%datetime{%H:%m:%s.%g}] [%thread thread/%level]: %msg";
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

    std::vector<std::string_view> split(std::string_view s, std::string_view delimiter) {
        //std::string s = "scott>=tiger>=mushroom";

        std::vector<std::string_view> res;

        size_t pos = 0;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            //std::cout << token << std::endl;
            res.push_back(s.substr(0, pos));
            //s.erase(0, pos + delimiter.length());
            s = s.substr(pos + delimiter.length());
        }
        //std::cout << s << std::endl;
        return res;
    }

}