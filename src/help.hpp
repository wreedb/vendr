#pragma once

#include <cstdlib>
#include <string>
#include <print>

bool hasEnv(const std::string& key) {
    char* value = std::getenv(key.c_str());
    return value == nullptr ? false : true;
}

std::string getEnv(const std::string& key) {
    char* value = std::getenv(key.c_str());
    return value == nullptr ? std::string("") : std::string(value);
}

std::string stripArgZero(const std::string& arg) {
    std::string argz = arg;
    // remove './' from beginning of string
    if (argz.size() >= 2 && argz.substr(0, 2) == "./")
        argz = argz.substr(2);
    // like 'basename'
    size_t last_slash = argz.find_last_of("/");
    if (last_slash != std::string::npos)
        argz = argz.substr(last_slash + 1);
    return argz;
}

namespace vendr {
namespace log {
    inline bool useColor = hasEnv("NO_COLOR") ? false : true;
    inline bool verbose = false;

    template <typename... Args>
    void err (std::format_string<Args...> fmt, Args&&... args) {
	   std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
	   std::cerr
       << (useColor ? "\033[31m" : "")
  	   << argZero
  	   << (useColor ? "\033[0m"  : "")
       << ": "
  	   << fmtMsg
  	   << std::endl;
    }

    template <typename... Args>
    void info (std::format_string<Args...> fmt, Args&&... args) {
	   std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
	   std::cout
       << (useColor ? "\033[32m" : "")
  	   << argZero
  	   << (useColor ? "\033[0m" : "")
       << ": "
  	   << fmtMsg
  	   << std::endl;
    }

    template <typename... Args>
    void warn (std::format_string<Args...> fmt, Args&&... args) {
        if (!verbose) return;
	    std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
	    std::cout
        << (useColor ? "\033[33m" : "")
        << argZero
        << (useColor ? "\033[0m" : "")
        << ": "
        << fmtMsg
        << std::endl;
    }

    template <typename... Args>
    [[noreturn]] void fatal(int exitCode, std::format_string<Args...> fmt, Args&&... args) {
        std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
        std::cerr
        << (useColor ? "\033[1;31m" : "")
        << argZero
        << (useColor ? "\033[0m"    : "")
        << " (fatal): "
        << fmtMsg
        << std::endl;
        std::exit(exitCode);
    }
}
namespace usage {
    void manual() {
        int ecode = std::system("man 1 vendr");
        if (ecode != 0) {
            std::println("command returned exit code {}", ecode);
            std::exit(ecode);
        }
    }
    
    void help() {
        //using vendr::log::vendr::log::useColor;
        std::println("{}{}{} options:", (vendr::log::useColor ? "\033[34m" : ""), argZero, (vendr::log::useColor ? "\033[0m" : ""));
        std::println("  -f,--file <path>\tspecify file path");
        std::println("  -n,--name <name>\tspecify entry name");
        std::println("  -w,--overwrite\toverwrite existing files");
        std::println("  -v,--verbose\t\tproduce more output for operations");
        std::println("  -h,--help\t\tdisplay usage info");
        std::println("  -m,--manual\t\topen the manual page for vendr");
        std::println("  -V,--version\t\tdisplay version info");
    }

    void version() {
        // using vendr::log::vendr::log::useColor;
        std::println("{}{}{}: version {}", (vendr::log::useColor ? "\033[34m" : ""), argZero, (vendr::log::useColor ? "\033[0m" : ""), projectVersion);
    }
}
}
