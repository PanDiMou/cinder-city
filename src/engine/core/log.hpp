// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_LOG_HPP
#define CINDER_CITY_LOG_HPP

#include <format>
#include <string>
#include <utility>

namespace cinder::log {
    enum class level {
        info,
        warning,
        error
    };

    namespace detail {
        void write(level, std::string const&);
    }

    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        detail::write(level::info, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) {
        detail::write(level::warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        detail::write(level::error, std::format(fmt, std::forward<Args>(args)...));
    }
}

#endif //CINDER_CITY_LOG_HPP
