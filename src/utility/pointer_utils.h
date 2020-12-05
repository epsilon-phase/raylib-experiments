#pragma once
#include <stddef.h>
const void *find_biggest(void *list,
                         const void *(*comparison)(const void *, const void *),
                         size_t nmembers, size_t size);