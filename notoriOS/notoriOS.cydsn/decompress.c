#define COPY_SIZE 16

/* === Firmware Decompression === */

static void file_copy(FS_FILE *file, uint32_t len, int32_t offset) {
  uint32_t end = FS_FTell(file) + len;
  for (uint8_t i = 0; i < len; i += COPY_SIZE) {
    uint8_t[COPY_SIZE] buffer = {0};
    FS_FSeek(file, -offset, FS_SEEK_CUR);
    uint32_t read = FS_FRead(&buffer, 1, COPY_SIZE, file);
    FS_FSeek(file, offset - read, FS_SEEK_CUR);
    FS_FWrite(&buffer, 1, COPY_SIZE, file);
  }
  FS_FSeek(file, end, FS_SEEK_SET);
  FS_SetEndOfFile(file);
}

static void decompress(FS_FILE *out, uint8_t *in) {
  uint32_t len = *(uint32_t*)in & 0x7fffffff; in += 4;
  for (uint8_t *end = in + len, s = 0; in != end;) {
    uint32_t run = *in >> 4, ref = (*in & 0xf) + 4;
    if (run == 0xf) do { s = *in++; run += s; } while (s == 0xff)
    FS_FWrite(in, 1, run, out); in += run;
    int32_t offset = *in++; offset |= *in++ << 8;
    if (ref == 0xf) do { s = *in++; ref += s; } while (s == 0xff)
    file_copy(out, ref, offset);
  }
}
