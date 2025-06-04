#pragma once

#include <bitset>
#include <iostream>
#include <sstream>

#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/Checksum.h>
#include <klvparse/stanag/vmti/vtarget/VTargetEncoders.h>
#include <klvparse/stanag/vmti/vtarget/VTargetParsers.h>

#define _VTARGET_ID "id"

namespace klvparse {
    class VTarget
    {
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

        static void init_tags();
        static void init_parsers();
        static void init_encoders();
        static void init();

        std::shared_ptr<Context> ctx = nullptr;
        VTarget();
        VTarget(Context ctx);
        VTarget(std::weak_ptr<Context> ctx);
        VTarget(std::shared_ptr<Context> ctx);
        virtual ~VTarget();

        bool parse(const uint8_t *bytes, uint64_t size, nl::json *res, uint64_t skip = 0);
        bool parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip = 0);
        bool parse(ByteReader &buffer, nl::json *res, uint64_t skip = 0);

        // bool parse(const nl::json &j, KLV *res);
        bool encode(const nl::json &j, std::vector<uint8_t> *res);
        bool verify_checksum(ByteReader &reader);

        bool is_strict();
    };

    namespace parsers {
        bool vtarget_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
    } // namespace parsers

    namespace encoders {
        bool vtarget_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
    } // namespace encoders

} // namespace klvparse
