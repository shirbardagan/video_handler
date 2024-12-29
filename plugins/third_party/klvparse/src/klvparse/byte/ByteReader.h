#pragma once

#include <cstdint>
#include <klvparse/byte/endian_wrap.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <klvparse/byte/HexDump.h>

#define BYTE_READER_SHOULD_THROW

#define __GET_BASIC_H(type_name, type, num_bytes) \
    bool get_##type_name(type &res);              \
    bool peek_##type_name(type &res);             \
    bool move_##type_name();

#define __GET_VARIABLE_H(type_name, type, num_bytes) \
    bool get_variable_##type_name(type &value, uint8_t *length = NULL);

#define __GET_VARIABLE_BE_H(type_name, type, inter_type, num_bytes) \
    bool get_variable_##type_name(type &value, uint8_t *length = NULL);

#define __GET_BE_SIMPLE_H(type_name, type, num_bytes) \
    bool get_##type_name##_be(type &value);

#define __GET_BE_COMPLEX_H(type_name, type, num_bytes, container_bytes) \
    bool get_##type_name##_be(type &value);

class ByteReader
{
private:
    std::vector<uint8_t> _buffer;
    const uint8_t *buffer = NULL;
    size_t size = 0;
    size_t index = 0;
    size_t start = 0;
    size_t end = 0;

    template <typename T, size_t T_size>
    bool get_simple(T *value);

    size_t _get_remaining(size_t index) const;

public:
    ByteReader();
    ByteReader(std::vector<uint8_t> buffer, size_t index = 0, size_t start = 0, size_t end = SIZE_MAX);
    ByteReader(const uint8_t *buffer, size_t size, size_t index = 0, size_t start = 0, size_t end = SIZE_MAX);

    void update(std::vector<uint8_t> buffer, size_t index = 0, size_t start = 0, size_t end = SIZE_MAX);
    void update(const uint8_t *buffer, size_t size, size_t index = 0, size_t start = 0, size_t end = SIZE_MAX);

    size_t get_start() const;
    void set_start(size_t start);

    size_t get_end() const;
    void set_end(size_t end);

    size_t get_index() const;
    void set_index(size_t index);

    uint8_t at(size_t i) const;
    uint8_t operator[](size_t i);

    size_t get_size() const;
    size_t get_total() const;
    size_t get_remaining() const;
    void restart();
    void skip(size_t next);
    ByteReader splice(size_t next = SIZE_MAX);
    ByteReader copy();
    std::string dump() const;
    std::string debug();

    ~ByteReader();

    bool get_ui8(uint8_t *result);
    bool get_ui16(uint16_t *result);
    bool get_ui24(uint32_t *result);
    bool get_ui32(uint32_t *result);
    bool get_ui40(uint64_t *result);
    bool get_ui48(uint64_t *result);
    bool get_ui56(uint64_t *result);
    bool get_ui64(uint64_t *result);

    uint8_t ui8();
    uint16_t ui16();
    uint32_t ui24();
    uint32_t ui32();
    uint64_t ui40();
    uint64_t ui48();
    uint64_t ui56();
    uint64_t ui64();

    bool get_variable_ui8(uint8_t *result, size_t *actual = NULL);
    bool get_variable_ui16(uint16_t *result, size_t *actual = NULL);
    bool get_variable_ui24(uint32_t *result, size_t *actual = NULL);
    bool get_variable_ui32(uint32_t *result, size_t *actual = NULL);
    bool get_variable_ui40(uint64_t *result, size_t *actual = NULL);
    bool get_variable_ui48(uint64_t *result, size_t *actual = NULL);
    bool get_variable_ui56(uint64_t *result, size_t *actual = NULL);
    bool get_variable_ui64(uint64_t *result, size_t *actual = NULL);

    bool get_i8(int8_t *result);
    bool get_i16(int16_t *result);
    bool get_i24(int32_t *result);
    bool get_i32(int32_t *result);
    bool get_i40(int64_t *result);
    bool get_i48(int64_t *result);
    bool get_i56(int64_t *result);
    bool get_i64(int64_t *result);

    int8_t i8();
    int16_t i16();
    int32_t i24();
    int32_t i32();
    int64_t i40();
    int64_t i48();
    int64_t i56();
    int64_t i64();

    bool get_variable_i8(int8_t *result, size_t *actual = NULL);
    bool get_variable_i16(int16_t *result, size_t *actual = NULL);
    bool get_variable_i24(int32_t *result, size_t *actual = NULL);
    bool get_variable_i32(int32_t *result, size_t *actual = NULL);
    bool get_variable_i40(int64_t *result, size_t *actual = NULL);
    bool get_variable_i48(int64_t *result, size_t *actual = NULL);
    bool get_variable_i56(int64_t *result, size_t *actual = NULL);
    bool get_variable_i64(int64_t *result, size_t *actual = NULL);

    int8_t vi8();
    int16_t vi16();
    int32_t vi24();
    int32_t vi32();
    int64_t vi40();
    int64_t vi48();
    int64_t vi56();
    int64_t vi64();

    template <typename T>
    bool get_by_length(T *value, size_t length);

    template <typename T>
    bool get_max(T *value, size_t length, size_t *actual = NULL);

    bool get_ber(uint64_t *result);
    bool get_ber_oid(uint64_t *result);
    
    uint64_t ber();
    uint64_t ber_oid();

    bool get_ber_next(size_t *next);
    bool get_ber_oid_next(size_t *next);

    size_t ber_next();
    size_t ber_oid_next();

    bool get_data(const uint8_t **result, uint64_t *size);
};
