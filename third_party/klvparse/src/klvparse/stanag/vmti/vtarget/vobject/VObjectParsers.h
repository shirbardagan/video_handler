#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/byte/IMAP.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONParsers.h>

#define _FRAME_WIDTH "frame_width"

namespace klvparse {
    namespace parsers {

        /**
         *  0x01 - 1 - ontology
         */
        bool vobject_ontology_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x02 - 2 - class
         */
        bool vobject_class_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

    } // namespace parsers
} // namespace klvparse