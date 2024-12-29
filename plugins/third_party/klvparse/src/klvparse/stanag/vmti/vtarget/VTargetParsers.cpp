#include <klvparse/stanag/vmti/vtarget/VTargetParsers.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;

bool parsers::vtarget_pixels_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint64_t value;
    if (!reader.get_variable_ui48(&value)) {
        return false;
    }
    if (ctx == nullptr) {
        return false;
    }
    auto storage = ctx->get_storage();
    if (storage == nullptr) {
        return false;
    }
    const nl::json &frame_width_json = (*storage)[_FRAME_WIDTH];
    if (!frame_width_json.is_number()) {
        return false;
    }
    uint64_t frame_width = frame_width_json.get<uint64_t>();
    uint64_t x = value % frame_width;
    uint64_t y = value / frame_width;
    nl::json _res({x, y});
    *res = std::move(_res);
    return true;
}
bool parsers::vtarget_lat_long_parser(ByteReader &reader, nl::json *res) {
    uint32_t _temp = 0;
    if (!reader.get_ui24(&_temp)) {
        return false;
    }
    // if (!reader.get_variable_uint24(_temp)) {
    //     return false;
    // }
    float_t _res = IMAPB_to_double(_temp, -19.2, 19.2, 3);
    *res = _res;
    return true;
}
bool parsers::vtarget_center_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_parser(reader, res, ctx);
}
bool parsers::vtarget_top_left_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_parser(reader, res, ctx);
}
bool parsers::vtarget_bottom_right_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_parser(reader, res, ctx);
}
bool parsers::vtarget_priority_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}
bool parsers::vtarget_confidence_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    // return uint8_parser(reader, res);
    return deg_uint8_to_float_parser(reader, res, 0, 1, 0);
}
bool parsers::vtarget_history_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint16_parser(reader, res);
}
bool parsers::vtarget_bbox_fill_percentage_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint8_to_float_parser(reader, res, 0, 1, 0);
}
bool parsers::vtarget_color_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint24_parser(reader, res);
}
bool parsers::vtarget_intensity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
bool parsers::vtarget_latitude_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_longitude_offset_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    uint16_t _temp = 0;
    if (!reader.get_variable_ui16(&_temp)) {
        return false;
    }
    float_t _res = IMAPB_to_float(_temp, -900, 19000, 2);
    *res = _res;
    return true;
}
bool parsers::vtarget_top_left_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_top_left_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_bottom_right_latitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_bottom_right_longitude_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_parser(reader, res);
}
bool parsers::vtarget_location_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_boundary_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_y_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint32_parser(reader, res);
}
bool parsers::vtarget_x_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint32_parser(reader, res);
}
bool parsers::vtarget_fpa_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_mask_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_object_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    // return false; // not implemented
    return vobject_parser(reader, res, ctx);
}
bool parsers::vtarget_feature_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_tracker_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return vtracker_parser(reader, res, ctx);
}
bool parsers::vtarget_chip_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool parsers::vtarget_chip_series_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}