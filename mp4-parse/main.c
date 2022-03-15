#include <byteswap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct MP4_ATOM {
//   uint32_t size;
//   char name[4];
// };
const uint8_t MP4_ATOM_MINIMUM_SIZE = 8;

#define BOX                                                                    \
  uint32_t size;                                                               \
  char name[4];
#define FULL_BOX                                                               \
  BOX;                                                                         \
  uint8_t version;                                                             \
  uint8_t flags[3];
#define ATOM(name)                                                             \
  struct name {                                                                \
    BOX;                                                                       \
  };
#define FULL_ATOM(name)                                                        \
  struct name {                                                                \
    FULL_BOX;                                                                  \
  }
FULL_ATOM(MP4_FULL_ATOM);
ATOM(MP4_ATOM);
ATOM(TRAK_ATOM);
ATOM(EDTS_ATOM);
ATOM(ELST_ATOM);
ATOM(FREE_ATOM);
ATOM(HDLR_ATOM);
ATOM(ILST_ATOM);
ATOM(MDAT_ATOM);
ATOM(MDIA_ATOM);
ATOM(META_ATOM);
ATOM(MOOV_ATOM);
ATOM(TKHD_ATOM);
ATOM(UDTA_ATOM);

#define MVHD_DATA                                                              \
  int32_t rate;                                                                \
  int16_t volume;                                                              \
  const uint16_t reserverd;                                                    \
  const uint32_t reserverd2[2];                                                \
  uint32_t matrix[9];                                                          \
  uint32_t pre_defined[6];                                                     \
  uint32_t next_track_ID;

struct MVHD_ATOM_V0 {
  FULL_BOX;
  uint32_t creation_time;
  uint32_t modification_time;
  uint32_t timescale;
  uint32_t duration;
  MVHD_DATA;
};
struct MVHD_ATOM_V1 {
  FULL_BOX;
  uint64_t creation_time;
  uint64_t modification_time;
  uint32_t timescale;
  uint64_t duration;
  MVHD_DATA;
};

struct FTYP_ATOM {
  BOX;
  char major_brand[4];
  uint32_t minor_version;
  char compatible_brands[];
};

static uint32_t char4ToInt(char name[4]) {
  return __bswap_32(*((uint32_t *)name));
}

static void *copyTo(void *ptr, const void *source) {
  return memcpy(ptr, source, MP4_ATOM_MINIMUM_SIZE);
}

static size_t readTo(void *ptr, size_t bytesToRead, FILE *filePointer) {
  return fread(ptr, bytesToRead - MP4_ATOM_MINIMUM_SIZE, 1, filePointer);
};

static size_t read8(void *ptr, FILE *filePointer) {
  return fread(ptr, MP4_ATOM_MINIMUM_SIZE, 1, filePointer);
}

static void printFtyp(const struct MP4_ATOM *box, FILE *fp) {
  int32_t size = box->size;
  struct FTYP_ATOM *ftyp = malloc(size);
  copyTo(ftyp, box);
  readTo(&ftyp->major_brand, size, fp);
  printf("Found ftyp\n Major Brand: %.4s \n Minor version: %d\n",
         ftyp->major_brand, __bswap_32(ftyp->minor_version));
  printf("Compat: %.*s\n", size - 16, ftyp->compatible_brands);
  free(ftyp);
}

static void printMvhd(const struct MP4_ATOM *box, FILE *fp) {
  int32_t size = box->size;
  struct MP4_FULL_ATOM *ptr = malloc(size);
  copyTo(ptr, box);
  readTo(&ptr->version, size, fp);
  printf("Found MVHD\n");
  switch (ptr->version) {
  case 0: {
    struct MVHD_ATOM_V0 *mvhd = ((struct MVHD_ATOM_V0 *)ptr);
    printf(" Version: %d\n Timescale: %d\n Rate: %d\n Duration: %d\n",
           mvhd->version, bswap_32(mvhd->timescale), bswap_32(mvhd->rate),
           bswap_32(mvhd->duration));

    break;
  }
  case 1: {
    struct MVHD_ATOM_V1 *mvhd = ((struct MVHD_ATOM_V1 *)ptr);
    printf(" Version: %d\n Timescale: %d\n Rate: %d\n Duration: %ld\n",
           mvhd->version, bswap_32(mvhd->timescale), bswap_32(mvhd->rate),
           bswap_64(mvhd->duration));
    break;
  }
  }
  free(ptr);
}

int main(int argc, char const *argv[]) {
  // Open file pointer
  FILE *fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    perror("Error while opening the file.\n");
    return 1;
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
    case 'udta':
    case 'minf':
    case 'mdia':
    case 'edts':
    case 'mdat':
    case 'free':
      printf("Found atom: %.4s\n Skipping to next atom\n", box.name);
      fseek(fp, box.size - MP4_ATOM_MINIMUM_SIZE, SEEK_CUR);
      break;
    case 'moov':
    case 'trak':
    case 'stbl':
    case 'dinf':
      printf("Found atom: %.4s\n. Checking next 8 bytes\n", box.name);
      break;
    default:
      break;
    }
  }

  fclose(fp);
  fp = NULL;

  return EXIT_SUCCESS;
}