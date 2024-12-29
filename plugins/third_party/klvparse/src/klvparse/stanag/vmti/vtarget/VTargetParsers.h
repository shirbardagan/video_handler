#pragma once

#include <klvparse/byte/IMAP.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONParsers.h>
#include <klvparse/stanag/vmti/vtarget/vobject/VObject.h>
#include <klvparse/stanag/vmti/vtarget/vtracker/VTracker.h>

#include <chrono>
#include <ctime>
#include <string>

#define _FRAME_WIDTH "frame_width"

namespace klvparse {
    namespace parsers {
        bool vtarget_pixels_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        bool vtarget_lat_long_parser(ByteReader &reader, nl::json *res);

        /**
         *  0x01 - 1 - center
         */
        bool vtarget_center_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x02 - 2 - top_left
         */
        bool vtarget_top_left_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x03 - 3 - bottom_right
         */
        bool vtarget_bottom_right_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x03 - 3 - bottom_right
         */
        bool vtarget_bottom_right_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x04 - 4 - priority
         */
        bool vtarget_priority_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x05 - 5 - confidence
         */
        bool vtarget_confidence_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x06 - 6 - history
         */
        bool vtarget_history_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x07 - 7 - bbox_fill_percentage
         */
        bool vtarget_bbox_fill_percentage_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x08 - 8 - color
         */
        bool vtarget_color_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x09 - 9 - intensity
         */
        bool vtarget_intensity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0a - a - latitude_offset
         */
        bool vtarget_latitude_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0b - b - longitude_offset
         */
        bool vtarget_longitude_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0c - c - height
         */
        bool vtarget_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0d - 13 - top_left_latitude
         */
        bool vtarget_top_left_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0e - 14 - top_left_longitude
         */
        bool vtarget_top_left_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0f - 15 - bottom_right_latitude
         */
        bool vtarget_bottom_right_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x10 - 16 - bottom_right_longitude
         */
        bool vtarget_bottom_right_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x11 - 17 - location
         */
        bool vtarget_location_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x12 - 18 - boundary
         */
        bool vtarget_boundary_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x13 - 19 - y
         */
        bool vtarget_y_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x14 - 20 - x
         */
        bool vtarget_x_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x15 - 21 - fpa
         */
        bool vtarget_fpa_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x65 - 101 - mask
         */
        bool vtarget_mask_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x66 - 102 - object
         */
        bool vtarget_object_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x67 - 103 - feature
         */
        bool vtarget_feature_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x68 - 104 - tracker
         */
        bool vtarget_tracker_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x69 - 105 - chip
         */
        bool vtarget_chip_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6a - 106 - chip series
         */
        bool vtarget_chip_series_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

    } // namespace parsers
} // namespace klvparse