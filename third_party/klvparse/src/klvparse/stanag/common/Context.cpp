#include <klvparse/stanag/common/Context.h>

using namespace klvparse;

std::shared_ptr<nl::json> Context::get_storage() {
    return this->storage;
}

Context::Context() {
    this->storage = std::make_shared<nl::json>(nl::json::object());
}

Context::Context(std::shared_ptr<nl::json> storage) {
    this->storage = storage;
}

// std::ostream& operator << (std::ostream& os, const klvparse::Level& obj)
// {
//    os << static_cast<std::underlying_type<klvparse::Level>::type>(obj);
//    return os;
// }

const char *klvparse::Level_to_string(Level level) {
    switch (level) {
    case Level::debug:
        return "DEBUG";
        break;
    case Level::info:
        return "INFO";
        break;
    case Level::warn:
        return "WARN";
        break;
    case Level::error:
        return "ERROR";
        break;
    default:
        break;
    }
    return NULL;
}

// #define _COLOR(number) "\u001b[" \
//                        "38;5;" number "m"
#define _COLOR(number) "\u001b[" \
                       "38;5;" number "m"
#define _COLOR_RESET "\u001b[0m"

const char *klvparse::Level_to_color(Level level) {
    switch (level) {
    case Level::debug:
        return _COLOR("2");
        break;
    case Level::info:
        return _COLOR("7");
        break;
    case Level::warn:
        return _COLOR("3");
        break;
    case Level::error:
        return _COLOR("1");
        break;
    default:
        break;
    }
    return NULL;
}

std::shared_ptr<Context> Context::sub(const char *name) {
    auto sub = std::make_shared<Context>(this->storage);
    sub->order = std::vector<const char *>(this->order.begin(), this->order.end());
    return sub;
}

void _level(std::ostream &os, Level level) {
    os << "[" << Level_to_color(level) << Level_to_string(level) << _COLOR_RESET << "]";
}

void _order(std::ostream &os, std::vector<const char *> &order) {
    size_t order_size = order.size();
    if (order_size > 0) {
        os << "[";
        for (size_t i = 0; i < order_size - 1; i++) {
            os << order.at(i) << "/";
        }
        os << order.at(order_size - 1);
        os << "]";
    }
}

void Context::log(
    Level level,
    std::string message,
    const char *function, const char *file, int line) {
    if (level < this->verbosity) {
        return;
    }
    std::ostream &os = std::cout;
    _level(os, level);
    _order(os, order);
    if (message != "") {
        os << "[" << message << "]";
    }
    os << _COLOR("8") << " (" << file << ":" << line << ")" << _COLOR_RESET;
    os << std::endl;
}

void Context::log_parser(
    Level level,
    std::string message,
    const KLV *klv, size_t tag_value, const std::string &tag_name,
    const char *function, const char *file, int line) {
    if (level < this->verbosity) {
        return;
    }
    std::ostream &os = std::cout;
    _level(os, level);
    _order(os, order);
    os << "[" << tag_value << ", " << tag_name << "]";
    if (message != "") {
        os << "[" << message << "]";
    }
    os << _COLOR("8") << " (" << file << ":" << line << ")" << _COLOR_RESET;
    os << std::endl;
    if (level == Level::error) {
        os << klv->value.dump() << std::endl;
    }
}

void Context::log_encoder(
    Level level,
    std::string message,
    const nl::json *json, size_t tag_value, const std::string &tag_name,
    const char *function, const char *file, int line) {
    if (level < this->verbosity) {
        return;
    }
    std::ostream &os = std::cout;
    _level(os, level);
    _order(os, order);
    os << "[" << tag_value << ", " << tag_name << "]";
    if (message != "") {
        os << "[" << message << "]";
    }
    os << _COLOR("8") << " (" << file << ":" << line << ")" << _COLOR_RESET;
    os << std::endl;
}

void Context::log_pretty(
    Level level,
    std::string message,
    const nl::json *json, size_t tag_value, const std::string &tag_name,
    const char *function, const char *file, int line) {
    if (level < this->verbosity) {
        return;
    }
    std::ostream &os = std::cout;
    _level(os, level);
    _order(os, order);
    os << "[" << tag_value << ", " << tag_name << "]";
    if (message != "") {
        os << "[" << message << "]";
    }
    os << _COLOR("8") << " (" << file << ":" << line << ")" << _COLOR_RESET;
    os << std::endl;
}

void Context::log_ugly(
    Level level,
    std::string message,
    const nl::json *json, size_t tag_value, const std::string &tag_name,
    const char *function, const char *file, int line) {
    if (level < this->verbosity) {
        return;
    }
    std::ostream &os = std::cout;
    _level(os, level);
    _order(os, order);
    os << "[" << tag_value << ", " << tag_name << "]";
    if (message != "") {
        os << "[" << message << "]";
    }
    os << _COLOR("8") << " (" << file << ":" << line << ")" << _COLOR_RESET;
    os << std::endl;
}