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
         *  0x01 - 1 - track_id
         */
        bool vtracker_track_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x02 - 2 - detection_status
         */
        bool vtracker_detection_status_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x03 - 3 - start_timestamp
         */
        bool vtracker_start_timestamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x04 - 4 - end_timestamp
         */
        bool vtracker_end_timestamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x05 - 5 - bounding_box
         */
        bool vtracker_bounding_box_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x06 - 6 - algorithm
         */
        bool vtracker_algorithm_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x07 - 7 - confidence
         */
        bool vtracker_confidence_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x08 - 8 - number_of_track_points
         */
        bool vtracker_number_of_track_points_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x09 - 9 - locus
         */
        bool vtracker_locus_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0a - 10 - velocity
         */
        bool vtracker_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0b - 11 - acceleration
         */
        bool vtracker_acceleration_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        /**
         *  0x0c - 12 - algorithm_id
         */
        bool vtracker_algorithm_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

    } // namespace parsers
} // namespace klvparse