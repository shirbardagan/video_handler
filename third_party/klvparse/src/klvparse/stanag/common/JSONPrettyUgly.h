
#pragma once

#include <klvparse/stanag/common/Timestamp.h>
#include <klvparse/stanag/common/Azimuth.h>
#include <klvparse/stanag/common/Context.h>

#include <nlohmann/json.hpp>

namespace klvparse {
    namespace pretty {
        bool timestamp_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);

        bool azimuth_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);
        bool latitude_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);
        bool longitude_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);
    }; // namespace pretty

    namespace ugly {
        bool timestamp_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);

        bool azimuth_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);
        bool latitude_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);
        bool longitude_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx);

    }; // namespace ugly
} // namespace klvparse