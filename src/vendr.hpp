#include <cstring>

#include <string>
#include <format>
#include <filesystem>
#include <iostream>
#include <vector>
#include <optional>

#include <git2.h>

#include "macros.hpp"

std::string argZero = "vendr";

namespace fs = std::filesystem;

template <typename... Args>
void logInfo (std::format_string<Args...> fmt, Args&&... args) {
    std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
    std::cout << "\033[34m"
              << argZero
              << "\033[0m: "
              << fmtMsg
              << std::endl;
}

template <typename... Args>
void logWarn (std::format_string<Args...> fmt, Args&&... args) {
    std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
    std::cout << "\033[33m"
              << argZero
              << "\033[0m: "
              << fmtMsg
              << std::endl;
}

template <typename... Args>
void logErr (std::format_string<Args...> fmt, Args&&... args) {
    std::string fmtMsg = std::format(fmt, std::forward<Args>(args)...);
    std::cerr << "\033[31m"
              << argZero
              << "\033[0m: "
              << fmtMsg
              << std::endl;
}

void logFatal (const std::string& msg, const int exitCode) {
    std::cerr << "(fatal) \033[31m"
              << argZero
              << "\033[0m: "
              << msg
              << std::endl;
    std::exit(exitCode);
}

namespace vendr {
    
    struct repo {
        std::string name;
        std::string url;
        std::string path;
        std::string tag;
        int depth;
    };
    
    vendr::repo createRepo(std::string name, std::string url, std::string path, std::string tag, int depth) {
        return vendr::repo
        {.name  = name,
         .url   = url,
         .path  = path,
         .tag   = tag,
         .depth = depth};
    }

    void ppRepo(vendr::repo repo) {
        std::cout << std::format("\033[32mname\033[0m:  {}\n", repo.name);
        std::cout << std::format("\033[32murl\033[0m:   {}\n", repo.url);
        std::cout << std::format("\033[32mpath\033[0m:  {}\n", repo.path);
        std::cout << std::format("\033[32mtag\033[0m:   {}\n", repo.tag);
        std::cout << std::format("\033[32mdepth\033[0m: {}\n", repo.depth);
    }
    
    int fetchProgress(const git_indexer_progress *stats, void *payload) {
        int percentage = (stats->total_objects > 0) ? (stats->received_objects * 100) / stats->total_objects : 0;
        std::cout
            << std::format
            ("\r\033[32m{}\033[0m: fetched [\033[32m{:3}%\033[0m]",
             argZero, percentage) << std::flush;
             //stats->received_objects,
             //stats->total_objects)
        return 0;
    }
    
    void checkoutProgress(const char *path, size_t completedSteps, size_t totalSteps, void *payload) {
        int percentage = (totalSteps > 0) ? (completedSteps * 100) / totalSteps : 0;
        std::cout
            << std::format ("\r\033[32m{}\033[0m: checkout [\033[32m{:3}%\033[0m]", argZero, percentage) << std::flush;
             //argZero,
             //completedSteps,
             //totalSteps)
    }

    void fetch(vendr::repo repo) {
        git_libgit2_init();

        git_repository *gitRepo = nullptr;
        git_remote   *gitRemote = nullptr;

        GITCHECK
            (git_repository_init
            (&gitRepo, repo.path.c_str(), false));

        GITCHECK
            (git_remote_create
            (&gitRemote, gitRepo, "origin", repo.url.c_str()));
        
        std::string refSpec = std::format
            ("+refs/tags/{}:refs/tags/{}", repo.tag, repo.tag);
        
        std::string revSpec = std::format
            ("refs/tags/{}", repo.tag);

        const char *refSpecCStr = refSpec.c_str();
        
        git_strarray refSpecsArr{const_cast<char**>(&refSpecCStr), 1};
        git_fetch_options fetchOpts = GIT_FETCH_OPTIONS_INIT;

        // libgit2 considers a depth of 0 to be infinite depth; e.g. a full clone
        fetchOpts.depth = repo.depth;
        
        fetchOpts.proxy_opts.type = GIT_PROXY_AUTO;
        fetchOpts.follow_redirects = GIT_REMOTE_REDIRECT_INITIAL;
        fetchOpts.custom_headers = git_strarray{nullptr, 0};

        // progress callback, shows objects fetched of objects total
        fetchOpts.callbacks.transfer_progress = fetchProgress;
        fetchOpts.callbacks.payload = nullptr;

        GITCHECK
            (git_remote_fetch
            (gitRemote, &refSpecsArr, &fetchOpts, nullptr));

        // newline after fetch progress callback finishes
        std::cout << std::endl;


        git_object *gitRev;

        GITCHECK
            (git_revparse_single
            (&gitRev, gitRepo, revSpec.c_str()));

        git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
        checkoutOpts.checkout_strategy = GIT_CHECKOUT_SAFE;
        
        // progress callback, shows objects fetched of objects total
        checkoutOpts.progress_cb = checkoutProgress;
        
        GITCHECK
            (git_reset
            (gitRepo, gitRev, GIT_RESET_HARD, &checkoutOpts));

        // newline after checkout progress callback finishes
        std::cout << std::endl;
        
        std::cout << "fetched " << repo.name << "@" << repo.tag << std::endl;
        
        git_remote_free(gitRemote);
        git_repository_free(gitRepo);
    }

    std::optional<vendr::repo> findRepoByName
        (const std::vector<vendr::repo>& repositories,
         const std::string& name) {
        
        const int length = repositories.size();
        for (int n = 0; n < length; n++) {
            if (repositories.at(n).name == name)
                return repositories.at(n);
        }
        return std::nullopt;
    }
            

    std::vector<vendr::repo> serializeToml(const std::string& filePath) {
        if (!fs::exists(fs::path(filePath)))
            logFatal(std::format("path {} doesn't exist!", filePath), 1);
            
        try {
            auto rawToml = toml::parse_file(filePath);
            toml::array& repoArray = *rawToml["repos"].as_array();
            const int repoArrayLength = repoArray.size();
            
            std::vector<vendr::repo> repositories;
            repositories.resize(repoArrayLength);
            
            int n = 0;
            for (auto& node : repoArray) {
                if (toml::table* thisTable = node.as_table()) {

                    toml::table& current = *thisTable;
                    
                    std::optional<std::string> name = current["name"].value<std::string>();
                    if (!name)
                        logFatal(std::format("index #{} in {} doesn't contain the required 'name' field!", n, filePath), 1);

                    std::optional<std::string> url = current["url"].value<std::string>();
                    if (!url)
                        logFatal(std::format("table {} doesn't contain the required 'url' field!", n, filePath), 1);
                    
                    std::optional<std::string> path = current["path"].value<std::string>();
                    if (!path)
                        logFatal(std::format("table {} doesn't contain the required 'path' field!", n, filePath), 1);
                    
                    std::optional<std::string> tag = current["tag"].value<std::string>();
                    if (!tag)
                        logFatal(std::format("table {} doesn't contain the required 'tag' field!", n, filePath), 1);

                    int depth = current["depth"].value_or(1);
                    
                    repositories[n] = vendr::createRepo(*name, *url, *path, *tag, depth);
                    n = n + 1;
                }
            }

            return repositories;

        } catch (const toml::parse_error& e) {
            logErr("error encountered while parsing file {}", filePath);
            std::cerr << e << std::endl;
            std::exit(1);
        }

    }
    
}

