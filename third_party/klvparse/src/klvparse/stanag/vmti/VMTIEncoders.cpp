#include <klvparse/stanag/vmti/VMTIEncoders.h>
using namespace klvparse;
namespace encoders = klvparse::encoders;

// bool encoders::vmti_checksum_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
//     return uint16_parser(reader, res);
// }
bool encoders::vmti_precision_time_stamp_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return timestamp_encoder(item, res);
}
bool encoders::vmti_system_name_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::vmti_ls_version_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint16_encoder(item, res);
}
bool encoders::vmti_number_detected_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
bool encoders::vmti_number_reported_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
bool encoders::vmti_frame_number_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
bool encoders::vmti_frame_width_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    bool _res = variable_uint24_encoder(item, res);
    if (_res) {
        nl::json temp;
        ByteReader r = *res;
        parsers::vmti_frame_width_parser(r, &temp, ctx); // this is to store frame_width;
        // (*storage.lock().get())[_FRAME_WIDTH] = ;
    }
    return _res;
}
bool encoders::vmti_frame_height_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
bool encoders::vmti_source_sensor_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::vmti_fov_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    if (!item.is_number()) {
        return false;
    }
    float_t source = item.get<float_t>();
    uint16_t target = IMAPB_to_uint16(source, 0, 180, 2);
    return encode_uint16(target, res);
}
bool encoders::vmti_horizontal_fov_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vmti_fov_encoder(item, res);
}
bool encoders::vmti_vertical_fov_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return vmti_fov_encoder(item, res);
}
bool encoders::vmti_miis_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return bytes_encoder(item, res);
}
bool encoders::vmti_target_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    if (!item.is_array()) {
        return false;
    }
    VTarget vtarget_encoder(ctx);
    std::vector<uint8_t> _res;
    // std::vector<uint8_t> tags_bytes;
    std::vector<uint8_t> value_bytes;
    std::vector<uint8_t> length_bytes;
    uint64_t count = 0;
    for (auto const &j : item) {
        if (!vtarget_encoder.encode(j, &value_bytes)) {
            std::cout << "vtarget series: failed to encode vtarget: " << j.dump(-1) << std::endl;
            continue;
        }
        uint64_t l = value_bytes.size();
        if (!encode_ber(l, &length_bytes)) {
            std::cout << "vtarget series: failed to encode vtarget length (" << l << ")" << std::endl;
            continue;
        }
        // tags_bytes.insert(tags_bytes.end(), length_bytes.begin(), length_bytes.end());
        // tags_bytes.insert(tags_bytes.end(), value_bytes.begin(), value_bytes.end());
        _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
        _res.insert(_res.end(), value_bytes.begin(), value_bytes.end());
        count++;
    }
    if (count <= 0) {
        std::cout << "vtarget series: failed to encode any vtarget or vtarget series is empty" << std::endl;
        return false;
    }
    // {
    //     uint64_t l = tags_bytes.size();
    //     if (!encode_ber(l, &length_bytes)) {
    //         std::cout << "vtarget series: failed to encode vtarget series length (" << l << ")" << std::endl;
    //         return false;
    //     }
    //     _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
    //     _res.insert(_res.end(), tags_bytes.begin(), tags_bytes.end());
    // }
    *res = std::move(_res);
    return true;
}
bool encoders::vmti_algorithm_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vmti_ontology_series_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}