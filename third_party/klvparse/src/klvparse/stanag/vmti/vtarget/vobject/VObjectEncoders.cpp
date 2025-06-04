#include <klvparse/stanag/vmti/vtarget/vobject/VObjectEncoders.h>
namespace encoders = klvparse::encoders;

bool encoders::vobject_ontology_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::vobject_class_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
