#include <klvparse/byte/ByteReader.h>

ByteReader::ByteReader() {
}

ByteReader::ByteReader(std::vector<uint8_t> buffer, size_t index, size_t start, size_t end) {
    update(buffer, index, start, end);
}

ByteReader::ByteReader(const uint8_t *buffer, size_t size, size_t index, size_t start, size_t end) {
    update(buffer, size, index, start, end);
}

void ByteReader::update(const uint8_t *buffer, size_t size, size_t index, size_t start, size_t end) {
    this->buffer = buffer;
    this->size = size;
    set_start(start);
    set_end(end);
    set_index(index);
}

void ByteReader::update(std::vector<uint8_t> buffer, size_t index, size_t start, size_t end) {
    this->_buffer = buffer;
    this->buffer = _buffer.data();
    this->size = _buffer.size();
    set_start(start);
    set_end(end);
    set_index(index);
}

template <typename T>
static T add_no_overflow(T x, T y) {
    T res = x + y;
    res |= -(res < x);
    return res;
}

template uint64_t add_no_overflow(uint64_t x, uint64_t y);
template uint32_t add_no_overflow(uint32_t x, uint32_t y);
template uint16_t add_no_overflow(uint16_t x, uint16_t y);
template uint8_t add_no_overflow(uint8_t x, uint8_t y);

template <typename T>
static T clamp(T val, T min, T max) {
    return val < min   ? min
           : val > max ? max
                       : val;
}

ByteReader::~ByteReader() {}

size_t ByteReader::get_start() const {
    return this->start;
}

void ByteReader::set_start(size_t start) {
    this->start = clamp(start, (size_t)0, size);
}

size_t ByteReader::get_end() const {
    return this->end;
}

void ByteReader::set_end(size_t end) {
    this->end = clamp(end, start, size);
}

size_t ByteReader::get_index() const {
    return this->index;
}

void ByteReader::set_index(size_t index) {
    this->index = clamp(index, (size_t)0, get_total());
}

uint8_t ByteReader::at(size_t i) const {
    return this->buffer[this->start + i];
}

uint8_t ByteReader::operator[](size_t i) {
    return this->at(i);
}

size_t ByteReader::get_size() const {
    return this->size;
}

size_t ByteReader::get_total() const {
    return this->end - this->start;
}

size_t ByteReader::_get_remaining(size_t index) const {
    return get_total() - index;
}

size_t ByteReader::get_remaining() const {
    return _get_remaining(this->index);
}

void ByteReader::restart() {
    return this->set_index(0);
}

void ByteReader::skip(size_t next) {
    // set_index(add_no_overflow(this->index, next));
    set_index(this->index + next);
}

template <typename T, size_t T_size>
bool ByteReader::get_simple(T *result) {
    if (get_remaining() < T_size) {
        return false;
    }
    T _result = *((T *)(this->buffer + this->start + this->index));
    (*result) = _result;
    this->index += T_size;
    return true;
}

template <typename T>
bool ByteReader::get_by_length(T *result, size_t length) {
    if (get_remaining() < length) {
        return false;
    }
    T mask = le64toh((1ul << (length * 8)) - 1);
    T _result = *((T *)(this->buffer + this->start + this->index));
    *result = _result & mask;
    this->index += length;
    return true;
}

template bool ByteReader::get_by_length(uint64_t *result, size_t length);
template bool ByteReader::get_by_length(uint32_t *result, size_t length);
template bool ByteReader::get_by_length(uint16_t *result, size_t length);

template <typename T>
bool ByteReader::get_max(T *result, size_t length, size_t *actual) {
    auto remaining = get_remaining();
    size_t _actual = length < remaining ? length : remaining;
    T mask = le64toh((1ul << (_actual * 8)) - 1);
    T _result = *((T *)(this->buffer + this->start + this->index));
    *result = _result & mask;
    if (actual != NULL) {
        *actual = _actual;
    }
    this->index += _actual;
    return true;
}

template bool ByteReader::get_max(uint64_t *result, size_t length, size_t *actual);
template bool ByteReader::get_max(uint32_t *result, size_t length, size_t *actual);
template bool ByteReader::get_max(uint16_t *result, size_t length, size_t *actual);

bool ByteReader::get_ui8(uint8_t *result) {
    return get_simple<uint8_t, 1>(result);
}

bool ByteReader::get_ui16(uint16_t *result) {
    bool ret = get_simple<uint16_t, 2>(result);
    if (ret) {
        *result = htobe16(*result);
    }
    return ret;
}

bool ByteReader::get_ui24(uint32_t *result) {
    bool ret = get_by_length<uint32_t>(result, 3);
    if (ret) {
        *result = htobe24(*result);
    }
    return ret;
}

bool ByteReader::get_ui32(uint32_t *result) {
    bool ret = get_simple<uint32_t, 4>(result);
    if (ret) {
        *result = htobe32(*result);
    }
    return ret;
}

bool ByteReader::get_ui40(uint64_t *result) {
    bool ret = get_by_length<uint64_t>(result, 5);
    if (ret) {
        *result = htobe40(*result);
    }
    return ret;
}

bool ByteReader::get_ui48(uint64_t *result) {
    bool ret = get_by_length<uint64_t>(result, 6);
    if (ret) {
        *result = htobe48(*result);
    }
    return ret;
}

bool ByteReader::get_ui56(uint64_t *result) {
    bool ret = get_by_length<uint64_t>(result, 7);
    if (ret) {
        *result = htobe56(*result);
    }
    return ret;
}

bool ByteReader::get_ui64(uint64_t *result) {
    bool ret = get_simple<uint64_t, 8>(result);
    if (ret) {
        *result = htobe64(*result);
    }
    return ret;
}

uint8_t ByteReader::ui8() {
    uint8_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui8(&result)) {
        throw std::runtime_error("get ui8 failed");
    };
#else
    get_ui8(&result);
#endif
    return result;
}
uint16_t ByteReader::ui16() {
    uint16_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui16(&result)) {
        throw std::runtime_error("get ui16 failed");
    };
#else
    get_ui16(&result);
#endif
    return result;
}
uint32_t ByteReader::ui24() {
    uint32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui24(&result)) {
        throw std::runtime_error("get ui24 failed");
    };
#else
    get_ui24(&result);
#endif
    return result;
}
uint32_t ByteReader::ui32() {
    uint32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui32(&result)) {
        throw std::runtime_error("get ui32 failed");
    };
#else
    get_ui32(&result);
#endif
    return result;
}
uint64_t ByteReader::ui40() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui40(&result)) {
        throw std::runtime_error("get ui40 failed");
    };
#else
    get_ui40(&result);
#endif
    return result;
}
uint64_t ByteReader::ui48() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui48(&result)) {
        throw std::runtime_error("get ui48 failed");
    };
#else
    get_ui48(&result);
#endif
    return result;
}
uint64_t ByteReader::ui56() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui56(&result)) {
        throw std::runtime_error("get ui56 failed");
    };
#else
    get_ui56(&result);
#endif
    return result;
}
uint64_t ByteReader::ui64() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ui64(&result)) {
        throw std::runtime_error("get ui64 failed");
    };
#else
    get_ui64(&result);
#endif
    return result;
}

bool ByteReader::get_variable_ui8(uint8_t *result, size_t *actual) {
    return get_max<uint8_t>(result, 1, actual);
}

bool ByteReader::get_variable_ui16(uint16_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint16_t>(result, 2, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui24(uint32_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint32_t>(result, 3, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui32(uint32_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint32_t>(result, 4, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        } else if (_actual == 4) {
            *result = htobe32(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui40(uint64_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint64_t>(result, 5, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        } else if (_actual == 4) {
            *result = htobe32(*result);
        } else if (_actual == 5) {
            *result = htobe40(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui48(uint64_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint64_t>(result, 6, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        } else if (_actual == 4) {
            *result = htobe32(*result);
        } else if (_actual == 5) {
            *result = htobe40(*result);
        } else if (_actual == 6) {
            *result = htobe48(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui56(uint64_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint64_t>(result, 7, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        } else if (_actual == 4) {
            *result = htobe32(*result);
        } else if (_actual == 5) {
            *result = htobe40(*result);
        } else if (_actual == 6) {
            *result = htobe48(*result);
        } else if (_actual == 7) {
            *result = htobe56(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_variable_ui64(uint64_t *result, size_t *actual) {
    size_t _actual = 0;
    bool ret = get_max<uint64_t>(result, 8, &_actual);
    if (ret) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if (_actual == 2) {
            *result = htobe16(*result);
        } else if (_actual == 3) {
            *result = htobe24(*result);
        } else if (_actual == 4) {
            *result = htobe32(*result);
        } else if (_actual == 5) {
            *result = htobe40(*result);
        } else if (_actual == 6) {
            *result = htobe48(*result);
        } else if (_actual == 7) {
            *result = htobe56(*result);
        } else if (_actual == 8) {
            *result = htobe64(*result);
        }
#endif
        if (actual) {
            *actual = _actual;
        }
    }
    return ret;
}

bool ByteReader::get_i8(int8_t *result) {
    return get_ui8((uint8_t *)result);
}

bool ByteReader::get_i16(int16_t *result) {
    return get_ui16((uint16_t *)result);
}

bool ByteReader::get_i24(int32_t *result) {
    return get_ui24((uint32_t *)result);
}

bool ByteReader::get_i32(int32_t *result) {
    return get_ui32((uint32_t *)result);
}

bool ByteReader::get_i40(int64_t *result) {
    return get_ui40((uint64_t *)result);
}

bool ByteReader::get_i48(int64_t *result) {
    return get_ui48((uint64_t *)result);
}

bool ByteReader::get_i56(int64_t *result) {
    return get_ui56((uint64_t *)result);
}

bool ByteReader::get_i64(int64_t *result) {
    return get_ui64((uint64_t *)result);
}

int8_t ByteReader::i8() {
    int8_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i8(&result)) {
        throw std::runtime_error("get i8 failed");
    }
#else
    get_i8(&result);
#endif
    return result;
}
int16_t ByteReader::i16() {
    int16_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i16(&result)) {
        throw std::runtime_error("get i16 failed");
    }
#else
    get_i16(&result);
#endif
    return result;
}
int32_t ByteReader::i24() {
    int32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i24(&result)) {
        throw std::runtime_error("get i24 failed");
    }
#else
    get_i24(&result);
#endif
    return result;
}
int32_t ByteReader::i32() {
    int32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i32(&result)) {
        throw std::runtime_error("get i32 failed");
    }
#else
    get_i32(&result);
#endif
    return result;
}
int64_t ByteReader::i40() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i40(&result)) {
        throw std::runtime_error("get i40 failed");
    }
#else
    get_i40(&result);
#endif
    return result;
}
int64_t ByteReader::i48() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i48(&result)) {
        throw std::runtime_error("get i48 failed");
    }
#else
    get_i48(&result);
#endif
    return result;
}
int64_t ByteReader::i56() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i56(&result)) {
        throw std::runtime_error("get i56 failed");
    }
#else
    get_i56(&result);
#endif
    return result;
}
int64_t ByteReader::i64() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_i64(&result)) {
        throw std::runtime_error("get i64 failed");
    }
#else
    get_i64(&result);
#endif
    return result;
}

bool ByteReader::get_variable_i8(int8_t *result, size_t *actual) {
    return get_variable_ui8((uint8_t *)result, actual);
}

bool ByteReader::get_variable_i16(int16_t *result, size_t *actual) {
    return get_variable_ui16((uint16_t *)result, actual);
}

bool ByteReader::get_variable_i24(int32_t *result, size_t *actual) {
    return get_variable_ui24((uint32_t *)result, actual);
}

bool ByteReader::get_variable_i32(int32_t *result, size_t *actual) {
    return get_variable_ui32((uint32_t *)result, actual);
}

bool ByteReader::get_variable_i40(int64_t *result, size_t *actual) {
    return get_variable_ui40((uint64_t *)result, actual);
}

bool ByteReader::get_variable_i48(int64_t *result, size_t *actual) {
    return get_variable_ui48((uint64_t *)result, actual);
}

bool ByteReader::get_variable_i56(int64_t *result, size_t *actual) {
    return get_variable_ui56((uint64_t *)result, actual);
}

bool ByteReader::get_variable_i64(int64_t *result, size_t *actual) {
    return get_variable_ui64((uint64_t *)result, actual);
}

int8_t ByteReader::vi8() {
    int8_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i8(&result)) {
        throw std::runtime_error("get i8 failed");
    }
#else
    get_variable_i8(&result, NULL);
#endif
    return result;
}
int16_t ByteReader::vi16() {
    int16_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i16(&result)) {
        throw std::runtime_error("get i16 failed");
    }
#else
    get_variable_i16(&result, NULL);
#endif
    return result;
}
int32_t ByteReader::vi24() {
    int32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i24(&result)) {
        throw std::runtime_error("get i24 failed");
    }
#else
    get_variable_i24(&result, NULL);
#endif
    return result;
}
int32_t ByteReader::vi32() {
    int32_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i32(&result)) {
        throw std::runtime_error("get i32 failed");
    }
#else
    get_variable_i32(&result, NULL);
#endif
    return result;
}
int64_t ByteReader::vi40() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i40(&result)) {
        throw std::runtime_error("get i40 failed");
    }
#else
    get_variable_i40(&result, NULL);
#endif
    return result;
}
int64_t ByteReader::vi48() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i48(&result)) {
        throw std::runtime_error("get i48 failed");
    }
#else
    get_variable_i48(&result, NULL);
#endif
    return result;
}
int64_t ByteReader::vi56() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i56(&result)) {
        throw std::runtime_error("get i56 failed");
    }
#else
    get_variable_i56(&result, NULL);
#endif
    return result;
}
int64_t ByteReader::vi64() {
    int64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_variable_i64(&result)) {
        throw std::runtime_error("get i64 failed");
    }
#else
    get_variable_i64(&result, NULL);
#endif
    return result;
}

bool ByteReader::get_ber(uint64_t *result) {
    size_t prev = this->index;
    uint8_t byte = 0;
    if (!get_ui8(&byte)) {
        return false;
    }
    if (!(byte & 0x80)) {
        *result = byte;
        return true;
    }
    uint8_t length = byte & 0x7f;
    if (get_remaining() < length) {
        this->index = prev;
        return false;
    }
    if (length > 8) {
        this->index = prev;
        return false;
    }
    uint64_t _result = 0;
    for (uint8_t i = 0; i < length; i++) {
        if (!get_ui8(&byte)) {
            this->index = prev;
            return false;
        }
        _result = (_result << 8) | byte;
    }
    *result = _result;
    return true;
}

bool ByteReader::get_ber_oid(uint64_t *result) {
    size_t prev = this->index;
    uint8_t byte = 0;
    uint64_t _result = 0;
    for (uint8_t i = 0;; i++) {
        if (i > 8) {
            // std::cout << "[ERROR] failed to parse ber-oid, size=" << i << " too big for uint64" << std::endl;
            this->index = prev;
            return false;
        }
        if (!get_ui8(&byte)) {
            return false;
        }
        _result = _result << 7 | (0x7f & byte);
        if (!(byte & 0x80))
            break;
    }
    *result = _result;
    return true;
}

uint64_t ByteReader::ber() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ber(&result)) {
        throw std::runtime_error("get ber failed");
    }
#else
    get_ber(&result);
#endif
    return result;
}

uint64_t ByteReader::ber_oid() {
    uint64_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ber_oid(&result)) {
        throw std::runtime_error("get ber_oid failed");
    }
#else
    get_ber_oid(&result);
#endif
    return result;
}

bool ByteReader::get_ber_next(size_t *next) {
    size_t _next = this->index;
    uint8_t byte;
    if (_get_remaining(_next) < 1) {
        return false;
    }
    byte = this->at(_next);
    _next += 1;
    if (!(byte & 0x80)) {
        *next = _next - this->index;
        return true;
    }
    uint8_t length = byte & 0x7f;
    if (_get_remaining(_next) < length) {
        return false;
    }
    _next += length;
    *next = _next - this->index;
    return true;
}

bool ByteReader::get_ber_oid_next(size_t *next) {
    uint8_t byte = 0;
    auto _next = this->index;
    for (uint8_t i = 0;; i++) {
        if (_get_remaining(_next) < 1) {
            break;
        }
        byte = this->at(_next);
        _next += 1;
        if (!(byte & 0x80))
            break;
    }
    *next = _next - this->index;
    return true;
}

size_t ByteReader::ber_next() {
    size_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ber_next(&result)) {
        throw std::runtime_error("get ber_next failed");
    }
#else
    get_ber_next(&result);
#endif
    return result;
}

size_t ByteReader::ber_oid_next() {
    size_t result = 0;
#ifdef BYTE_READER_SHOULD_THROW
    if (!get_ber_oid_next(&result)) {
        throw std::runtime_error("get ber_oid_next failed");
    }
#else
    get_ber_oid_next(&next);
#endif
    return result;
}

ByteReader ByteReader::splice(size_t next) {
    size_t start = this->start + this->index;
    skip(next);
    // return ByteReader(this->buffer, this->size, 0, i, add_no_overflow(i, next));
    return ByteReader(this->buffer, this->size, 0, start, this->start + this->index);
}

ByteReader ByteReader::copy() {
    return ByteReader(this->buffer, this->size, this->index, this->start, this->end);
}

std::string ByteReader::dump() const {
    return hexdump(this->buffer + this->start, this->end - this->start);
}

std::string ByteReader::debug() {
    std::stringstream sstr;
    auto total = get_total();
    auto index = get_index();
    auto start = get_start();
    auto end = get_end();
    sstr << "(" << index << "/" << total << ", " << start << "->" << end << ")";
    return sstr.str();
}

bool ByteReader::get_data(const uint8_t **result, uint64_t *size) {
    // const uint8_t *_res = &this->buffer.begin()[this->start + this->index];
    const uint8_t *_result = this->buffer + this->start + this->index;
    const uint64_t _size = get_remaining();
    *result = this->buffer + this->start + this->index;
    *size = get_remaining();
    return true;
}

// bool ByteReader::get_data(const uint8_t **res, uint64_t *size) {
//     // const uint8_t *_res = &this->buffer.begin()[this->start + this->index];
//     const uint8_t *_res = &this->buffer[this->start + this->index];
//     const uint64_t _size = this->end - this->start - this->index;
//     *res = _res;
//     *size = _size;
//     return true;
// }