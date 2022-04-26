#ifndef INCLUDED_IO_PLAIN_READ_WRITE
#define INCLUDED_IO_PLAIN_READ_WRITE

#include "../h-basic.h"

extern void read_byte(FILE *file, byte *target);
extern void read_u16b(FILE *file, u16b *target);
extern void read_s16b(FILE *file, s16b *target);
extern void read_u32b(FILE *file, u32b *target);
extern void read_s32b(FILE *file, s32b *target);
extern void read_string(FILE *file, char *str, int);

extern void write_byte(FILE *file, byte);
extern void write_u16b(FILE *file, u16b);
extern void write_s16b(FILE *file, s16b);
extern void write_u32b(FILE *file, u32b);
extern void write_s32b(FILE *file, s32b);
extern void write_string(FILE *file, cptr);

#endif // INCLUDED_IO_PLAIN_READ_WRITE
