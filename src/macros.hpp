#pragma once
#include <git2.h>

#define GITCHECK(call) do { \
    int error = call; \
    if (error != 0) { \
        const git_error *err = git_error_last(); \
        std::cerr << "\033[31merror\033[0m: "; \
        std::cerr << err->message << std::endl; \
        git_libgit2_shutdown(); \
        std::exit(1); \
    } \
} while (0)
