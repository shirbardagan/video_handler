#include <klvparse/stanag/StanagParsers.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;

bool parsers::stanag_checksum_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint16_parser(reader, res);
}

bool parsers::stanag_precision_time_stamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return timestamp_parser(reader, res);
}

bool parsers::stanag_mission_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_platform_tail_number_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_platform_heading_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_platform_pitch_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -20, 20, 0, "Out of Range");
}

bool parsers::stanag_platform_roll_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -50, 50, 0, "Out of Range");
}

bool parsers::stanag_platform_true_airspeed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_platform_indicated_airspeed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_platform_designation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_image_source_sensor_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_image_coordinate_system_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_sensor_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Reserved");
}

bool parsers::stanag_sensor_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "Reserved");
}

bool parsers::stanag_sensor_true_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_sensor_horizontal_field_of_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 180, 0);
}

bool parsers::stanag_sensor_vertical_field_of_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 180, 0);
}

bool parsers::stanag_sensor_relative_azimuth_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint32_to_double_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_sensor_relative_elevation_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "Reserved");
}

bool parsers::stanag_sensor_relative_roll_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint32_to_double_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_slant_range_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint32_to_double_parser(reader, res, 0, 5e6, 0); // 5,000,000
}

bool parsers::stanag_target_width_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 1e4, 0); // 10,000
}

bool parsers::stanag_frame_center_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto remaining = reader.get_remaining();
    int32_t value = 0;
    if (remaining == 8) {
        int64_t _value = 0;
        if (!reader.get_i64(&_value)) {
            return false;
        }
        value = _value;
    } else {
        if (!reader.get_i32(&value)) {
            return false;
        }
    }
    // bool _res = deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "N/A (Off-Earth)");
    double dest = 0;
    bool _res = deg_int32_to_double_reserved(value, &dest, -90, 90, 0);
    if (_res) {
        *res = dest;
    } else {
        *res = "N/A (Off-Earth)";
    }
    _res = true;
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (_res) {
        if (ctx != nullptr) {
            auto storage = ctx->get_storage();
            if (storage != nullptr) {
                (*storage)[_FRAME_CENTER_LATITUDE] = *res;
            }
        }
    }
#endif
    return _res;
    // return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "N/A (Off-Earth)");
}

bool parsers::stanag_frame_center_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto remaining = reader.get_remaining();
    int32_t value = 0;
    if (remaining == 8) {
        int64_t _value = 0;
        if (!reader.get_i64(&_value)) {
            return false;
        }
        value = _value;
    } else {
        if (!reader.get_i32(&value)) {
            return false;
        }
    }
    // bool _res = deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "N/A (Off-Earth)");
    double dest = 0;
    bool _res = deg_int32_to_double_reserved(value, &dest, -180, 180, 0);
    if (_res) {
        *res = dest;
    } else {
        *res = "N/A (Off-Earth)";
    }
    _res = true;
#if defined(USE_FRAME_CENTER_FOR_OFFSET) && USE_FRAME_CENTER_FOR_OFFSET
    if (_res) {
        if (ctx != nullptr) {
            auto storage = ctx->get_storage();
            if (storage != nullptr) {
                (*storage)[_FRAME_CENTER_LONGITUDE] = *res;
            }
        }
    }
#endif
    return _res;
}

bool parsers::stanag_frame_center_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_offset_corner_latitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
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
    return deg_int16_to_float_reserved_parser(reader, res, -0.075, 0.075, item_23_val, "N/A (Off-Earth)");
}

bool parsers::stanag_offset_corner_longitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
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
    return deg_int16_to_float_reserved_parser(reader, res, -0.075, 0.075, item_24_val, "N/A (Off-Earth)");
}

bool parsers::stanag_offset_corner_latitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_longitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_latitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_longitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_latitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_longitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_latitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_offset_corner_longitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_offset_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_icing_detected_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t value;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    // g_value_unset(&item->value_parsed);
    // g_value_init(&item->value_parsed, G_TYPE_STRING);
    bool _res = true;
    switch (value) {
    case 0:
        // g_value_set_string(&item->value_parsed, "Detector off");
        *res = "Detector off";
        break;
    case 1:
        // g_value_set_string(&item->value_parsed, "No icing Detected");
        *res = "No icing Detected";
        break;
    case 2:
        // g_value_set_string(&item->value_parsed, "Icing Detected");
        *res = "Icing Detected";
        break;

    default:
        _res = false;
        break;
    }
    return _res;
}

bool parsers::stanag_wind_direction_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_wind_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint8_to_float_parser(reader, res, 0, 100, 0);
}

bool parsers::stanag_static_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 5000, 0);
}

bool parsers::stanag_density_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_outside_air_temperature_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_target_location_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "N/A (Off-Earth)");
}

bool parsers::stanag_target_location_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "N/A (Off-Earth)");
}

bool parsers::stanag_target_location_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_target_track_gate_width_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t _res = 0;
    if (!reader.get_ui8(&_res)) {
        return false;
    }
    // uint16_t doubled = (uint16_t)_res * 2;
    uint16_t doubled = (uint16_t)_res << 1;
    *res = doubled;
    return true;
}

bool parsers::stanag_target_track_gate_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t _res = 0;
    if (!reader.get_ui8(&_res)) {
        return false;
    }
    // uint16_t doubled = (uint16_t)_res * 2;
    uint16_t doubled = (uint16_t)_res << 1;
    *res = doubled;
    return true;
}

bool parsers::stanag_target_error_estimate_ce90_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 4095, 0);
}

bool parsers::stanag_target_error_estimate_le90_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 4095, 0);
}

bool parsers::stanag_generic_flag_data_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_security_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return security_local_set_parser(reader, res, ctx);
    // return general_klv_parser(reader, res);
}

bool parsers::stanag_differential_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 5000, 0);
}

bool parsers::stanag_platform_angle_of_attack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -20, 20, 0, "Out of Range");
}

bool parsers::stanag_platform_vertical_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -180, 180, 0, "Out of Range");
}

bool parsers::stanag_platform_sideslip_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -20, 20, 0, "Out of Range");
}

bool parsers::stanag_airfield_barometric_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 5000, 0);
}

bool parsers::stanag_airfield_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_relative_humidity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint8_to_float_parser(reader, res, 0, 100, 0);
}

bool parsers::stanag_platform_ground_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_ground_range_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint32_to_double_parser(reader, res, 0, 5e6, 0); // 5,000,000
}

bool parsers::stanag_platform_fuel_remaining_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 1e4, 0); // 10,000
}

bool parsers::stanag_platform_call_sign_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_weapon_load_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint16_parser(reader, res);
}

bool parsers::stanag_weapon_fired_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_laser_prf_code_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint16_parser(reader, res);
}

bool parsers::stanag_sensor_field_of_view_name_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t value = 0;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    switch (value) {
    case 0:
        *res = "Ultranarrow";
        break;
    case 1:
        *res = "Narrow";
        break;
    case 2:
        *res = "Medium";
        break;
    case 3:
        *res = "Wide";
        break;
    case 4:
        *res = "Ultrawide";
        break;
    case 5:
        *res = "Narrow Medium";
        break;
    case 6:
        *res = "2x Ultranarrow";
        break;
    case 7:
        *res = "4x Ultranarrow";
        break;
    case 8:
        *res = "Continuous Zoom";
        break;
    default:
        *res = "Reserved - Do not use";
        break;
    }
    return true;
}

bool parsers::stanag_platform_magnetic_heading_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_uas_datalink_ls_version_number_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_deprecated_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    // g_value_unset(&item->value_parsed);
    // g_value_init(&item->value_parsed, G_TYPE_STRING);
    // g_value_set_string(&item->value_parsed, "Deprecated");
    *res = "Deprecated";
    return true;
}

bool parsers::stanag_target_location_covariance_matrix_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Reserved");
}

bool parsers::stanag_alternate_platform_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Reserved");
}

bool parsers::stanag_alternate_platform_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "Reserved");
}

bool parsers::stanag_alternate_platform_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_alternate_platform_name_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_alternate_platform_heading_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_event_start_time_utc_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return timestamp_parser(reader, res);
}

bool parsers::stanag_rvt_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    // return general_klv_parser(reader, res);
    return string_parser(reader, res);
}

bool parsers::stanag_vmti_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vmti_local_set_parser(reader, res, ctx);
}

bool parsers::stanag_sensor_ellipsoid_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_alternate_platform_ellipsoid_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_operational_mode_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t value = 0;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    switch (value) {
    case 0:
        *res = "Other";
        break;
    case 1:
        *res = "Operational";
        break;
    case 2:
        *res = "Training";
        break;
    case 3:
        *res = "Exercise";
        break;
    case 4:
        *res = "Maintenance";
        break;
    case 5:
        *res = "Test";
        break;
    default:
        *res = "Reserved - Do not use";
        break;
    }
    return true;
}

bool parsers::stanag_frame_center_height_above_ellipsoid_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint16_to_float_parser(reader, res, -900, 19000, -900);
}

bool parsers::stanag_sensor_north_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -327, 327, 0, "Out of Range");
}

bool parsers::stanag_sensor_east_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int16_to_float_reserved_parser(reader, res, -327, 327, 0, "Out of Range");
}

bool parsers::stanag_image_horizon_pixel_pack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto points = nl::json::array();
    // GVariantBuilder builder = G_VARIANT_BUILDER_INIT(G_VARIANT_TYPE("a(dd)"));
    { // first (x,y)(x,y) each uint8, representing percentage 0->100
        uint8_t x = 0;
        uint8_t y = 0;
        float f_x = 0;
        float f_y = 0;
        for (uint8_t i = 0; i < 2; i++) {
            if (!reader.get_ui8(&x)) {
                break;
            }
            if (!reader.get_ui8(&y)) {
                return false;
            }
            if (!deg_uint8_to_float(x, &f_x, 0, 100, 0)) {
                return false;
            }
            if (!deg_uint8_to_float(y, &f_y, 0, 100, 0)) {
                return false;
            }
            points.push_back({f_x, f_y});
        }
    }
    { // second (x,y)(x,y) are optional each int32 converted to angle;
        // TODO: add reserved, ie out of bounds to deg_int32_to_double;
        int32_t x = 0;
        int32_t y = 0;
        double f_x = 0;
        double f_y = 0;
        for (uint8_t i = 0; i < 2; i++) {
            if (!reader.get_i32(&x)) {
                break;
            }
            if (!reader.get_i32(&y)) {
                return false;
            }
            if (!deg_int32_to_double(x, &f_x, -90, 90, 0)) {
                return false;
            }
            if (!deg_int32_to_double(y, &f_y, -180, 180, 0)) {
                return false;
            }
            points.push_back({f_x, f_y});
            // g_variant_builder_add(&builder, "(dd)", deg_int32_to_double(x, -90, 90), deg_int32_to_double(x, -180, 180));
        }
    }
    *res = points;
    return true;
}

bool parsers::stanag_corner_latitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "N/A (Off-Earth)");
}

bool parsers::stanag_corner_longitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "N/A (Off-Earth)");
}

bool parsers::stanag_corner_latitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_longitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_latitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_longitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_latitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_longitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_latitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_latitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_corner_longitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return stanag_corner_longitude_point_parser(reader, res, ctx);
}

bool parsers::stanag_platform_pitch_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Out of Range");
}

bool parsers::stanag_platform_roll_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Out of Range");
}

bool parsers::stanag_platform_angle_of_attack_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -90, 90, 0, "Out of Range");
}

bool parsers::stanag_platform_sideslip_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_int32_to_double_reserved_parser(reader, res, -180, 180, 0, "Out of Range");
}

bool parsers::stanag_miis_core_identifier_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1204
    nl::json _res = nl::json::object();
    uint8_t byte = 0;
    if (!reader.get_ui8(&byte)) {
        return false;
    }
    _res["ber_oid_value"] = byte;
    if (!reader.get_ui8(&byte)) {
        return false;
    }
    // bool reserved_top = (byte >> 7) & 0b1;
    uint8_t sensor_type_id = (byte >> 5) & 0b11;
    _res["sensor_type_id"] = sensor_type_id;
    uint8_t platform_id_type = (byte >> 3) & 0b11;
    _res["platform_id_type"] = platform_id_type;
    bool window_id_type = (byte >> 2) & 0b1;
    _res["window_id_type"] = window_id_type;
    bool minor_id_type = (byte >> 1) & 0b1;
    _res["minor_id_type"] = minor_id_type;
    nl::json physical_sensor_id;
    if (!uuid_parser(reader, &physical_sensor_id)) {
        return false;
    }
    _res["physical_sensor_id"] = physical_sensor_id;
    nl::json virtual_platform_id;
    if (!uuid_parser(reader, &virtual_platform_id)) {
        return false;
    }
    _res["virtual_platform_id"] = virtual_platform_id;
    *res = _res;
    return true; // not implemented
}

bool parsers::stanag_sar_motion_imagery_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1206
    // *res = "not implemented";
    return false; // not implemented
}

bool parsers::stanag_target_width_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint64_to_double_parser(reader, res, 0, 15e5, 0); // 1,500,000
}

bool parsers::stanag_range_image_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1002
    // *res = "not implemented";
    return false; // not implemented
}

bool parsers::stanag_geo_registration_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1601 [19]
    // *res = "not implemented";
    return false; // not implemented
}

bool parsers::stanag_composite_imaging_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1602 [20]
    // *res = "not implemented";
    return false; // not implemented
}

// bool parsers::stanag_segment_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1607 [11]
//     // *res = "not implemented";
//     return false; // not implemented
// }

// bool parsers::stanag_amend_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1607 [11]
//     // *res = "not implemented";
//     return false; // not implemented
// }

bool parsers::stanag_sdcc_flp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: does not parse, only sets; need to view MISB ST 1010 [9]
    // *res = "not implemented";
    return false; // not implemented
}

bool parsers::stanag_density_altitude_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint64_to_double_parser(reader, res, -900, 4e4, -900); // 40,000
}

bool parsers::stanag_sensor_ellipsoid_height_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint64_to_double_parser(reader, res, -900, 4e4, -900); // 40,000
}

bool parsers::stanag_alternate_platform_ellipsoid_height_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint64_to_double_parser(reader, res, -900, 4e4, -900); // 40,000
}

bool parsers::stanag_stream_designator_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_operational_base_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_broadcast_source_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_range_to_recovery_location_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, 0, 21000, 0);
}

bool parsers::stanag_time_airborne_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint32_t _res;
    if (!reader.get_variable_ui32(&_res)) {
        return false;
    }
    *res = _res;
    return true;
}

bool parsers::stanag_propulsion_unit_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint32_t _res;
    if (!reader.get_variable_ui32(&_res)) {
        return false;
    }
    *res = _res;
    return true;
}

bool parsers::stanag_platform_course_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint64_to_double_parser(reader, res, 0, 360, 0);
}

bool parsers::stanag_altitude_agl_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: check issue with docs - wants float64, but max length is 4;
    return deg_variable_uint64_to_double_parser(reader, res, -900, 4e4, -900);                              // 40,000
}

bool parsers::stanag_radar_altimeter_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: check issue with docs - wants float64, but max length is 4;
    return deg_variable_uint64_to_double_parser(reader, res, -900, 4e4, -900);                                 // 40,000
}

bool parsers::stanag_control_command_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint64_t command_id = 0;
    if (!reader.get_ber_oid(&command_id)) {
        return false;
    }
    uint64_t string_size = 0;
    if (!reader.get_ber(&string_size)) {
        return false;
    }
    uint64_t actual_string_size = 0;
    const uint8_t *string_data;
    if (!reader.get_data(&string_data, &actual_string_size)) {
        return false;
    }
    if (actual_string_size < string_size) {
        return false;
    }
    std::string str;
    str.assign((const char *)string_data, string_size);
    uint64_t command_timestamp;
    if (!reader.get_ui64(&command_timestamp)) {
        return false;
    }
    // std::stringstream s;
    // s << command_id << ", " << str << ", " << timestamp_from_uint64(command_timestamp);
    // *res = s.str();
    *res = nl::json({
        {"id", command_id},
        {"timestamp", command_timestamp},
        {"message", str},
    });
    return true;
}

bool parsers::stanag_control_command_verification_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto commands = nl::json::array();
    uint64_t command_id = 0;
    for (uint8_t i = 0;; i++) {
        if (!reader.get_ber_oid(&command_id)) {
            break;
        }
        commands.push_back(command_id);
        // g_variant_builder_add(&builder, "t", command_id);
    }
    *res = commands;
    return true;
}

bool parsers::stanag_sensor_azimuth_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, -1000, 1000, -1000);
}

bool parsers::stanag_sensor_elevation_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, -1000, 1000, -1000);
}

bool parsers::stanag_sensor_roll_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, -1000, 1000, -1000);
}

bool parsers::stanag_onboard_mi_storage_percent_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, 0, 100, 0);
}

bool parsers::stanag_active_wavelength_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto waves = nl::json::array();
    uint64_t wavelength_id = 0;
    for (uint8_t i = 0;; i++) {
        if (!reader.get_ber_oid(&wavelength_id)) {
            break;
        }
        waves.push_back(wavelength_id);
        // g_variant_builder_add(&builder, "t", command_id);
    }
    *res = waves;
    return true;
}

bool parsers::stanag_country_codes_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint64_t length = 0;
    uint8_t codeing_method = 0;
    const uint8_t *data = NULL;
    // GString *overflight_country = NULL;
    std::string overflight_country;
    // GString *operator_country = NULL;
    std::string operator_country;
    // GString *country_of_manufacture = NULL;
    std::string country_of_manufacture;

    if (!reader.get_ber_oid(&length)) {
        return false;
    }
    if (length != 1) {
        return false;
    }
    if (!reader.get_ui8(&codeing_method)) {
        return false;
    }
    if (!reader.get_ber_oid(&length)) {
        return false;
    }
    uint64_t actual_length;
    if (!reader.get_data(&data, &actual_length)) {
        return false;
    }

    overflight_country.assign((const char *)data, length);
    if (reader.get_ber_oid(&length)) {
        if (length > 0) {
            if (!reader.get_data(&data, &actual_length)) {
                return false;
            }
            operator_country.assign((const char *)data, length);
        }
        if (reader.get_ber_oid(&length)) {
            if (length > 0) {
                if (!reader.get_data(&data, &actual_length)) {
                    return false;
                }
                country_of_manufacture.assign((const char *)data, length);
            }
        }
    }
    *res = nl::json({
        {"codeing_method", codeing_method},
        {"overflight_country", overflight_country},
        {"operator_country", operator_country},
        {"country_of_manufacture", country_of_manufacture},
    });
    return true;
}

bool parsers::stanag_number_of_navsats_in_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}

bool parsers::stanag_positioning_method_source_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res); // #TODO: this includes flags, might be able to do a dictionary / list
}

bool parsers::stanag_platform_status_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t value;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    switch (value) {
    case 0:
        *res = "Active";
        break;
    case 1:
        *res = "Pre-flight";
        break;
    case 2:
        *res = "Pre-flight-taxiing";
        break;
    case 3:
        *res = "Run-up";
        break;
    case 4:
        *res = "Take-off";
        break;
    case 5:
        *res = "Ingress";
        break;
    case 6:
        *res = "Manual operation";
        break;
    case 7:
        *res = "Automated-orbit";
        break;
    case 8:
        *res = "Transitioning";
        break;
    case 9:
        *res = "Egress";
        break;
    case 10:
        *res = "Landing";
        break;
    case 11:
        *res = "Landed-taxiing";
        break;
    case 12:
        *res = "Landed-Parked";
        break;
    default:
        *res = "Reserved";
        break;
    }
    return true;
}

bool parsers::stanag_sensor_control_mode_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint8_t value;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    switch (value) {
    case 0:
        *res = "Off";
        break;
    case 1:
        *res = "Home Position";
        break;
    case 2:
        *res = "Uncontrolled";
        break;
    case 3:
        *res = "Manual Control";
        break;
    case 4:
        *res = "Calibrating";
        break;
    case 5:
        *res = "Auto - Holding Position";
        break;
    case 6:
        *res = "Auto - Tracking";
        break;
    default:
        *res = "Reserved";
        break;
    }
    return true;
}

bool parsers::stanag_sensor_frame_rate_pack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint64_t numerator = 0;
    uint64_t denomerator = 0;
    if (!reader.get_ber_oid(&numerator)) {
        return false;
    }
    if (!reader.get_ber_oid(&denomerator)) {
        return false;
    }
    *res = nl::json({numerator, denomerator});
    return true;
}

bool parsers::stanag_wavelengths_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::stanag_target_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_airbase_locations_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::stanag_takeoff_time_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: docs say variable length, assuming length is not variable;
    return timestamp_parser(reader, res);
}

bool parsers::stanag_transmission_frequency_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, 1, 99999, 1);
}

bool parsers::stanag_onboard_mi_storage_capacity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint32_t _res;
    if (!reader.get_variable_ui32(&_res)) {
        return false;
    }
    *res = _res;
    return true;
}

bool parsers::stanag_zoom_percentage_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_variable_uint32_to_float_parser(reader, res, 0, 100, 0);
}

bool parsers::stanag_communications_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}

bool parsers::stanag_leap_seconds_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: could cause issues due to unknown encoding when variable and negative
    int32_t _res = 0;
    if (!reader.get_variable_i32(&_res)) {
        return false;
    }
    *res = _res;
    return true;
}

bool parsers::stanag_correction_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    int64_t _res = 0;
    if (!reader.get_variable_i64(&_res)) {
        return false;
    }
    *res = _res;
    return true;
}

bool parsers::stanag_payload_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    uint64_t payload_count = 0;
    if (!reader.get_ber_oid(&payload_count)) {
        return false;
    }

    uint64_t last_remaining = 0;
    uint64_t remaining = 0;

    nl::json _res = nl::json::array();

    for (uint64_t i = 0;; i++) {
        remaining = reader.get_remaining();
        if (remaining <= 0) {
            break;
        } else if (remaining == last_remaining) {
            CTX_LOG(Level::error, "stuck");
            break;
        }
        last_remaining = remaining;

        uint64_t length = 0;
        if (!reader.get_ber(&length)) {
            break;
        };
        ByteReader payload_reader = reader.splice(length);

        uint64_t payload_id = 0;
        if (!payload_reader.get_ber_oid(&payload_id)) {
            break;
        }

        uint64_t payload_type = 0;
        if (!payload_reader.get_ber_oid(&payload_type)) {
            break;
        }

        std::string payload_type_string = "";
        {
            switch (payload_type) {
            case 0:
                payload_type_string = "Electro Optical MI Sensor";
                break;
            case 1:
                payload_type_string = "LIDAR";
                break;
            case 2:
                payload_type_string = "RADAR";
                break;
            case 3:
                payload_type_string = "SIGINT";
                break;
            case 4:
                payload_type_string = "SAR";
                break;
            default:
                payload_type_string = std::to_string(payload_id);
                break;
            }
        }

        length = 0;
        if (!payload_reader.get_ber(&length)) {
            break;
        };
        nl::json payload_name;
        if (!string_parser(payload_reader, &payload_name)) {
            break;
        }
        _res.push_back({
            {"id", payload_id},
            // {"type", payload_type},
            {"type", payload_type_string},
            {"name", payload_name},
        });
    }
    *res = _res;
    return true;
}

bool parsers::stanag_active_payloads_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::stanag_weapons_stores_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::stanag_waypoint_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::stanag_view_domain_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}
