// "License": Public Domain
// I, Mathias Panzenböck, place this file hereby into the public domain. Use it at your own risk for whatever you like.
// In case there are jurisdictions that don't support putting things in the public domain you can also consider it to
// be "dual licensed" under the BSD, MIT and Apache licenses, if you want to. This code is trivial anyway. Consider it
// an example on how to get the endian conversion functions on different platforms.

#ifndef PORTABLE_ENDIAN_H__
#define PORTABLE_ENDIAN_H__

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#include <endian.h>

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#elif defined(__OpenBSD__)

#include <endian.h>

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#include <sys/endian.h>

#define be16toh(x) betoh16(x)
#define le16toh(x) letoh16(x)

#define be32toh(x) betoh32(x)
#define le32toh(x) letoh32(x)

#define be64toh(x) betoh64(x)
#define le64toh(x) letoh64(x)

#elif defined(__WINDOWS__)

#include <winsock2.h>
#ifdef __GNUC__
#include <sys/param.h>
#endif

#if BYTE_ORDER == LITTLE_ENDIAN

#define htobe16(x) htons(x)
#define htole16(x) (x)
#define be16toh(x) ntohs(x)
#define le16toh(x) (x)

#define htobe32(x) htonl(x)
#define htole32(x) (x)
#define be32toh(x) ntohl(x)
#define le32toh(x) (x)

#define htobe64(x) htonll(x)
#define htole64(x) (x)
#define be64toh(x) ntohll(x)
#define le64toh(x) (x)

#elif BYTE_ORDER == BIG_ENDIAN

/* that would be xbox 360 */
#define htobe16(x) (x)
#define htole16(x) __builtin_bswap16(x)
#define be16toh(x) (x)
#define le16toh(x) __builtin_bswap16(x)

#define htobe32(x) (x)
#define htole32(x) __builtin_bswap32(x)
#define be32toh(x) (x)
#define le32toh(x) __builtin_bswap32(x)

#define htobe64(x) (x)
#define htole64(x) __builtin_bswap64(x)
#define be64toh(x) (x)
#define le64toh(x) __builtin_bswap64(x)

#else

#error byte order not supported

#endif

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#elif defined(__QNXNTO__)

#include <gulliver.h>

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __PDP_ENDIAN 3412

#if defined(__BIGENDIAN__)

#define __BYTE_ORDER __BIG_ENDIAN

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)

#define htole16(x) ENDIAN_SWAP16(x)
#define htole32(x) ENDIAN_SWAP32(x)
#define htole64(x) ENDIAN_SWAP64(x)

#elif defined(__LITTLEENDIAN__)

#define __BYTE_ORDER __LITTLE_ENDIAN

#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)

#define htobe16(x) ENDIAN_SWAP16(x)
#define htobe32(x) ENDIAN_SWAP32(x)
#define htobe64(x) ENDIAN_SWAP64(x)

#else

#error byte order not supported

#endif

#define be16toh(x) ENDIAN_BE16(x)
#define be32toh(x) ENDIAN_BE32(x)
#define be64toh(x) ENDIAN_BE64(x)
#define le16toh(x) ENDIAN_LE16(x)
#define le32toh(x) ENDIAN_LE32(x)
#define le64toh(x) ENDIAN_LE64(x)

#else

#error platform not supported

#endif

#endif

#ifndef __bswap_constant_24
#define __bswap_constant_24(x) \
    ((((x)&0xff0000u) >> 16) | \
     (((x)&0x00ff00u)) |       \
     (((x)&0x0000ffu) << 16))
#endif

#ifndef __bswap_constant_40
#define __bswap_constant_40(x)       \
    ((((x)&0xff00000000ull) >> 32) | \
     (((x)&0x00ff000000ull) >> 16) | \
     (((x)&0x0000ff0000ull)) |       \
     (((x)&0x000000ff00ull) << 16) | \
     (((x)&0x00000000ffull) << 32))
#endif

#ifndef __bswap_constant_48
#define __bswap_constant_48(x)         \
    ((((x)&0xff0000000000ull) >> 40) | \
     (((x)&0x00ff00000000ull) >> 24) | \
     (((x)&0x0000ff000000ull) >> 8) |  \
     (((x)&0x000000ff0000ull) << 8) |  \
     (((x)&0x00000000ff00ull) << 24) | \
     (((x)&0x0000000000ffull) << 40))
#endif

#ifndef __bswap_constant_56
#define __bswap_constant_56(x)           \
    ((((x)&0xff000000000000ull) >> 48) | \
     (((x)&0x00ff0000000000ull) >> 24) | \
     (((x)&0x0000ff00000000ull) >> 16) | \
     (((x)&0x000000ff000000ull)) |       \
     (((x)&0x00000000ff0000ull) << 16) | \
     (((x)&0x0000000000ff00ull) << 24) | \
     (((x)&0x000000000000ffull) << 48))
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define htobe24(x) __bswap_constant_24(x)
#define htole24(x) __uint32_identity(x)
#define be24toh(x) __bswap_constant_24(x)
#define le24toh(x) __uint32_identity(x)

#define htobe40(x) __bswap_constant_40(x)
#define htole40(x) __uint64_identity(x)
#define be40toh(x) __bswap_constant_40(x)
#define le40toh(x) __uint64_identity(x)

#define htobe48(x) __bswap_constant_48(x)
#define htole48(x) __uint64_identity(x)
#define be48toh(x) __bswap_constant_48(x)
#define le48toh(x) __uint64_identity(x)

#define htobe56(x) __bswap_constant_56(x)
#define htole56(x) __uint64_identity(x)
#define be56toh(x) __bswap_constant_56(x)
#define le56toh(x) __uint64_identity(x)

#else

#define htobe24(x) __uint32_identity(x)
#define htole24(x) __bswap_constant_24(x)
#define be24toh(x) __uint32_identity(x)
#define le24toh(x) __bswap_constant_24(x)

#define htobe40(x) __uint64_identity(x)
#define htole40(x) __bswap_constant_40(x)
#define be40toh(x) __uint64_identity(x)
#define le40toh(x) __bswap_constant_40(x)

#define htobe48(x) __uint64_identity(x)
#define htole48(x) __bswap_constant_48(x)
#define be48toh(x) __uint64_identity(x)
#define le48toh(x) __bswap_constant_48(x)

#define htobe56(x) __uint64_identity(x)
#define htole56(x) __bswap_constant_56(x)
#define be56toh(x) __uint64_identity(x)
#define le56toh(x) __bswap_constant_56(x)

#endif