#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONEncoders.h>
#include <klvparse/stanag/vmti/VMTIParsers.h>

namespace klvparse {
    namespace encoders {

        bool vmti_fov_encoder(const nl::json &item, std::vector<uint8_t> *res);

        /**
         *  0x01 - 1 - checksum
         */
        // bool vmti_checksum_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x02 - 2 - precision_time_stamp
         */
        bool vmti_precision_time_stamp_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x03 - 3 - system_name
         */
        bool vmti_system_name_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x04 - 4 - ls_version
         */
        bool vmti_ls_version_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x05 - 5 - number_detected
         */
        bool vmti_number_detected_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x06 - 6 - number_reported
         */
        bool vmti_number_reported_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x07 - 7 - frame_number
         */
        bool vmti_frame_number_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x08 - 8 - frame_width
         */
        bool vmti_frame_width_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x09 - 9 - frame_height
         */
        bool vmti_frame_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0a - 10 - source_sensor
         */
        bool vmti_source_sensor_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0b - 11 - horizontal_fov
         */
        bool vmti_horizontal_fov_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0c - 12 - vertical_fov
         */
        bool vmti_vertical_fov_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0d - 13 - miis_id
         */
        bool vmti_miis_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x65 - 101 - target_series
         */
        bool vmti_target_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x66 - 102 - algorithm_series
         */
        bool vmti_algorithm_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

        /**
         *  0x67 - 103 - ontology_series
         */
        bool vmti_ontology_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx);

    } // namespace encoders
} // namespace klvparse