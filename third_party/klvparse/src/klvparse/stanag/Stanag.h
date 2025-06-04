#pragma once

#include <bitset>
#include <iostream>
#include <sstream>

#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/StanagEncoders.h>
#include <klvparse/stanag/StanagParsers.h>
#include <klvparse/stanag/common/Checksum.h>
#include <klvparse/stanag/common/JSONPrettyUgly.h>
namespace klvparse {
    class Stanag {
    private:
        static bool get_tag(KLV &klv, uint64_t *res);
        bool get_key_complex(uint64_t tag, nl::json *res);
        bool get_key(KLV &klv, nl::json *res);
        bool get_value_complex(KLV &klv, uint64_t tag, nl::json *res);
        bool get_value(KLV &klv, nl::json *res);

    public:
        static std::map<uint64_t, std::string> number_tag;
        static std::unordered_map<std::string, uint64_t> tag_number;

        static std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> parsers;
        static std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> encoders;

        static std::unordered_map<uint64_t, std::function<bool(const nl::json &, nl::json *, std::shared_ptr<Context>)>> prettys;
        static std::unordered_map<uint64_t, std::function<bool(const nl::json &, nl::json *, std::shared_ptr<Context>)>> uglys;

        static void init_tags();
        static void init_parsers();
        static void init_encoders();
        static void init_pretty();
        static void init_ugly();
        static void init();

        std::shared_ptr<Context> ctx = nullptr;
        Stanag();
        Stanag(std::shared_ptr<Context> ctx);
        virtual ~Stanag();

        bool parse(const uint8_t *bytes, uint64_t size, nl::json *res, uint64_t skip = 0, bool with_key = true);
        bool parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip = 0, bool with_key = true);
        bool parse(ByteReader &buffer, nl::json *res, uint64_t skip = 0, bool with_key = true);

        bool encode(const nl::json &j, std::vector<uint8_t> *res);

        bool pretty(const nl::json &j, nl::json *res);
        bool ugly(const nl::json &j, nl::json *res);

        bool verify_checksum(ByteReader &reader);

        bool is_strict();

        bool stanag_segment_local_set_parser(ByteReader &reader, nl::json *res);
        bool stanag_segment_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res);
        bool stanag_amend_local_set_parser(ByteReader &reader, nl::json *res);
        bool stanag_amend_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res);
    };
    namespace parsers {
        bool stanag_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        bool stanag_segment_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        bool stanag_amend_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
    } // namespace parsers

    namespace encoders {
        bool stanag_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        bool stanag_segment_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        bool stanag_amend_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
    } // namespace encoders

} // namespace klvparse
