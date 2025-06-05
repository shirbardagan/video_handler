#include <klvparse/stanag/vmti/vtarget/vtracker/VTrackerParsers.h>

namespace parsers = klvparse::parsers;

bool parsers::vtracker_track_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uuid_parser(reader, res);
}
bool parsers::vtracker_detection_status_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return uint8_parser(reader, res);
}
bool parsers::vtracker_start_timestamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return timestamp_parser(reader, res);
}
bool parsers::vtracker_end_timestamp_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return timestamp_parser(reader, res);
}
bool parsers::vtracker_bounding_box_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool parsers::vtracker_algorithm_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}
bool parsers::vtracker_confidence_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return deg_uint8_to_float_parser(reader, res, 0, 1, 0);
}
bool parsers::vtracker_number_of_track_points_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool parsers::vtracker_locus_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool parsers::vtracker_velocity_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool parsers::vtracker_acceleration_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool parsers::vtracker_algorithm_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_parser(reader, res);
}
