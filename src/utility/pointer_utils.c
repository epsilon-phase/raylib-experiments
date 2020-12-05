#include "./pointer_utils.h"
const void *find_biggest(void *list,
                         const void *(*comparison)(const void *, const void *),
                         size_t nmembers, size_t size) {
  const void *biggest = list;
  for (void *other = list + size; other < list + nmembers * size;
       other += size) {
    biggest = comparison(biggest, other);
  }
  return biggest;
}