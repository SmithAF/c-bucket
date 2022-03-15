#include <byteswap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const uint8_t MP4_ATOM_MINIMUM_SIZE = 8;
uint32_t char4ToInt(char name[4]) { return __bswap_32(*((uint32_t *)name)); }

void *copyTo(void *ptr, const void *source) {
  return memcpy(ptr, source, MP4_ATOM_MINIMUM_SIZE);
}

size_t readToMinus8(void *ptr, size_t bytesToRead, FILE *filePointer) {
  return fread(ptr, bytesToRead - MP4_ATOM_MINIMUM_SIZE, 1, filePointer);
};

size_t read8(void *ptr, FILE *filePointer) {
  return fread(ptr, MP4_ATOM_MINIMUM_SIZE, 1, filePointer);
}