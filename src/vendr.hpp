#pragma once

#include <cstring>
#include <string>
#include <format>
#include <filesystem>
#include <iostream>
#include <vector>
#include <optional>
#include <regex>
#include <fstream>
#include <print>

#include <git2.h>
#include <toml++/toml.hpp>

#include <cpr/cpr.h>
// #include "macros.hpp"
// #include "log.hpp"

namespace fs = std::filesystem;

namespace vendr {

    struct repo {
        std::string name;
        std::string url;
        std::string path;
        std::string tag;
        int depth;
    };

    struct file {
	    std::string name;
	    std::string url;
	    std::string path;
    };

    struct targets {
    	std::vector<vendr::repo> repos;
    	std::vector<vendr::file> files;
    };

    struct httpUrl {
    	std::string full;
    	std::string base;
    	std::string path;
    	std::string filename;
    };

    vendr::file createFile(const std::string& name, const std::string& url, const std::string& path) {
    	return vendr::file
    	{.name = name,
    	 .url  = url,
    	 .path = path};
    }

    vendr::repo createRepo(std::string name, std::string url, std::string path, std::string tag, int depth) {
        return vendr::repo
        {.name  = name,
         .url   = url,
         .path  = path,
         .tag   = tag,
         .depth = depth};
    }

    int fetchProgress(const git_indexer_progress *stats, void *payload) {
        using vendr::log::useColor;
        
        int percentage = (stats->total_objects > 0) ? (stats->received_objects * 100) / stats->total_objects : 0;
        std::cout
        << std::format
        ("\r{}{}{}: git fetch [{}{:3}%{}]",
         (useColor ? "\033[32m" : ""),
         argZero,
         (useColor ? "\033[0m"  : ""),
         (useColor ? "\033[32m" : ""),
         percentage,
         (useColor ? "\033[0m"  : ""))
	    << std::flush;
        return 0;
    }

    void checkoutProgress(const char *path, size_t completedSteps, size_t totalSteps, void *payload) {
        using vendr::log::useColor;

        int percentage = (totalSteps > 0) ? (completedSteps * 100) / totalSteps : 0;
        std::cout
        << std::format
        ("\r{}{}{}: git checkout [{}{:3}%{}]",
         (useColor ? "\033[32m" : ""),
         argZero,
         (useColor ? "\033[0m"  : ""),
         (useColor ? "\033[32m" : ""),
         percentage,
         (useColor ? "\033[0m"  : ""))
	    << std::flush;
    }

    bool httpGetProgress(cpr::cpr_off_t downloadTotal,
                 	     cpr::cpr_off_t downloadNow,
                         cpr::cpr_off_t uploadTotal,
                         cpr::cpr_off_t uploadNow,
                         intptr_t userdata) {
        using vendr::log::useColor;
        int percentage = (downloadTotal > 0) ? (downloadNow * 100) / downloadTotal : 0;
        std::cout
        << std::format
        ("\r{}{}{}: download [{}{:3}%{}]",
         (useColor ? "\033[32m" : ""),
         argZero,
         (useColor ? "\033[0m" : ""),
         (useColor ? "\033[32m" : ""),
         percentage,
         (useColor ? "\033[0m" : ""))
        << std::flush;
        return true;
    }

    httpUrl httpParseUrl(const std::string& url) {
	    std::regex urlRegex(R"(^(https?://[^/]+)(/.*)$)");
	    std::smatch match;
	    if (std::regex_match(url, match, urlRegex)) {
    	    std::string base = match[1].str();
    	    std::string path = match[2].str();

    	    std::string filename;

    	    size_t lastSlash = path.find_last_of('/');

    	    if (lastSlash != std::string::npos && lastSlash + 1 < path.length())
        	    filename = path.substr(lastSlash + 1);
    	    else
        	    filename = "download";


    	    return vendr::httpUrl
		    {.full = url,
		     .base = base,
		     .path = path,
		     .filename = filename};
	    }
	
	    std::regex baseOnlyRegex(R"(^(https?://[^/]+)$)");

	    if (std::regex_match(url, match, baseOnlyRegex)) {
    	    return {match[1].str(), "/", "download"};
	    }
	
	    vendr::log::fatal(1, "invalid url provided: {}", url);
    }

    int get(const vendr::file& file, const bool& overwrite) {
	    std::string outputFile;
	    vendr::httpUrl vUrl = httpParseUrl(file.url);
	    const cpr::Url url = cpr::Url{vUrl.full};

	    cpr::Session session;

	    session.SetUrl(url);
	    session.SetAcceptEncoding({
		    cpr::AcceptEncodingMethods::deflate,
		    cpr::AcceptEncodingMethods::gzip,
		    cpr::AcceptEncodingMethods::zlib
	    });

	    cpr::Response head = session.Head();

	    if (!file.path.empty()) {
		    // use path from toml entry if present
		    outputFile = file.path;
	
	    } else if (head.header.count("Content-Disposition")) {
		    // if header for content-disposition exists, parse and use it as filename
            std::string disp = head.header["Content-Disposition"];
            std::regex filenameRegex(R"(filename[*]?=["]?([^";]+)["]?)");
            std::smatch match;
            if (std::regex_search(disp, match, filenameRegex))
                outputFile = match[1].str();
        } else {
		    // no toml path or header, use manually parsed filename
		    // from vendr::httpParseUrl
            outputFile = vUrl.filename;
	    }


	    if (!overwrite) {
		    // --overwrite not passed, check output path, but don't delete existing file or
		    // download again
		    if (fs::exists(fs::path(outputFile))) {
			    vendr::log::warn("file {} already exists, not overwriting", outputFile);
			    return 0;
		    }
	    } else {
		    if (fs::exists(fs::path(outputFile))) {
			    vendr::log::warn("file {} already exists, overwriting", outputFile);
			    try {
				    // delete the file, not remove_all since it should only be a single file
				    fs::remove(fs::path(outputFile));
			    } catch (const fs::filesystem_error& e) {
				    vendr::log::err("filesystem error encounter while removing {}", outputFile);
				    std::cerr << e.what() << "\n";
				    return 1;
			    }
		    }
	    }

	    // prepare output file stream
	    std::ofstream outfile(outputFile, std::ios::binary);

	    // progress and write callbacks
	    session.SetProgressCallback({{httpGetProgress}});
	    session.SetWriteCallback(
		    cpr::WriteCallback{
			    [&outfile, &outputFile](std::string_view data, intptr_t userdata) -> bool {
				    if (!outfile.is_open()) {
					    vendr::log::err("failure writing data to {}", outputFile);
					    return false;
				    }
				    outfile.write(data.data(), data.size());
				    return true;
			    }
		    }
	    );

	    // perform main get request
        vendr::log::info("{}", file.name);
	    cpr::Response response = session.Get();
	    // newline after progress callback
	    std::cout << "\n";

	    // close the file stream and display info about fetched file
	    outfile.close();
	    vendr::log::info("{} -> {}", file.name, outputFile);
	    return 0;
    }


    int fetch(const vendr::repo& repo, const bool& overwrite) {

	    if (!overwrite) {
		    if (fs::exists(fs::path(repo.path))) {
			    vendr::log::warn("directory {} already exists, not overwriting", repo.path);
			    return 0;
		    }
	    }

	    if (fs::exists(fs::path(repo.path))) {
		    try {
			    vendr::log::warn("directory {} already exists, overwriting", repo.path);
			    fs::remove_all(fs::path(repo.path));
		    } catch (fs::filesystem_error& e) {
			    vendr::log::fatal(1, "error encountered while removing {}", repo.path);
		    }
	    }

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

        // progress callback
        fetchOpts.callbacks.transfer_progress = fetchProgress;
        fetchOpts.callbacks.payload = nullptr;

        GITCHECK
            (git_remote_fetch
            (gitRemote, &refSpecsArr, &fetchOpts, nullptr));

        // newline after fetch progress callback finishes
        std::cout << "\n";


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

	    vendr::log::info("{}@{} -> {}", repo.name, repo.tag, repo.path);
    
        git_remote_free(gitRemote);
        git_repository_free(gitRepo);
	    return 0;
    }

    std::optional<vendr::repo> findRepoByName
        (const std::vector<vendr::repo>& repositories,
         const std::string& name) {
    
        const int length = repositories.size();
        for (int n = 0; n < length; n++)
            if (repositories.at(n).name == name)
                return repositories.at(n);

        return std::nullopt;
    }

    std::optional<vendr::file> findFileByName
        (const std::vector<vendr::file>& files,
         const std::string& name) {
    
        const int length = files.size();
        for (int n = 0; n < length; n++)
            if (files.at(n).name == name)
                return files.at(n);

        return std::nullopt;
    }

    vendr::targets serializeToml(const std::string& filePath) {
        if (!fs::exists(fs::path(filePath)))
            vendr::log::fatal(1, "path {} doesn't exist!", filePath);
        
        try {
            auto rawToml = toml::parse_file(filePath);
       	    std::vector<vendr::repo> repositories;
		    std::vector<vendr::file> files;

		    if (rawToml["repos"].is_array()) {
			    toml::array& repoArray = *rawToml["repos"].as_array();
        	    const int repoArrayLength = repoArray.size();
        
        	    repositories.resize(repoArrayLength);
        
        	    int n = 0;
        	    for (auto& node : repoArray) {
            	    if (toml::table* thisTable = node.as_table()) {

                	    toml::table& current = *thisTable;
                
                	    std::optional<std::string> name = current["name"].value<std::string>();
                	    if (!name)
                    	    vendr::log::fatal(1, "index #{} in {} doesn't contain the required 'name' field!", n, filePath);

                	    std::optional<std::string> url = current["url"].value<std::string>();
                	    if (!url)
                    	    vendr::log::fatal(1, "table {} doesn't contain the required 'url' field!", n, filePath);

                	    std::optional<std::string> path = current["path"].value<std::string>();
                	    if (!path)
                    	    vendr::log::fatal(1, "table {} doesn't contain the required 'path' field!", n, filePath);
               
                	    std::optional<std::string> tag = current["tag"].value<std::string>();
                	    if (!tag)
                    	    vendr::log::fatal(1, "table {} doesn't contain the required 'tag' field!", n, filePath);

                	    int depth = current["depth"].value_or(1);
                
                	    repositories[n] = vendr::createRepo(*name, *url, *path, *tag, depth);
                	    n = n + 1;
            	    }
        	    }
		    }

		    if (rawToml["files"].is_array()) {
			    toml::array& fileArray = *rawToml["files"].as_array();
        	    const int fileArrayLength = fileArray.size();
        
        	    files.resize(fileArrayLength);
        
        	    int n = 0;
        	    for (auto& node : fileArray) {
            	    if (toml::table* thisTable = node.as_table()) {

                	    toml::table& current = *thisTable;
                        
                        // name required, fail otherwise
                	    std::optional<std::string> name = current["name"].value<std::string>();
                	    if (!name)
                    	    vendr::log::fatal(1, "index #{} in {} doesn't contain the required 'name' field!", n, filePath);

                        // url required, fail otherwise
                	    std::optional<std::string> url = current["url"].value<std::string>();
                	    if (!url)
                    	    vendr::log::fatal(1, "table {} doesn't contain the required 'url' field!", n, filePath);

                	    std::string path = current["path"].value_or("");
                
                	    files[n] = vendr::createFile(*name, *url, path);
                	    n = n + 1;
            	    }
        	    }
		    }

		    return vendr::targets{.repos = repositories, .files = files};
    
	    } catch (const toml::parse_error& tomlerr) {
		    vendr::log::err("error encountered while parsing file {}", filePath);
            std::cerr << tomlerr << std::endl;
            std::exit(1);
        }
    }
}
