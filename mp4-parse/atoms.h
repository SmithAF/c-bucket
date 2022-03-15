#include <stdint.h>
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

#define TKHD_DATA                                                              \
  uint32_t reserved2[2];                                                       \
  int16_t layer;                                                               \
  int16_t alternate_group;                                                     \
  int16_t volume;                                                              \
  uint16_t reserved3;                                                          \
  int32_t matrix[9];                                                           \
  uint32_t width;                                                              \
  uint32_t height;

struct TKHD_ATOM_V0 {
  FULL_BOX;
  uint32_t creation_time;
  uint32_t modification_time;
  uint32_t track_ID;
  uint32_t reserved;
  uint32_t duration;
  TKHD_DATA;
};

struct TKHD_ATOM_V1 {
  FULL_BOX;
  uint64_t creation_time;
  uint64_t modification_time;
  uint32_t track_ID;
  uint32_t reserved;
  uint64_t duration;
  TKHD_DATA;
};

struct FTYP_ATOM {
  BOX;
  char major_brand[4];
  uint32_t minor_version;
  char compatible_brands[];
};
