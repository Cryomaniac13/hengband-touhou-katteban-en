#include "../angband.h"

#include "io-plain-read-write.h"

byte get_byte(FILE *file)
{
    byte c;

    c = getc(file) & 0xFF;

    return c;
}

void read_byte(FILE *file, byte *target)
{
   *target = get_byte(file);
}

void read_u16b(FILE *file, u16b *target)
{
    *target = get_byte(file);
    *target |= ((u16b)(get_byte(file)) << 8);
}

void read_s16b(FILE *file, s16b *target)
{
    read_u16b(file, (u16b*)target);
}

void read_u32b(FILE *file, u32b *target)
{
    *target = get_byte(file);
    *target |= ((u32b)(get_byte(file)) << 8);
    *target |= ((u32b)(get_byte(file)) << 16);
    *target |= ((u32b)(get_byte(file)) << 24);
}

void read_s32b(FILE *file, s32b *target)
{
    read_u32b(file, (u32b*)target);
}

void read_string(FILE *file, char *str, int max_len)
{
    int i;

    for (i = 0; TRUE; i++)
    {
        byte tmp8u;

        read_byte(file, &tmp8u);

        if (i < max_len) str[i] = tmp8u;
        if (!tmp8u) break;
    }

    str[max_len - 1] = '\0';


#ifdef JP
	/* Convert Kanji code */
	switch (kanji_code)
	{
#ifdef SJIS
	case 2:
		/* EUC to SJIS */
		euc2sjis(str);
		break;
#endif

#ifdef EUC
	case 3:
		/* SJIS to EUC */
		sjis2euc(str);
		break;
#endif

	case 0:
	{
		/* 不明の漢字コードからシステムの漢字コードに変換 */
		byte code = codeconv(str);

		/* 漢字コードが判明したら、それを記録 */
		if (code) kanji_code = code;

		break;
	}
	default:
		/* No conversion needed */
		break;
	}
#endif
}

void write_byte(FILE *file, byte v)
{
    putc((int)v, file);
}

void write_u16b(FILE *file, u16b v)
{
    write_byte(file, (byte)(v & 0xFF));
    write_byte(file, (byte)((v >> 8) & 0xFF));
}

void write_s16b(FILE *file, s16b v)
{
    write_u16b(file, (u16b)v);
}

void write_u32b(FILE *file, u32b v)
{
    write_byte(file, (byte)(v & 0xFF));
    write_byte(file, (byte)((v >> 8)  & 0xFF));
    write_byte(file, (byte)((v >> 16) & 0xFF));
    write_byte(file, (byte)((v >> 24) & 0xFF));
}

void write_s32b(FILE *file, s32b v)
{
    write_u32b(file, (u32b)v);
}

void write_string(FILE *file, cptr str)
{
    while (*str)
    {
        write_byte(file, *str);
        str++;
    }
    write_byte(file, *str);
}
