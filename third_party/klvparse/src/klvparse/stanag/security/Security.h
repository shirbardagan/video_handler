#pragma once

#include <bitset>
#include <iostream>
#include <sstream>

#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/Checksum.h>
#include <klvparse/stanag/security/SecurityEncoders.h>
#include <klvparse/stanag/security/SecurityParsers.h>

namespace klvparse {
    class Security
    {
    private:
        static bool get_tag(KLV &item, uint64_t *res);
        bool get_key_complex(uint64_t tag, nl::json *res);
        bool get_key(KLV &item, nl::json *res);
        bool get_value_complex(KLV &item, uint64_t tag, nl::json *res);
        bool get_value(KLV &item, nl::json *res);

    public:
        static std::map<uint64_t, std::string> number_tag;
        static std::unordered_map<std::string, uint64_t> tag_number;

        static std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> parsers;
        static std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> encoders;

        static void init_tags();
        static void init_parsers();
        static void init_encoders();
        static void init();

        std::shared_ptr<Context> ctx = nullptr;
        Security();
        Security(Context ctx);
        Security(std::weak_ptr<Context> ctx);
        Security(std::shared_ptr<Context> ctx);
        virtual ~Security();

        bool parse(const uint8_t *bytes, uint64_t size, nl::json *res, uint64_t skip = 0);
        bool parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip = 0);
        bool parse(ByteReader &buffer, nl::json *res, uint64_t skip = 0);

        // bool parse(const nl::json &j, KLV *res);
        bool encode(const nl::json &j, std::vector<uint8_t> *res);

        bool is_strict();
    };

    namespace parsers {
        bool security_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
    } // namespace parsers

    namespace encoders {
        bool security_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
    } // namespace encoders

} // namespace klvparse