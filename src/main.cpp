#include "config.hpp"
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
#include "help.hpp"
#include "macros.hpp"
#include "vendr.hpp"


int main(const int argc, const char *argv[]) {
    // remove './' and reduce to basename of executable
    argZero = stripArgZero(std::string(argv[0]));

    // default behaviour
    std::string tomlPath = "vendr.toml";
    bool allTargets = true;
    bool overwriteFiles = false;
    std::string namedEntry;
    
    args::ArgumentParser argParser(argZero);

    args::ValueFlag<std::string> argFilename(argParser, "", "", {'f', "file"});
    args::ValueFlag<std::string> argName(argParser, "", "", {'n', "name"});
    args::Flag argOverwrite(argParser, "", "", {'w', "overwrite"});
    args::Flag argVerbose(argParser, "", "", {'v', "verbose"});
    args::Flag argVersion(argParser, "", "", {'V', "version"});
    args::Flag argHelp(argParser, "", "", {'h', "help"});

    try {
        argParser.ParseCLI(argc, argv);
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
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
        vendr::log::verbose = args::get(argVerbose);

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
            vendr::log::err("no entry found by name '{}'", namedEntry);
            return 1;
        }
    }
}
