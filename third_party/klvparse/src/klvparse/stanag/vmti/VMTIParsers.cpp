#include <klvparse/stanag/vmti/VMTIParsers.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;

bool parsers::vmti_checksum_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint16_parser(reader, res);
}
bool parsers::vmti_precision_time_stamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return timestamp_parser(reader, res);
}
bool parsers::vmti_system_name_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}
bool parsers::vmti_ls_version_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint16_parser(reader, res);
}
bool parsers::vmti_number_detected_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
bool parsers::vmti_number_reported_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
bool parsers::vmti_frame_number_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
bool parsers::vmti_frame_width_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    auto _res = variable_uint24_parser(reader, res);
    if (_res) {
        if (ctx != nullptr) {
            auto storage = ctx->get_storage();
            if (storage != nullptr) {
                (*storage)[_FRAME_WIDTH] = *res;
            }
        }
    }
    return _res;
}
bool parsers::vmti_frame_height_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
bool parsers::vmti_source_sensor_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}
bool parsers::fov_parser(ByteReader &reader, nl::json *res) {
    uint16_t _temp = 0;
    if (!reader.get_ui16(&_temp)) {
        return false;
    }
    float_t _res = IMAPB_to_float(_temp, 0, 180, 2);
    *res = _res;
    return true;
}

bool parsers::vmti_horizontal_fov_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return fov_parser(reader, res);
}

bool parsers::vmti_vertical_fov_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return fov_parser(reader, res);
}

bool parsers::vmti_miis_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return bytes_parser(reader, res);
}
bool parsers::vmti_target_series_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    KLV klv;
    VTarget vtarget_parser(ctx);
    nl::json _res(nl::json::array());
    nl::json item;
    uint64_t count = 0;
    for (; reader.get_remaining() > 0; count++) {
        klv = KLV::parse(reader, PARSE_FORMATS::_0, PARSE_FORMATS::_BER);
        if (!vtarget_parser.parse(klv.value, &item)) {
            continue;
        }
        try {
            _res.push_back(item);
        } catch (std::exception &e) {

            std::cout << e.what() << std::endl;
            std::cout << item.dump() << std::endl;
        }
    }
    if (count <= 0) {
        return false;
    }
    *res = std::move(_res);
    return true;
}

bool parsers::vmti_algorithm_series_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}

bool parsers::vmti_ontology_series_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) { // #TODO: not implemented
    return false;
}