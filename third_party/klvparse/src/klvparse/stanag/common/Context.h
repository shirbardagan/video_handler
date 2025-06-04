#pragma once

// #include <string_view>

#include <klvparse/byte/ByteReader.h>
#include <klvparse/klv/KLV.h>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#define CTX_LOG(level, message)                                     \
    if (ctx != NULL) {                                              \
        ctx->log(level, message, __FUNCTION__, __FILE__, __LINE__); \
    }
#define CTX_LOG_PARSER(level, message, klv, tag_value, tag_name)                                     \
    if (ctx != NULL) {                                                                               \
        ctx->log_parser(level, message, klv, tag_value, tag_name, __FUNCTION__, __FILE__, __LINE__); \
    }
#define CTX_LOG_ENCODER(level, message, json, tag_value, tag_name)                                     \
    if (ctx != NULL) {                                                                                 \
        ctx->log_encoder(level, message, json, tag_value, tag_name, __FUNCTION__, __FILE__, __LINE__); \
    }
#define CTX_LOG_PRETTY(level, message, klv, tag_value, tag_name)                                     \
    if (ctx != NULL) {                                                                               \
        ctx->log_pretty(level, message, klv, tag_value, tag_name, __FUNCTION__, __FILE__, __LINE__); \
    }
#define CTX_LOG_UGLY(level, message, klv, tag_value, tag_name)                                     \
    if (ctx != NULL) {                                                                             \
        ctx->log_ugly(level, message, klv, tag_value, tag_name, __FUNCTION__, __FILE__, __LINE__); \
    }
namespace klvparse {

    enum Level
    {
        debug,
        info,
        warn,
        error,
        none
    };

    const char *Level_to_string(Level level);
    const char *Level_to_color(Level level);

    class Context
    {
    private:
        std::shared_ptr<nl::json> storage = nullptr;

    public:
        std::vector<const char *> order = std::vector<const char *>();
        std::shared_ptr<nl::json> get_storage();
        Context();
        Context(std::shared_ptr<nl::json> storage);

        std::shared_ptr<Context> sub(const char *name);

        void log(Level level, std::string message, const char *file, const char *function, int line);
        void log_parser(Level level, std::string message, const KLV *klv, size_t tag_value, const std::string &tag_name, const char *function, const char *file, int line);
        void log_encoder(Level level, std::string message, const nl::json *json, size_t tag_value, const std::string &tag_name, const char *function, const char *file, int line);
        void log_pretty(Level level, std::string message, const nl::json *json, size_t tag_value, const std::string &tag_name, const char *function, const char *file, int line);
        void log_ugly(Level level, std::string message, const nl::json *json, size_t tag_value, const std::string &tag_name, const char *function, const char *file, int line);

        bool strict = false;
        Level verbosity = Level::warn;
        // Level verbosity = Level::none;
    };
} // namespace klvparse
