#include <format>
#include <iostream>
#include <optional>

#include "config.hpp"

#if TOML_HEADER_ONLY
    #include "toml.hpp"
#else
    #include <toml++/toml.hpp>
#endif

#include <boost/program_options.hpp>

#include "vendr.hpp"

namespace bp = boost::program_options;

int main(int argc, char *argv[]) {
    
    if (argc > 0) {
        argZero = argv[0];

        if (argZero.size() >= 2 && argZero.substr(0, 2) == "./")
            argZero = argZero.substr(2);

        argZero = argZero.substr(argZero.find_last_of("/") + 1);
    }

    std::string tomlPath = "vendr.toml";
    bool allRepos = true;
    std::string namedRepo;
    
    try {
        bp::options_description optDesc("options");
        optDesc.add_options()
            ("help,h",                                  "display this usage info")
            ("file,f",       bp::value<std::string>(),  "specify vendr.toml path")
            ("name,n",       bp::value<std::string>(),  "fetch only repository <name>")
            ("version,v",                               "show version information");

        bp::variables_map optMap;
        bp::store(bp::parse_command_line(argc, argv, optDesc), optMap);
       
        if (optMap.count("version")) {
            std::cout << "version 0.0.1" << std::endl;
            return 0;
        }

        if (optMap.count("help")) {
            std::cout << "\033[33m" << argZero << "\033[0m ";
            std::cout << optDesc;
            return 0;
        }

        if (optMap.count("file"))
            tomlPath = optMap["file"].as<std::string>();

        if (optMap.count("name")) {
            allRepos = false;
            namedRepo = optMap["name"].as<std::string>();
        }

    } catch (const bp::error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    std::vector<vendr::repo> repoVec = vendr::serializeToml(tomlPath);
    int repoVecSize = repoVec.size();
    if (allRepos) {
        for (int n = 0; n < repoVecSize; n = n + 1) {
            vendr::repo r = repoVec.at(n);
            if (fs::is_directory(fs::path(r.path))) {
                logWarn("repo ({}) at path {} already exists, skipping.", r.name, r.path);
                continue;
            } else {
                vendr::fetch(r);
            }
        }
        return 0;
    } else if (namedRepo.size() > 0) {

        auto repo = vendr::findRepoByName(repoVec, namedRepo);
        
        if (repo) {
            vendr::fetch(repo.value());
            return 0;
        
        } else {
            logErr("repository '{}' was not found in {}.", namedRepo, tomlPath);
            return 1;

        }
    
    }

}
