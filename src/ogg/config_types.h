#pragma once

#include <stdio.h>
#include <stddef.h>
#include <kernel.h>

typedef int16_t ogg_int16_t;
typedef u_int16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef u_int32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;
typedef u_int64_t ogg_uint64_t;

#define _ogg_malloc  k_malloc
#define _ogg_calloc  k_calloc
#define _ogg_realloc k_realloc_self
#define _ogg_free    k_free