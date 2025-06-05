#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONEncoders.h>

namespace klvparse {
    namespace encoders {

        /**
         *  0x01 - 1 - ontology
         */
        bool vobject_ontology_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x02 - 2 - class
         */
        bool vobject_class_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

    } // namespace encoders
} // namespace klvparse
