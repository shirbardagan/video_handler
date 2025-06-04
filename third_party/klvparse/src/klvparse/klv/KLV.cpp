#include <klvparse/klv/KLV.h>

KLV::KLV() {
}

KLV::~KLV() {
}

size_t KLV::get_next(ByteReader &reader, PARSE_FORMATS format) {
    size_t next = 0;
    if (format == PARSE_FORMATS::_BER) {
        if (reader.get_ber_next(&next)) {
            return next;
        }
    } else if (format == PARSE_FORMATS::_BER_OID) {
        if (reader.get_ber_oid_next(&next)) {
            return next;
        }
    } else if (((size_t)format) > 0) {
        return (size_t)format;
    }
    return 0;
}

uint64_t KLV::get_content(ByteReader &reader, PARSE_FORMATS format) {
    uint64_t content = 0;
    if (format == PARSE_FORMATS::_BER) {
        if (reader.get_ber(&content)) {
            return content;
        }
    } else if (format == PARSE_FORMATS::_BER_OID) {
        if (reader.get_ber_oid(&content)) {
            return content;
        }
    } else if (((size_t)format) > 0) {
        if (reader.get_by_length(&content, (size_t)format)) {
            return content;
        }
    }
    return 0;
}

KLV KLV::parse(ByteReader &reader, PARSE_FORMATS key, PARSE_FORMATS length) {
    KLV klv;
    size_t next = get_next(reader, key);
    klv.key = reader.splice(next);
    next = get_next(reader, length);
    klv.length = reader.splice(next);
    next = get_content(klv.length, length);
    klv.value = reader.splice(next);
    return klv;
}

std::string KLV::debug() const {
    std::stringstream sstr;
    auto _debug = [](std::stringstream &sstr, const char *name, const ByteReader &r) {
        auto total = r.get_total();
        auto index = r.get_index();
        auto start = r.get_start();
        auto end = r.get_end();
        sstr << "(" << name << ": " << index << "/" << total << ", " << start << "->" << end << ")";
    };
    _debug(sstr, "key", this->key);
    _debug(sstr, "length", this->length);
    _debug(sstr, "value", this->value);
    return sstr.str();
}