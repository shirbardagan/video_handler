#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include <klvparse/klv/KLV.h>
#include <klvparse/stanag/common/JSONParsers.h>
#include <klvparse/stanag/security/Security.h>
#include <klvparse/stanag/vmti/VMTI.h>

#define _UAS_DATALINK_LS "uas_datalink_ls"
#define _FRAME_CENTER_LATITUDE "frame_center_latitude"
#define _FRAME_CENTER_LONGITUDE "frame_center_longitude"

#define USE_FRAME_CENTER_FOR_OFFSET 0

namespace klvparse {

    namespace parsers {

        /**
         *  0x01 - 1 - checksum
         */
        bool stanag_checksum_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x02 - 2 - precision_time_stamp
         */
        bool stanag_precision_time_stamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x03 - 3 - mission_id
         */
        bool stanag_mission_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x04 - 4 - platform_tail_number
         */
        bool stanag_platform_tail_number_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x05 - 5 - platform_heading_angle
         */
        bool stanag_platform_heading_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x06 - 6 - platform_pitch_angle
         */
        bool stanag_platform_pitch_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x07 - 7 - platform_roll_angle
         */
        bool stanag_platform_roll_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x08 - 8 - platform_true_airspeed
         */
        bool stanag_platform_true_airspeed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x09 - 9 - platform_indicated_airspeed
         */
        bool stanag_platform_indicated_airspeed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0A - 10 - platform_designation
         */
        bool stanag_platform_designation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0B - 11 - image_source_sensor
         */
        bool stanag_image_source_sensor_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0C - 12 - image_coordinate_system
         */
        bool stanag_image_coordinate_system_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0D - 13 - sensor_latitude
         */
        bool stanag_sensor_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0E - 14 - sensor_longitude
         */
        bool stanag_sensor_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x0F - 15 - sensor_true_altitude
         */
        bool stanag_sensor_true_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x10 - 16 - sensor_horizontal_field_of_view
         */
        bool stanag_sensor_horizontal_field_of_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x11 - 17 - sensor_vertical_field_of_view
         */
        bool stanag_sensor_vertical_field_of_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x12 - 18 - sensor_relative_azimuth_angle
         */
        bool stanag_sensor_relative_azimuth_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x13 - 19 - sensor_relative_elevation_angle
         */
        bool stanag_sensor_relative_elevation_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x14 - 20 - sensor_relative_roll_angle
         */
        bool stanag_sensor_relative_roll_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x15 - 21 - slant_range
         */
        bool stanag_slant_range_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x16 - 22 - target_width
         */
        bool stanag_target_width_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x17 - 23 - frame_center_latitude
         */
        bool stanag_frame_center_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x18 - 24 - frame_center_longitude
         */
        bool stanag_frame_center_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x19 - 25 - frame_center_elevation
         */
        bool stanag_frame_center_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        bool stanag_offset_corner_latitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        bool stanag_offset_corner_longitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1A - 26 - offset_corner_latitude_point_1
         */
        bool stanag_offset_corner_latitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1B - 27 - offset_corner_longitude_point_1
         */
        bool stanag_offset_corner_longitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1C - 28 - offset_corner_latitude_point_2
         */
        bool stanag_offset_corner_latitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1D - 29 - offset_corner_longitude_point_2
         */
        bool stanag_offset_corner_longitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1E - 30 - offset_corner_latitude_point_3
         */
        bool stanag_offset_corner_latitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x1F - 31 - offset_corner_longitude_point_3
         */
        bool stanag_offset_corner_longitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x20 - 32 - offset_corner_latitude_point_4
         */
        bool stanag_offset_corner_latitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x21 - 33 - offset_corner_longitude_point_4
         */
        bool stanag_offset_corner_longitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x22 - 34 - icing_detected
         */
        bool stanag_icing_detected_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x23 - 35 - wind_direction
         */
        bool stanag_wind_direction_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x24 - 36 - wind_speed
         */
        bool stanag_wind_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x25 - 37 - static_pressure
         */
        bool stanag_static_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x26 - 38 - density_altitude
         */
        bool stanag_density_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x27 - 39 - outside_air_temperature
         */
        bool stanag_outside_air_temperature_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x28 - 40 - target_location_latitude
         */
        bool stanag_target_location_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x29 - 41 - target_location_longitude
         */
        bool stanag_target_location_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2A - 42 - target_location_elevation
         */
        bool stanag_target_location_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2B - 43 - target_track_gate_width
         */
        bool stanag_target_track_gate_width_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2C - 44 - target_track_gate_height
         */
        bool stanag_target_track_gate_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2D - 45 - target_error_estimate_ce90
         */
        bool stanag_target_error_estimate_ce90_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2E - 46 - target_error_estimate_le90
         */
        bool stanag_target_error_estimate_le90_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x2F - 47 - generic_flag_data
         */
        bool stanag_generic_flag_data_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x30 - 48 - security_local_set
         */
        bool stanag_security_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x31 - 49 - differential_pressure
         */
        bool stanag_differential_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x32 - 50 - platform_angle_of_attack
         */
        bool stanag_platform_angle_of_attack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x33 - 51 - platform_vertical_speed
         */
        bool stanag_platform_vertical_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x34 - 52 - platform_sideslip_angle
         */
        bool stanag_platform_sideslip_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x35 - 53 - airfield_barometric_pressure
         */
        bool stanag_airfield_barometric_pressure_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x36 - 54 - airfield_elevation
         */
        bool stanag_airfield_elevation_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x37 - 55 - relative_humidity
         */
        bool stanag_relative_humidity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x38 - 56 - platform_ground_speed
         */
        bool stanag_platform_ground_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x39 - 57 - ground_range
         */
        bool stanag_ground_range_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3A - 58 - platform_fuel_remaining
         */
        bool stanag_platform_fuel_remaining_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3B - 59 - platform_call_sign
         */
        bool stanag_platform_call_sign_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3C - 60 - weapon_load
         */
        bool stanag_weapon_load_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3D - 61 - weapon_fired
         */
        bool stanag_weapon_fired_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3E - 62 - laser_prf_code
         */
        bool stanag_laser_prf_code_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x3F - 63 - sensor_field_of_view_name
         */
        bool stanag_sensor_field_of_view_name_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x40 - 64 - platform_magnetic_heading
         */
        bool stanag_platform_magnetic_heading_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x41 - 65 - uas_datalink_ls_version_number
         */
        bool stanag_uas_datalink_ls_version_number_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x42 - 66 - deprecated
         */
        bool stanag_deprecated_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x42 - 66 - target_location_covariance_matrix
         */
        bool stanag_target_location_covariance_matrix_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x43 - 67 - alternate_platform_latitude
         */
        bool stanag_alternate_platform_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x44 - 68 - alternate_platform_longitude
         */
        bool stanag_alternate_platform_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x45 - 69 - alternate_platform_altitude
         */
        bool stanag_alternate_platform_altitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x46 - 70 - alternate_platform_name
         */
        bool stanag_alternate_platform_name_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x47 - 71 - alternate_platform_heading
         */
        bool stanag_alternate_platform_heading_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x48 - 72 - event_start_time_utc
         */
        bool stanag_event_start_time_utc_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x49 - 73 - rvt_local_set
         */
        bool stanag_rvt_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4A - 74 - vmti_local_set
         */
        bool stanag_vmti_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4B - 75 - sensor_ellipsoid_height
         */
        bool stanag_sensor_ellipsoid_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4C - 76 - alternate_platform_ellipsoid_height
         */
        bool stanag_alternate_platform_ellipsoid_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4D - 77 - operational_mode
         */
        bool stanag_operational_mode_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4E - 78 - frame_center_height_above_ellipsoid
         */
        bool stanag_frame_center_height_above_ellipsoid_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x4F - 79 - sensor_north_velocity
         */
        bool stanag_sensor_north_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x50 - 80 - sensor_east_velocity
         */
        bool stanag_sensor_east_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x51 - 81 - image_horizon_pixel_pack
         */
        bool stanag_image_horizon_pixel_pack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        bool stanag_corner_latitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);
        bool stanag_corner_longitude_point_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x52 - 82 - corner_latitude_point_1
         */
        bool stanag_corner_latitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x53 - 83 - corner_longitude_point_1
         */
        bool stanag_corner_longitude_point_1_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x54 - 84 - corner_latitude_point_2
         */
        bool stanag_corner_latitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x55 - 85 - corner_longitude_point_2
         */
        bool stanag_corner_longitude_point_2_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x56 - 86 - corner_latitude_point_3
         */
        bool stanag_corner_latitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x57 - 87 - corner_longitude_point_3
         */
        bool stanag_corner_longitude_point_3_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x58 - 88 - corner_latitude_point_4
         */
        bool stanag_corner_latitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x59 - 89 - corner_longitude_point_4
         */
        bool stanag_corner_longitude_point_4_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5A - 90 - platform_pitch_angle_full
         */
        bool stanag_platform_pitch_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5B - 91 - platform_roll_angle_full
         */
        bool stanag_platform_roll_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5C - 92 - platform_angle_of_attack_full
         */
        bool stanag_platform_angle_of_attack_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5D - 93 - platform_sideslip_angle_full
         */
        bool stanag_platform_sideslip_angle_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5E - 94 - miis_core_identifier
         */
        bool stanag_miis_core_identifier_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x5F - 95 - sar_motion_imagery_local_set
         */
        bool stanag_sar_motion_imagery_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x60 - 96 - target_width_extended
         */
        bool stanag_target_width_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x61 - 97 - range_image_local_set
         */
        bool stanag_range_image_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x62 - 98 - geo_registration_local_set
         */
        bool stanag_geo_registration_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x63 - 99 - composite_imaging_local_set
         */
        bool stanag_composite_imaging_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x64 - 100 - segment_local_set
         */
        // bool stanag_segment_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x65 - 101 - amend_local_set
         */
        // bool stanag_amend_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x66 - 102 - sdcc_flp
         */
        bool stanag_sdcc_flp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x67 - 103 - density_altitude_extended
         */
        bool stanag_density_altitude_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x68 - 104 - sensor_ellipsoid_height_extended
         */
        bool stanag_sensor_ellipsoid_height_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x69 - 105 - alternate_platform_ellipsoid_height_extended
         */
        bool stanag_alternate_platform_ellipsoid_height_extended_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6A - 106 - stream_designator
         */
        bool stanag_stream_designator_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6B - 107 - operational_base
         */
        bool stanag_operational_base_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6C - 108 - broadcast_source
         */
        bool stanag_broadcast_source_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6D - 109 - range_to_recovery_location
         */
        bool stanag_range_to_recovery_location_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6E - 110 - time_airborne
         */
        bool stanag_time_airborne_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x6F - 111 - propulsion_unit_speed
         */
        bool stanag_propulsion_unit_speed_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x70 - 112 - platform_course_angle
         */
        bool stanag_platform_course_angle_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x71 - 113 - altitude_agl
         */
        bool stanag_altitude_agl_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x72 - 114 - radar_altimeter
         */
        bool stanag_radar_altimeter_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x73 - 115 - control_command
         */
        bool stanag_control_command_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x74 - 116 - control_command_verification_list
         */
        bool stanag_control_command_verification_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x75 - 117 - sensor_azimuth_rate
         */
        bool stanag_sensor_azimuth_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x76 - 118 - sensor_elevation_rate
         */
        bool stanag_sensor_elevation_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x77 - 119 - sensor_roll_rate
         */
        bool stanag_sensor_roll_rate_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x78 - 120 - onboard_mi_storage_percent_full
         */
        bool stanag_onboard_mi_storage_percent_full_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x79 - 121 - active_wavelength_list
         */
        bool stanag_active_wavelength_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7A - 122 - country_codes
         */
        bool stanag_country_codes_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7B - 123 - number_of_navsats_in_view
         */
        bool stanag_number_of_navsats_in_view_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7C - 124 - positioning_method_source
         */
        bool stanag_positioning_method_source_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7D - 125 - platform_status
         */
        bool stanag_platform_status_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7E - 126 - sensor_control_mode
         */
        bool stanag_sensor_control_mode_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x7F - 127 - sensor_frame_rate_pack
         */
        bool stanag_sensor_frame_rate_pack_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x80 - 128 - wavelengths_list
         */
        bool stanag_wavelengths_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x81 - 129 - target_id
         */
        bool stanag_target_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x82 - 130 - airbase_locations
         */
        bool stanag_airbase_locations_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x83 - 131 - takeoff_time
         */
        bool stanag_takeoff_time_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x84 - 132 - transmission_frequency
         */
        bool stanag_transmission_frequency_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x85 - 133 - onboard_mi_storage_capacity
         */
        bool stanag_onboard_mi_storage_capacity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x86 - 134 - zoom_percentage
         */
        bool stanag_zoom_percentage_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x87 - 135 - communications_method
         */
        bool stanag_communications_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x88 - 136 - leap_seconds
         */
        bool stanag_leap_seconds_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x89 - 137 - correction_offset
         */
        bool stanag_correction_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x8A - 138 - payload_list
         */
        bool stanag_payload_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x8B - 139 - active_payloads
         */
        bool stanag_active_payloads_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x8C - 140 - weapons_stores
         */
        bool stanag_weapons_stores_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x8D - 141 - waypoint_list
         */
        bool stanag_waypoint_list_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

        /**
         *  0x8E - 142 - view_domain
         */
        bool stanag_view_domain_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx);

    } // namespace parsers
} // namespace klvparse