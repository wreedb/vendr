// SPDX-FileCopyrightText: 2025 Will Reed
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <libintl.h>
#include <string>
#include <format>

inline std::string _(const char* msgid) {
    return gettext(msgid);
}

template<typename... Args>
inline std::string _(const char* msgid, Args&&... args) {
    return std::vformat(gettext(msgid), std::make_format_args(args...));
}

template<typename... Args>
inline std::string _n(const char* singular, const char* plural, unsigned long n, Args&&... args) {
    return std::vformat(ngettext(singular, plural, n), std::make_format_args(n, args...));
}