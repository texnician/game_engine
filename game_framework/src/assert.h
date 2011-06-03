#ifndef _ASSERT_H_
#define _ASSERT_H_

#if defined(__linux__)
#include <assert.h>
#define ASSERT(expr) assert((expr))
#endif

#endif
