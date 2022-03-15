
#include "./atoms.h"
#include "./helpers.h"
#include <byteswap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define read_byte(dst, fp) fread(&dst, 1, 1, fp)
#define read_bits(dst, nbits, fp)                                              \
  typeof(dst) tmp;                                                             \
  fread(&tmp, sizeof(dst), 1, fp);                                             \
  *dst = __bswap##nbits(tmp)
void printMvhd(const struct MP4_ATOM *box, FILE *fp) {
  uint32_t size = box->size;
  struct MP4_FULL_ATOM *ptr = malloc(size);
  copyTo(ptr, box);
  readToMinus8(&ptr->version, size, fp);
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

void printFtyp(const struct MP4_ATOM *box, FILE *fp) {
  int32_t size = box->size;
  struct FTYP_ATOM *ftyp = malloc(size);
  copyTo(ftyp, box);
  readToMinus8(&ftyp->major_brand, size, fp);
  printf("Found ftyp\n Major Brand: %.4s \n Minor version: %d\n",
         ftyp->major_brand, __bswap_32(ftyp->minor_version));
  printf("Compat: %.*s\n", size - 16, ftyp->compatible_brands);
  free(ftyp);
}

void printTkhd(const struct MP4_ATOM *box, FILE *fp) {
  uint32_t size = box->size;
  char *ptr = malloc(size);
  copyTo(ptr, box);
  fread(&ptr[8], size - 8, 1, fp);
  printf("Found %.4s\n", &ptr[4]);
  printf(" Size: %d\n", ptr[0]);
  printf(" Version: %d\n", ptr[9]);
  /**
   * 0x0000001 Track enabled
   * 0x0000002 Track in movie
   * 0x0000004 Track in preview
   * 0x0000008 Track size is aspect ratio
   */
  uint8_t flags = ptr[11];
  uint8_t trackEnabled = (flags & 0x000001) != 0;
  uint8_t trackInMovie = (flags & 0x000002) != 0;
  uint8_t trackInPreview = (flags & 0x000004) != 0;
  uint8_t trackSizeIsAspectRatio = (flags & 0x000008) != 0;

  printf(" Flags:\n Track Enabled: %d\n", trackEnabled);
  printf(" Track in Movie: %d\n", trackInMovie);
  printf(" Track in Preview: %d\n", trackInPreview);
  printf(" Track size is aspect ratio: %d\n", trackSizeIsAspectRatio);

  // switch (ptr[9]) {
  // case 0:
  //   const struct TKHD_ATOM_V0 *tkhd = ptr;

  // case 1:
  //   const struct TKHD_ATOM_V1 *tkhd = ptr;
  //   break;
  // }

  free(ptr);
}