#include <klvparse/stanag/vmti/vtarget/vtracker/VTrackerEncoders.h>
namespace encoders = klvparse::encoders;

bool encoders::vtracker_track_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uuid_encoder(item, res);
}
bool encoders::vtracker_detection_status_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::vtracker_start_timestamp_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return timestamp_encoder(item, res);
}
bool encoders::vtracker_end_timestamp_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return timestamp_encoder(item, res);
}
bool encoders::vtracker_bounding_box_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vtracker_algorithm_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::vtracker_confidence_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return deg_float_to_uint8_encoder(item, res, 0, 1, 0);
}
bool encoders::vtracker_number_of_track_points_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vtracker_locus_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vtracker_velocity_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vtracker_acceleration_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool encoders::vtracker_algorithm_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return variable_uint24_encoder(item, res);
}
