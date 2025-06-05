#pragma once
#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/byte/IMAP.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONParsers.h>

namespace klvparse {

    const char *security_classification_to_string(uint8_t classification);
    const char *security_ccric_method_to_string(uint8_t ccric_method);
    const char *security_oc_method_to_string(uint8_t oc_method);

    namespace parsers {

        /**
         *  0x01 - 1 - classification
         */
        bool security_classification_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x02 - 2 - ccric_method
         */
        bool security_ccric_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x03 - 3 - country
         */
        bool security_country_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x04 - 4 - sci_information
         */
        bool security_sci_information_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x05 - 5 - caveats
         */
        bool security_caveats_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x06 - 6 - releasing_instructions
         */
        bool security_releasing_instructions_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x07 - 7 - by
         */
        bool security_by_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x08 - 8 - from
         */
        bool security_from_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x09 - 9 - reason
         */
        bool security_reason_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0a - 10 - date
         */
        bool security_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0b - 11 - system
         */
        bool security_system_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0c - 12 - oc_method
         */
        bool security_oc_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0d - 13 - oc_code
         */
        bool security_oc_code_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0e - 14 - comment
         */
        bool security_comment_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x0f - 15 - umid
         */
        bool security_umid_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x13 - 19 - stream_id
         */
        bool security_stream_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x14 - 20 - transport_stream_id
         */
        bool security_transport_stream_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x15 - 21 - item_designator_id
         */
        bool security_item_designator_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x16 - 22 - version
         */
        bool security_version_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x17 - 23 - ccric_version_date
         */
        bool security_ccric_version_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

        /**
         *  0x18 - 24 - oc_version_date
         */
        bool security_oc_version_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage);

    } // namespace parsers
} // namespace klvparse