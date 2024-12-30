#include <klvparse/stanag/StanagEncoders.h>
namespace encoders = klvparse::encoders;

bool encoders::stanag_precision_time_stamp_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return timestamp_encoder(item, res);
}
bool encoders::stanag_mission_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
    // return false;
}

bool encoders::stanag_platform_tail_number_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_platform_heading_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_number()) {
        return false;
    }
    return deg_float_to_uint16_encoder(item.get<float>(), res, 0, 360, 0);
}
bool encoders::stanag_platform_pitch_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -20, 20, 0); // TODO: should be reserved?
}
bool encoders::stanag_platform_roll_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -50, 50, 0); // TODO: should be reserved?
}
bool encoders::stanag_platform_true_airspeed_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_platform_indicated_airspeed_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_platform_designation_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_image_source_sensor_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_image_coordinate_system_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_sensor_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0); // TODO: should be reserved?
}
bool encoders::stanag_sensor_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -180, 180, 0); // TODO: should be reserved?
}
bool encoders::stanag_sensor_true_altitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_sensor_horizontal_field_of_view_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 180, 0);
}
bool encoders::stanag_sensor_vertical_field_of_view_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 180, 0);
}
bool encoders::stanag_sensor_relative_azimuth_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_encoder(item, res, 0, 360, 0);
}
bool encoders::stanag_sensor_relative_elevation_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -180, 180, 0); // TODO: should be reserved?
}
bool encoders::stanag_sensor_relative_roll_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_encoder(item, res, 0, 360, 0);
}
bool encoders::stanag_slant_range_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_encoder(item, res, 0, 5e6, 0); // 5,000,000
}
bool encoders::stanag_target_width_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 1e4, 0); // 10,000
}
bool encoders::stanag_frame_center_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    bool _res = deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0); // TODO: should be reserved?
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (_res) {
        if (ctx != nullptr) {
            auto storage = ctx->get_storage();
            if (storage != nullptr) {
                (*storage)[_FRAME_CENTER_LATITUDE] = item;
            }
        }
    }
#endif
    return _res;
}
bool encoders::stanag_frame_center_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    bool _res = deg_double_to_int32_be_reserved_encoder(item, res, -180, 180, 0); // TODO: should be reserved?
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (_res) {
        if (ctx != nullptr) {
            auto storage = ctx->get_storage();
            if (storage != nullptr) {
                (*storage)[_FRAME_CENTER_LONGITUDE] = item;
            }
        }
    }
#endif
    return _res;
}
bool encoders::stanag_frame_center_elevation_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_offset_corner_latitude_point_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (ctx == nullptr) {
        return false;
    }
    auto storage = ctx->get_storage();
    if (storage == nullptr) {
        return false;
    }
    auto item_23 = (*storage)[_FRAME_CENTER_LATITUDE];
    if (!item_23.is_number()) {
        return false;
    }
    double item_23_val = item_23.get<double>();
#else
    double item_23_val = 0;
#endif
    return deg_float_to_int16_be_reserved_encoder(item, res, -0.075, 0.075, item_23_val); // TODO: should be reserved?
}
bool encoders::stanag_offset_corner_longitude_point_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (ctx == nullptr) {
        return false;
    }
    auto storage = ctx->get_storage();
    if (storage == nullptr) {
        return false;
    }
    auto item_24 = (*storage)[_FRAME_CENTER_LONGITUDE];
    if (!item_24.is_number()) {
        return false;
    }
    double item_24_val = item_24.get<double>();
#else
    double item_24_val = 0;
#endif
    return deg_float_to_int16_be_reserved_encoder(item, res, -0.075, 0.075, item_24_val); // TODO: should be reserved?
}
bool encoders::stanag_offset_corner_latitude_point_1_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_longitude_point_1_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_latitude_point_2_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_longitude_point_2_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_latitude_point_3_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_longitude_point_3_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_latitude_point_4_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_offset_corner_longitude_point_4_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_encoder(item, res, ctx);
}
bool encoders::stanag_icing_detected_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "Detector off") {
            source = 0;
        } else if (str == "No icing Detected") {
            source = 1;
        } else if (str == "Icing Detected") {
            source = 2;
        } else {
            return false;
        }
    } else if (item.is_number_unsigned()) {
        source = item.get<uint8_t>();
        if (source > 2) {
            return false;
        }
    }
    return encode_uint8(source, res);
}
bool encoders::stanag_wind_direction_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 360, 0);
}
bool encoders::stanag_wind_speed_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint8_encoder(item, res, 0, 100, 0);
}
bool encoders::stanag_static_pressure_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 5000, 0);
}
bool encoders::stanag_density_altitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_outside_air_temperature_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_target_location_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_reserved_encoder(item, res, -90, 90, 0); // TODO: should be reserved?
}
bool encoders::stanag_target_location_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_reserved_encoder(item, res, -180, 180, 0); // TODO: should be reserved?
}
bool encoders::stanag_target_location_elevation_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_target_track_gate_width_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_number()) {
        return false;
    }
    uint16_t doubled = item.get<uint16_t>();
    uint8_t halved = doubled >> 1;
    return encode_uint8(halved, res);
}
bool encoders::stanag_target_track_gate_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_number()) {
        return false;
    }
    uint16_t doubled = item.get<uint16_t>();
    uint8_t halved = doubled >> 1;
    return encode_uint8(halved, res);
}
bool encoders::stanag_target_error_estimate_ce90_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 4095, 0);
}
bool encoders::stanag_target_error_estimate_le90_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 4095, 0);
}
bool encoders::stanag_generic_flag_data_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_security_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return security_local_set_encoder(item, res, ctx);
    // return general_klv_encoder(item, res);
}
bool encoders::stanag_differential_pressure_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 5000, 0);
}
bool encoders::stanag_platform_angle_of_attack_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -20, 20, 0);
}
bool encoders::stanag_platform_vertical_speed_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -180, 180, 0);
}
bool encoders::stanag_platform_sideslip_angle_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -20, 20, 0);
}
bool encoders::stanag_airfield_barometric_pressure_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 5000, 0);
}
bool encoders::stanag_airfield_elevation_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_relative_humidity_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint8_encoder(item, res, 0, 100, 0);
}
bool encoders::stanag_platform_ground_speed_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_ground_range_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_uint32_encoder(item, res, 0, 5e6, 0); // 5,000,000
}
bool encoders::stanag_platform_fuel_remaining_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 1e4, 0); // 10,000
}
bool encoders::stanag_platform_call_sign_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_weapon_load_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint16_encoder(item, res);
}
bool encoders::stanag_weapon_fired_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::stanag_laser_prf_code_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint16_encoder(item, res);
}
bool encoders::stanag_sensor_field_of_view_name_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "Ultranarrow") {
            source = 0;
        } else if (str == "Narrow") {
            source = 1;
        } else if (str == "Medium") {
            source = 2;
        } else if (str == "Wide") {
            source = 3;
        } else if (str == "Ultrawide") {
            source = 4;
        } else if (str == "Narrow Medium") {
            source = 5;
        } else if (str == "2x Ultranarrow") {
            source = 6;
        } else if (str == "4x Ultranarrow") {
            source = 7;
        } else if (str == "Continuous Zoom") {
            source = 8;
        } else if (str == "Reserved - Do not use") {
            source = 9;
        } else {
            source = 9;
            // return false;
        }
    } else if (item.is_number_unsigned()) {
        source = item.get<uint8_t>();
        // if (source > 9) {
        //     return false;
        // }
    }
    return encode_uint8(source, res);
}
bool encoders::stanag_platform_magnetic_heading_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 360, 0);
}
bool encoders::stanag_uas_datalink_ls_version_number_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
// bool encoders::stanag_deprecated_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
//     return deg_float_to_uint16_encoder(item, res, 0, 360, 0);
// }
bool encoders::stanag_target_location_covariance_matrix_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_encoder(item, res, -90, 90, 0);
}
bool encoders::stanag_alternate_platform_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_encoder(item, res, -90, 90, 0);
}
bool encoders::stanag_alternate_platform_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_encoder(item, res, -180, 180, 0);
}
bool encoders::stanag_alternate_platform_altitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
bool encoders::stanag_alternate_platform_name_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::stanag_alternate_platform_heading_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, 0, 360, 0);
}
bool encoders::stanag_event_start_time_utc_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return timestamp_encoder(item, res);
}
bool encoders::stanag_rvt_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    // return general_klv_encoder(item, res);
    return string_encoder(item, res);
}
// 74
bool encoders::stanag_vmti_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vmti_local_set_encoder(item, res, ctx);
    // return false;
}
// 75
bool encoders::stanag_sensor_ellipsoid_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
// 76
bool encoders::stanag_alternate_platform_ellipsoid_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
// 77
bool encoders::stanag_operational_mode_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "Other") {
            source = 0;
        } else if (str == "Operational") {
            source = 1;
        } else if (str == "Training") {
            source = 2;
        } else if (str == "Exercise") {
            source = 3;
        } else if (str == "Maintenance") {
            source = 4;
        } else if (str == "Test") {
            source = 5;
        } else if (str == "Reserved - Do not use") {
            source = 6;
        } else {
            source = 6;
            // return false;
        }
    } else if (item.is_number_unsigned()) {
        source = item.get<uint8_t>();
        // if (source > 2) {
        //     return false;
        // }
    }
    return encode_uint8(source, res);
}
// 78
bool encoders::stanag_frame_center_height_above_ellipsoid_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint16_encoder(item, res, -900, 19000, -900);
}
// 79
bool encoders::stanag_sensor_north_velocity_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -327, 327, 0);
}
// 80
bool encoders::stanag_sensor_east_velocity_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_int16_be_reserved_encoder(item, res, -327, 327, 0);
}
// 81
bool encoders::stanag_image_horizon_pixel_pack_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_array()) {
        return false;
    }
    auto size = item.size();
    if (size % 2 != 0) {
        return false;
    }
    std::vector<uint8_t> _res;
    std::vector<uint8_t> temp;
    for (size_t i = 0; i < 2 && i < size; i++) {
        auto &pair = item.at(i);
        if (!pair.is_array()) {
            return false;
        }
        auto _size = pair.size();
        if (_size != 2) {
            return false;
        }

        auto &x = pair.at(0);
        double x_value = x.get<double>();
        if (!deg_float_to_uint8_encoder(x, &temp, 0, 100, 0)) {
            return false;
        }
        _res.insert(_res.end(), temp.begin(), temp.end());

        auto &y = pair.at(1);
        double y_value = y.get<double>();
        if (!deg_float_to_uint8_encoder(y, &temp, 0, 100, 0)) {
            return false;
        }
        _res.insert(_res.end(), temp.begin(), temp.end());
    }
    for (size_t i = 2; i < 4 && i < size; i++) {
        auto &pair = item.at(i);
        if (!pair.is_array()) {
            return false;
        }
        auto _size = pair.size();
        if (_size != 2) {
            return false;
        }

        auto &x = pair.at(0);
        if (!deg_double_to_int32_be_encoder(x, &temp, -90, 90, 0)) {
            return false;
        }
        _res.insert(_res.end(), temp.begin(), temp.end());

        auto &y = pair.at(1);
        if (!deg_double_to_int32_be_encoder(y, &temp, -180, 180, 0)) {
            return false;
        }
        _res.insert(_res.end(), temp.begin(), temp.end());
    }
    *res = _res;
    return true;
}

bool encoders::stanag_corner_latitude_point_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0);
}
bool encoders::stanag_corner_longitude_point_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -180, 180, 0);
}
// 82
bool encoders::stanag_corner_latitude_point_1_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_encoder(item, res, ctx);
}
// 83
bool encoders::stanag_corner_longitude_point_1_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_encoder(item, res, ctx);
}
// 84
bool encoders::stanag_corner_latitude_point_2_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_encoder(item, res, ctx);
}
// 85
bool encoders::stanag_corner_longitude_point_2_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_encoder(item, res, ctx);
}
// 86
bool encoders::stanag_corner_latitude_point_3_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_encoder(item, res, ctx);
}
// 87
bool encoders::stanag_corner_longitude_point_3_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_encoder(item, res, ctx);
}
// 88
bool encoders::stanag_corner_latitude_point_4_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_encoder(item, res, ctx);
}
// 89
bool encoders::stanag_corner_longitude_point_4_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_encoder(item, res, ctx);
}
// 90
bool encoders::stanag_platform_pitch_angle_full_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0);
}
// 91
bool encoders::stanag_platform_roll_angle_full_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0);
}
// 92
bool encoders::stanag_platform_angle_of_attack_full_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -90, 90, 0);
}
// 93
bool encoders::stanag_platform_sideslip_angle_full_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_double_to_int32_be_reserved_encoder(item, res, -180, 180, 0);
}
// 94
bool encoders::stanag_miis_core_identifier_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_object()) {
        return false;
    }
    std::vector<uint8_t> _res(1 + 1 + 16 + 16);
    uint8_t ber_oid_value = 1;
    auto _p = item.find("ber_oid_value");
    if (_p != item.end()) {
        ber_oid_value = _p.value().get<uint8_t>();
    }
    _res[0] = ber_oid_value;

    uint8_t sensor_type_id = item["sensor_type_id"].get<uint8_t>();
    uint8_t platform_id_type = item["platform_id_type"].get<uint8_t>();
    uint8_t window_id_type = item["window_id_type"].get<uint8_t>();
    uint8_t minor_id_type = item["minor_id_type"].get<uint8_t>();
    _res[1] = ((sensor_type_id & 0b11) << 5) |
              ((platform_id_type & 0b11) << 3) |
              ((window_id_type & 0b1) << 2) |
              ((minor_id_type & 0b1) << 1);

    std::vector<uint8_t> physical_sensor_id;
    if (!uuid_encoder(item["physical_sensor_id"], &physical_sensor_id)) {
        return false;
    }
    auto start = _res.begin() + 2;
    std::swap_ranges(start, start + physical_sensor_id.size(), physical_sensor_id.begin());
    // _res.swap(res->begin() + 2, physical_sensor_id.begin(), physical_sensor_id.end());

    std::vector<uint8_t> virtual_platform_id;
    if (!uuid_encoder(item["virtual_platform_id"], &virtual_platform_id)) {
        return false;
    }
    start = _res.begin() + 2 + virtual_platform_id.size();
    std::swap_ranges(start, start + virtual_platform_id.size(), virtual_platform_id.begin());
    // _res.swap(res->begin() + 2, virtual_platform_id.begin(), virtual_platform_id.end());

    *res = _res;
    return true;
}
