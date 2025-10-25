// SPDX-FileCopyrightText: 2025 Will Reed
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "i18n.hpp"
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
    inline bool useColor = hasEnv("NO_COLOR") ? false : true;
    inline bool verbose = false;
    
    std::string color(const std::string& num) {
        if (!vendr::useColor)
            return std::string("");
        else
            return std::format("\033[{}m", num);
    }
    
namespace log {

    template <typename... Args>
    void err (std::string_view msg, Args&&... fmt) {
	    std::string fmtMsg = std::vformat(_(msg.data()), std::make_format_args(fmt...));
	    std::cerr
		<< vendr::color("31")
  	    << argZero
        << vendr::color("0")
        << ": "
  	    << fmtMsg
  	    << std::endl;
    }

    template <typename... Args>
    void info (std::string_view msg, Args&&... fmt) {
	    std::string fmtMsg = std::vformat(_(msg.data()), std::make_format_args(fmt...));
	    std::cout
        << vendr::color("34")
  	    << argZero
        << vendr::color("0")
        << ": "
  	    << fmtMsg
  	    << std::endl;
    }

    template <typename... Args>
    void warn (std::string_view msg, Args&&... fmt) {
        if (!vendr::verbose) return;
	    std::string fmtMsg = std::vformat(_(msg.data()), std::make_format_args(fmt...));
	    std::cout
        << vendr::color("33")
        << argZero
        << vendr::color("0")
        << ": "
        << fmtMsg
        << std::endl;
    }

    template <typename... Args>
    [[noreturn]] void fatal(int exitCode, std::string_view msg, Args&&... fmt) {
        std::string fmtMsg = std::vformat(_(msg.data()), std::make_format_args(fmt...));
        std::cerr
        << vendr::color("31")
        << argZero
        << vendr::color("0")
        << _(" (fatal): ")
        << fmtMsg
        << std::endl;
        std::exit(exitCode);
    }
} // end namespace log
namespace usage {
    void manual() {
        int ecode = std::system("man 1 vendr");
        if (ecode != 0) {
            vendr::log::err("command returned exit code {}", ecode);
            std::exit(ecode);
        }
    }
    
    void help() {
        vendr::log::info("options");
        std::cout << _("  -f,--file <path>\tspecify file path") << "\n";
        std::cout << _("  -n,--name <name>\tspecify entry name") << "\n";
        std::cout << _("  -w,--overwrite\toverwrite existing files") << "\n";
        std::cout << _("  -v,--verbose\t\tproduce more output for operations") << "\n";
        std::cout << _("  -h,--help\t\tdisplay usage info") << "\n";
        std::cout << _("  -m,--manual\t\topen the manual page for vendr") << "\n";
        std::cout << _("  -V,--version\t\tdisplay version info") << "\n";
    }

    void version() {
        vendr::log::info("version {}", projectVersion);
    }
} // end namespace usage

} // end namespace vendr
