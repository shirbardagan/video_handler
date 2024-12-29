#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <klvparse/byte/ByteReader.h>
#include <klvparse/byte/IMAP.h>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

enum class PARSE_FORMATS
{
    _ = -99,
    _BER = -2,
    _BER_OID = -1,
    _0 = 0,
    _1 = 1,
    _2 = 2,
    _4 = 4,
    _16 = 16,
};
// using json = nlohmann::json;

class KLV
{
public:
    KLV();
    virtual ~KLV();
    ByteReader key;
    ByteReader length;
    ByteReader value;

    static KLV parse(ByteReader &reader, PARSE_FORMATS key, PARSE_FORMATS len);
    std::string debug() const;

    static size_t get_next(ByteReader &reader, PARSE_FORMATS format);
    static uint64_t get_content(ByteReader &reader, PARSE_FORMATS format);
};
