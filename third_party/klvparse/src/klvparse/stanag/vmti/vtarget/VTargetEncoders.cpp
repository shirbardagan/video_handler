#include <klvparse/stanag/vmti/vtarget/VTargetEncoders.h>

using namespace klvparse;
namespace encoders = klvparse::encoders;

bool encoders::vtarget_pixels_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_array()) {
        return false;
    }
    if (item.size() < 2) {
        return false;
    }
    const nl::json &x_json = item[0];
    if (!x_json.is_number()) {
        return false;
    }
    uint64_t x = x_json.get<uint64_t>();
    const nl::json &y_json = item[1];
    if (!y_json.is_number()) {
        return false;
    }
    uint64_t y = y_json.get<uint64_t>();
    // std::cout << "storage: " << (*storage.lock().get()).dump(2) << std::endl;
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
    uint64_t _res = x + (y * frame_width);
    return encode_variable_uint24((uint32_t)_res, res);
}

bool encoders::vtarget_lat_long_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    if (!item.is_number()) {
        return false;
    }
    float_t source = item.get<float_t>();
    uint32_t target = IMAPB_to_uint32(source, -19.2, 19.2, 3);
    // if (bytes_needed(target) > 3) {
    //     return false;
    // }
    return encode_variable_uint24(target, res);
}

bool encoders::vtarget_center_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_encoder(item, res, ctx);
}
bool encoders::vtarget_top_left_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_encoder(item, res, ctx);
}
bool encoders::vtarget_bottom_right_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_pixels_encoder(item, res, ctx);
}
bool encoders::vtarget_priority_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::vtarget_confidence_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    // return uint8_encoder(item, res);
    return deg_float_to_uint8_encoder(item, res, 0, 1, 0);
}
bool encoders::vtarget_history_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint16_encoder(item, res);
}
bool encoders::vtarget_bbox_fill_percentage_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint8_encoder(item, res, 0, 1, 0);
}
bool encoders::vtarget_color_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint24_encoder(item, res);
}
bool encoders::vtarget_intensity_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
bool encoders::vtarget_latitude_offset_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_longitude_offset_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_number()) {
        return false;
    }
    float_t source = item.get<float_t>();
    uint16_t target = IMAPB_to_uint16(source, -900, 19000, 2);
    return encode_variable_uint16(target, res);
}
bool encoders::vtarget_top_left_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_top_left_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_bottom_right_latitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_bottom_right_longitude_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtarget_lat_long_encoder(item, res);
}
bool encoders::vtarget_location_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_boundary_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_y_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint32_encoder(item, res);
}
bool encoders::vtarget_x_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint32_encoder(item, res);
}
bool encoders::vtarget_fpa_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_mask_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_object_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vobject_encoder(item, res, ctx);
}
bool encoders::vtarget_feature_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_tracker_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vtracker_encoder(item, res, ctx);
}
bool encoders::vtarget_chip_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
bool encoders::vtarget_chip_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false; // not implemented
}
