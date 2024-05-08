#pragma once

#include <stdio.h>

#define true  1
#define false 0

#define REQUIRE(statement)                                                                         \
    if (statement != true) {                                                                       \
        printf("--> Expected statement to be TRUE. File: %s:%d\n", __FILE__, __LINE__);            \
        return false;                                                                              \
    }

#define REQUIRE_FAIL(statement)                                                                    \
    if (statement == true) {                                                                       \
        printf("--> Expected statement to be FALSE. File: %s:%d\n", __FILE__, __LINE__);           \
        return false;                                                                              \
    }

#define REQUIRE_TO_BE(expected, actual)                                                            \
    if (actual != expected) {                                                                      \
        printf("--> Expected %lld, but got: %lld. File: %s:%d\n",                                  \
               expected,                                                                           \
               actual,                                                                             \
               __FILE__,                                                                           \
               __LINE__);                                                                          \
        return false;                                                                              \
    }