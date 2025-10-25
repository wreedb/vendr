// SPDX-FileCopyrightText: 2025 Will Reed
// SPDX-License-Identifier: GPL-3.0-or-later

// meson configure_file()
#include "config.hpp"

#ifndef LOCALEDIR
    #define LOCALEDIR "/usr/share/locale"
#endif

#include "i18n.hpp"

#include <format>
#include <iostream>
#include <optional>

#if TOML_HEADER_ONLY
    #include "toml.hpp"
#else
    #include <toml++/toml.hpp>
#endif

#if SYSTEM_ARGSHXX
    #include <args.hxx>
#else
    #include "args.hxx"
#endif

static const std::string projectVersion = std::string(PROJECT_VERSION);
std::string argZero = "vendr";

// project-local headers
#include "macros.hpp"
#include "help.hpp"
#include "vendr.hpp"

int main(const int argc, const char *argv[]) {
    // locale initialization
    std::locale::global(std::locale(""));
    bindtextdomain("vendr", LOCALEDIR);
    textdomain("vendr");
    
    // remove './' and reduce to basename of executable
    argZero = stripArgZero(std::string(argv[0]));

    std::string tomlPath = "vendr.toml"; // default file path
    bool allTargets = true;              // default to all targets
    bool overwriteFiles = false;         // default to not overwriting files
    std::string namedEntry;              // storage for '-n/--name' argument
    
    args::ArgumentParser argParser(argZero);

    args::ValueFlag<std::string> argFilename(
        argParser,
        "file",
        "specify file path",
        {'f', "file"}
    );
    
    args::ValueFlag<std::string> argName(
        argParser,
        "name",
        "specify an entry name",
        {'n', "name"}
    );
    
    args::Flag argOverwrite(
        argParser,
        "overwrite",
        "overwrite existing files",
        {'w', "overwrite"}
    );
    
    args::Flag argVerbose(
        argParser,
        "",
        "",
        {'v', "verbose"}
    );
    
    args::Flag argVersion(
        argParser,
        "version",
        "display version info",
        {'V', "version"}
    );
    
    args::Flag argHelp(
        argParser,
        "help",
        "display usage info",
        {'h', "help"}
    );
    
    args::Flag argManual(
        argParser,
        "manual",
        "open the manual page for vendr",
        {'m', "manual"}
    );

    try {
        argParser.ParseCLI(argc, argv);
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (argManual) {
        vendr::usage::manual();
        return 0;
    }
    
    if (argVersion) {
        vendr::usage::version();
        return 0;
    }

    if (argHelp) {
        vendr::usage::help();
        return 0;
    }
    
    if (argVerbose)
        vendr::verbose = args::get(argVerbose);

    if (argFilename)
        tomlPath = args::get(argFilename);

    if (argName) {
        allTargets = false;
        namedEntry = args::get(argName);
    }

    if (argOverwrite)
        overwriteFiles = true;
    
    // END: argument parsing

    // BEGIN: main operation
    vendr::targets vendrTargets = vendr::serializeToml(tomlPath);
    
    if (allTargets) {
        int filesCount = vendrTargets.files.size();
        int reposCount = vendrTargets.repos.size();
        
        if (filesCount > 0)
            for (int n = 0; n < filesCount; n++)
                vendr::get(vendrTargets.files.at(n), overwriteFiles);

        if (reposCount > 0)
            for (int n = 0; n < reposCount; n++)
                vendr::fetch(vendrTargets.repos.at(n), overwriteFiles);

        return 0;

    // --name <name> passed
    } else if (!namedEntry.empty()) {
        std::optional<vendr::repo> uRepo = vendr::findRepoByName(vendrTargets.repos, namedEntry);
        std::optional<vendr::file> uFile = vendr::findFileByName(vendrTargets.files, namedEntry);
        if (uFile) {
            vendr::get(uFile.value(), overwriteFiles);
            return 0;
        } else if (uRepo) {
            vendr::fetch(uRepo.value(), overwriteFiles);
        } else {
            vendr::log::err(_("no entry found by name '{}'"), namedEntry);
            return 1;
        }
    }
}
