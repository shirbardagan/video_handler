#include <klvparse/stanag/vmti/vtarget/vobject/VObjectParsers.h>

namespace parsers = klvparse::parsers;

bool parsers::vobject_ontology_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}
bool parsers::vobject_class_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    return string_parser(reader, res);
}