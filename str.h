/**
 * @file str.h
 * @author Viktor Fejes (viktor@viktorfejes.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "defines.h"

#include <stdlib.h>

/**
 * @brief Function to calculate string length.
 * 
 * @param str Input string.
 * @return u64 The length of the string.
 */
INLINE u64 str_len(const char* str) {
    u64 len = 0;
    while (str[len] != '\0') { len++; }
    return len;
}

/**
 * @brief Compares two strings using binary comparison.
 * 
 * @param str_a C string to be compared.
 * @param str_b C string to be compared.
 * @return u32 Returns a value indicating the relationship between the strings.
 */
INLINE u32 str_cmp(const char* str_a, const char* str_b) {
    u64 i = 0;
    while (str_a[i] != '\0' && str_b[i] != '\0') {
        if (str_a[i] != str_b[i]) {
            return str_a[i] - str_b[i];
        }
        i++;
    }

    return str_a[i] - str_b[i];
}

/**
 * @brief Copies the C string from `src` to `dst` including the null terminator.
 * 
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src C string to be copied.
 * @return char* dst is returned. 
 */
INLINE char* str_cpy(char* dst, const char* src) {
    u64 i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return dst;
}

/**
 * @brief Returns a pointer to a null-terminated C string, which is a duplicate
 * of the string pointed to by `str`.
 * 
 * @param str Pointer to the C string to duplicate.
 * @return char* Pointer to the newly allocated C string, or NULL.
 */
INLINE char* str_dup(const char* str) {
    u64 len = str_len(str);
    char* new_str = (char*)malloc((len + 1) * sizeof(char));
    if (new_str == NULL) {
        return NULL;
    }
    str_cpy(new_str, str);
    return new_str;
}

/**
 * @brief Appends a copy of the `src` string to the `dst` string.
 * 
 * @param dst Pointer to the destination array. It should be large enough to contain
 * the concatenated resulting string.
 * @param src C string to be appended.
 * @return char* `dst` is returned.
 */
INLINE char* str_cat(char* dst, const char* src) {
    u64 dst_len = str_len(dst);
    u64 i = 0;
    while (src[i] != '\0') {
        dst[dst_len + i] = src[i];
        i++;
    }
    dst[dst_len + i] = '\0';
    return dst;
}