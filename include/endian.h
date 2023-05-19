#pragma once
/* Definitions for byte order, according to significance of bytes,
   from low addresses to high addresses.  The value is what you get by
   putting '4' in the most significant byte, '3' in the second most
   significant byte, '2' in the second least significant byte, and '1'
   in the least significant byte, and then writing down one digit for
   each byte, starting with the byte at the lowest address at the left,
   and proceeding to the byte with the highest address at the right.  */

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __PDP_ENDIAN 3412

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __LONG_LONG_PAIR(HI, LO) LO, HI
#elif __BYTE_ORDER == __BIG_ENDIAN
#define __LONG_LONG_PAIR(HI, LO) HI, LO
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htobe16(x) __bswap_16(x)
#define htole16(x) (x)
#define be16toh(x) __bswap_16(x)
#define le16toh(x) (x)

#define htobe32(x) __bswap_32(x)
#define htole32(x) (x)
#define be32toh(x) __bswap_32(x)
#define le32toh(x) (x)

#define htobe64(x) __bswap_64(x)
#define htole64(x) (x)
#define be64toh(x) __bswap_64(x)
#define le64toh(x) (x)
#else
#define htobe16(x) (x)
#define htole16(x) __bswap_16(x)
#define be16toh(x) (x)
#define le16toh(x) __bswap_16(x)

#define htobe32(x) (x)
#define htole32(x) __bswap_32(x)
#define be32toh(x) (x)
#define le32toh(x) __bswap_32(x)

#define htobe64(x) (x)
#define htole64(x) __bswap_64(x)
#define be64toh(x) (x)
#define le64toh(x) __bswap_64(x)
#endif
