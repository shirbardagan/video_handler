#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONEncoders.h>
#include <klvparse/stanag/security/SecurityParsers.h>

namespace klvparse {
    namespace encoders {

        /**
         *  0x01 - 1 - classification
         */
        bool security_classification_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x02 - 2 - ccric_method
         */
        bool security_ccric_method_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x03 - 3 - country
         */
        bool security_country_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x04 - 4 - sci_information
         */
        bool security_sci_information_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x05 - 5 - caveats
         */
        bool security_caveats_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x06 - 6 - releasing_instructions
         */
        bool security_releasing_instructions_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x07 - 7 - by
         */
        bool security_by_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x08 - 8 - from
         */
        bool security_from_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x09 - 9 - reason
         */
        bool security_reason_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0a - 10 - date
         */
        bool security_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0b - 11 - system
         */
        bool security_system_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0c - 12 - oc_method
         */
        bool security_oc_method_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0d - 13 - oc_code
         */
        bool security_oc_code_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0e - 14 - comment
         */
        bool security_comment_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0f - 15 - umid
         */
        bool security_umid_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x13 - 19 - stream_id
         */
        bool security_stream_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x14 - 20 - transport_stream_id
         */
        bool security_transport_stream_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x15 - 21 - item_designator_id
         */
        bool security_item_designator_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x16 - 22 - version
         */
        bool security_version_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x17 - 23 - ccric_version_date
         */
        bool security_ccric_version_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
        /**
         *  0x18 - 24 - oc_version_date
         */
        bool security_oc_version_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);
    } // namespace encoders
} // namespace klvparse