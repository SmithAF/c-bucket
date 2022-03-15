#include <stdint.h>
#include <stdio.h>
uint32_t char4ToInt(char name[4]);
void *copyTo(void *ptr, const void *source);
size_t readToMinus8(void *ptr, size_t bytesToRead, FILE *filePointer);
size_t read8(void *ptr, FILE *filePointer);