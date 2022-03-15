#include "./atoms.h"
#include "./helpers.h"
#include "./print.h"
#include <byteswap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char const *argv[]) {
  // Open file pointer
  FILE *fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    perror("Error while opening the file.\n");
    return EXIT_FAILURE;
  }
  // // allocate memory to read the first atom of the mp4
  struct MP4_ATOM box;

  while (read8(&box, fp)) {
    box.size = __bswap_32(box.size);
    switch (char4ToInt(box.name)) {
    case 'ftyp': {
      printFtyp(&box, fp);
      break;
    };
    case 'mvhd':
      printMvhd(&box, fp);
      break;
    case 'tkhd':
      printTkhd(&box, fp);
      break;
    // No need to bother parsing these atoms at this time
    case 'udta':
    case 'minf':
    case 'mdia':
    case 'edts':
    case 'mdat':
    case 'free':
      printf("Found atom: %.4s\n -> Skipping to next atom\n", box.name);
      fseek(fp, box.size - 8, SEEK_CUR);
      break;
    // Atoms with themselves but contain child atoms that do
    case 'moov':
    case 'trak':
    case 'stbl':
    case 'dinf':
      printf("Found atom: %.4s\n -> Checking next 8 bytes\n", box.name);
      break;
    default:
      break;
    }
  }

  fclose(fp);
  fp = NULL;

  return EXIT_SUCCESS;
}