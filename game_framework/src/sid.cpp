#include <ctype.h>
#include "define.h"
#include "sid.h"

const char* const sid::WILDCARD_STR = "*";

sid::sid(const char* const str)
    : id_(hash_name(str)),
      str_(str)
{}

sid_t sid::hash_name(const char* str)
{
    sid_t BASE = 65521;

    // NMAX is the largest n such that
    // 255n(n+1)/2 + (n+1)(BASE+1) <= 2^32-1
    sid_t NMAX = 5552;

#define DO1(buf,i) {s1 += tolower(buf[i]); s2 += s1;}
#define DO2(buf,i) DO1(buf,i); DO1(buf,i+1)
#define DO4(buf,i) DO2(buf,i); DO2(buf,i+2)
#define DO8(buf,i) DO4(buf,i); DO4(buf,i+4)
#define DO16(buf) DO8(buf,0); DO8(buf,8)

    if (0 == str) {
        return 0;
    }

    if (strcmp(str, WILDCARD_STR) == 0) {
        return 0;
    }

    sid_t s1 = 0;
    sid_t s2 = 0;

    for (size_t len = strlen(str); len > 0;) {
        sid_t k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(str);
            str += 16;
            k -= 16;
        }

        if (k != 0) do {
            s1 += *str++;
            s2 += s1;
        } while (--k);

        s1 %= BASE;
        s2 %= BASE;
    }

#if defined(_MSC_VER_)
#pragma warning(push)
#pragma warning(disable : 4312)
#endif

    return (s2 << 16) | s1;

#if defined(_MSC_VER_)
#pragma warning(pop)
#endif
        
#undef DO1
#undef DO2
#undef DO4
#undef DO8
#undef DO16
}
