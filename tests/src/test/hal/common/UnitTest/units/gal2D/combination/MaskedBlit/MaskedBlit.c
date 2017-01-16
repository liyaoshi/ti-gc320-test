/****************************************************************************
*
*    Copyright 2012 - 2015 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/


/*
 *  Feature:
 *  API:
 *  Check:
 */
#include <galUtil.h>

static gctCONST_STRING s_CaseDescription = \
"Case gal2DMaskedBlit\n" \
"Operation: Mono Stream Applications.\n" \
"2D API: gcoSURF_Blit gcoSURF_MonoBlit\n" \
"Src: Size        [800x600]\n"\
"     Rect        [configurable]\n"\
"     Format      [ARGB8888/INDEX1]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"     Transparency[masked]\n" \
"Dst: Size        [configurable]\n"\
"     Rect        [configurable]\n"\
"     Format      [configurable]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Brush: [None]\n"\
"StreamPack: [UNPACKED/PACKED]\n" \
"Alphablend: [disable]\n" \
"HW feature dependency: ";

typedef struct Test2D {
    GalTest     base;
    GalRuntime  *runtime;

	// destination surface
    gcoSURF			dstSurf;
	gceSURF_FORMAT	dstFormat;
	gctUINT			dstWidth;
	gctUINT			dstHeight;
	gctINT			dstStride;
	gctUINT32		dstPhyAddr;
	gctPOINTER		dstLgcAddr;

	gcoSURF bitmap;

	gctUINT8_PTR maskUnpacked;
	gctUINT8_PTR maskPacked8;
	gctUINT8_PTR maskPacked16;
	gctUINT8_PTR maskPacked32;
    gctUINT32 maskWidth;
	gctUINT32 maskHeight;

	gctUINT font;
} Test2D;

typedef struct _gcsFONTCHAR		* gcsFONTCHAR_PTR;
typedef struct _gcsFONTRANGE	* gcsFONTRANGE_PTR;
typedef struct _gcsFONT			* gcsFONT_PTR;

typedef struct _gcsFONTCHAR
{
	gctINT baseOffset;
	gctINT x;
	gctINT y;
	gctINT width;
	gctINT height;
}
gcsFONTCHAR;

typedef struct _gcsFONTRANGE
{
	char from;
	char to;
	gcsFONTCHAR_PTR font;
}
gcsFONTRANGE;

typedef struct _gcsFONT
{
	gcsFONTRANGE_PTR font;
	gctUINT count;
	gctUINT spacing;
}
gcsFONT;

gcsFONTCHAR g_SmallFont_Special1[] =
{
	{0, 756, 35, 4, 1},		/* space */
};

gcsFONTCHAR g_SmallFont_Special2[] =
{
	{ 2, 744, 35, 2, 4},		/* , */
	{-3, 750, 35, 4, 3},		/* - */
	{ 0, 737, 35, 2, 3},		/* . */
};

gcsFONTCHAR g_SmallFont_Numbers[] =
{
	{0, 626, 35, 4, 11},	/* 0 */
	{0, 636, 35, 5, 11},	/* 1 */
	{0, 644, 35, 5, 11},	/* 2 */
	{0, 654, 35, 6, 11},	/* 3 */
	{0, 664, 35, 5, 11},	/* 4 */
	{0, 674, 35, 5, 11},	/* 5 */
	{0, 684, 35, 5, 11},	/* 6 */
	{0, 694, 35, 5, 11},	/* 7 */
	{0, 704, 35, 5, 11},	/* 8 */
	{0, 714, 35, 5, 11},	/* 9 */
};

gcsFONTCHAR g_SmallFont_LowerCase[] =
{
	{0, 356, 35,  6,  9},	/* a */
	{0, 365, 35,  6, 11},	/* b */
	{0, 375, 35,  4,  9},	/* c */
	{0, 384, 35,  6, 11},	/* d */
	{0, 394, 35,  4,  9},	/* e */
	{0, 402, 35,  4, 11},	/* f */
	{2, 410, 35,  5, 13},	/* g */
	{0, 419, 35,  7, 11},	/* h */
	{0, 429, 35,  4, 11},	/* i */
	{2, 436, 35,  3, 13},	/* j */
	{0, 443, 35,  7, 11},	/* k */
	{0, 453, 35,  4, 11},	/* l */
	{0, 460, 35, 10,  9},	/* m */
	{0, 473, 35,  7,  9},	/* n */
	{0, 484, 35,  5,  9},	/* o */
	{2, 493, 35,  6, 11},	/* p */
	{2, 504, 35,  6, 11},	/* q */
	{0, 513, 35,  5,  9},	/* r */
	{0, 522, 35,  3,  9},	/* s */
	{0, 529, 35,  4, 11},	/* t */
	{0, 536, 35,  7,  9},	/* u */
	{0, 546, 35,  6,  9},	/* v */
	{0, 555, 35,  9,  9},	/* w */
	{0, 567, 35,  6,  9},	/* x */
	{2, 576, 35,  6, 11},	/* y */
	{0, 585, 35,  5,  9},	/* z */
};

gcsFONTCHAR g_SmallFont_UpperCase[] =
{
	{0,  72, 35,  8, 11},	/* A */
	{0,  83, 35,  7, 11},	/* B */
	{0,  95, 35,  4, 11},	/* C */
	{0, 103, 35,  7, 11},	/* D */
	{0, 114, 35,  6, 11},	/* E */
	{0, 123, 35,  6, 11},	/* F */
	{0, 133, 35,  7, 11},	/* G */
	{0, 143, 35,  8, 11},	/* H */
	{0, 154, 35,  4, 11},	/* I */
	{2, 161, 35,  4, 13},	/* J */
	{0, 168, 35,  7, 11},	/* K */
	{0, 178, 35,  6, 11},	/* L */
	{0, 187, 35,  8, 11},	/* M */
	{0, 198, 35,  6, 11},	/* N */
	{0, 208, 35,  6, 11},	/* O */
	{0, 218, 35,  7, 11},	/* P */
	{1, 230, 35,  6, 12},	/* Q */
	{0, 240, 35,  8, 11},	/* R */
	{0, 251, 35,  5, 11},	/* S */
	{0, 259, 35,  6, 11},	/* T */
	{0, 269, 35,  6, 11},	/* U */
	{0, 278, 35,  7, 11},	/* V */
	{0, 288, 35, 10, 11},	/* W */
	{0, 301, 35,  7, 11},	/* X */
	{0, 311, 35,  7, 11},	/* Y */
	{0, 321, 35,  5, 11},	/* Z */
};

gcsFONTCHAR g_BigFont_Special1[] =
{
	{0, 734, 88, 15, 1},	/* space */
};

gcsFONTCHAR g_BigFont_Special2[] =
{
	{  7, 678, 99, 14, 18},	/* , */
	{-11, 706, 99, 15, 10},	/* - */
	{  0, 651, 99, 12, 12},	/* . */
};

gcsFONTCHAR g_BigFont_Numbers[] =
{
	{0,  59, 531, 35, 47},	/* 0 */
	{0, 116, 531, 25, 47},	/* 1 */
	{0, 162, 531, 28, 47},	/* 2 */
	{0, 214, 531, 35, 47},	/* 3 */
	{0, 271, 531, 32, 47},	/* 4 */
	{0, 326, 531, 32, 47},	/* 5 */
	{0, 380, 531, 35, 57},	/* 6 */
	{0, 436, 531, 31, 47},	/* 7 */
	{0, 489, 531, 32, 47},	/* 8 */
	{0, 543, 531, 32, 47},	/* 9 */
};

gcsFONTCHAR g_BigFont_LowerCase[] =
{
	{ 0,  59, 358, 38, 35},	/* a */
	{ 0, 118, 358, 31, 51},	/* b */
	{ 0, 171, 358, 25, 35},	/* c */
	{ 0, 218, 358, 38, 51},	/* d */
	{ 0, 278, 358, 29, 35},	/* e */
	{13, 328, 358, 23, 62},	/* f */
	{14, 373, 358, 37, 48},	/* g */
	{ 0, 432, 358, 33, 49},	/* h */
	{ 0, 487, 358, 15, 49},	/* i */
	{10, 524, 358, 18, 59},	/* j */
	{13, 565, 358, 34, 62},	/* k */
	{ 0, 620, 358, 30, 49},	/* l */
	{ 6, 672, 358, 52, 40},	/* m */
	{ 0,  59, 445, 33, 34},	/* n */
	{ 0, 115, 445, 36, 35},	/* o */
	{13, 173, 445, 37, 48},	/* p */
	{13, 232, 445, 36, 48},	/* q */
	{ 0, 290, 445, 21, 35},	/* r */
	{ 5, 332, 445, 29, 40},	/* s */
	{ 0, 384, 445, 22, 49},	/* t */
	{ 0, 428, 445, 36, 35},	/* u */
	{ 0, 486, 445, 28, 35},	/* v */
	{ 4, 536, 445, 46, 39},	/* w */
	{ 0, 604, 445, 33, 34},	/* x */
	{13, 659, 445, 34, 47},	/* y */
	{ 0, 713, 445, 29, 37},	/* z */
};

gcsFONTCHAR g_BigFont_UpperCase[] =
{
	{ 0,  58,  99, 51, 49},	/* A */
	{ 9, 131,  99, 44, 58},	/* B */
	{ 0, 197,  99, 41, 49},	/* C */
	{ 0, 259,  99, 47, 50},	/* D */
	{ 0, 328,  99, 41, 49},	/* E */
	{ 0, 392,  99, 33, 49},	/* F */
	{13, 447,  99, 42, 62},	/* G */
	{ 0, 511,  99, 51, 49},	/* H */
	{ 0, 584,  99, 38, 50},	/* I */
	{12,  59, 185, 36, 62},	/* J */
	{ 0, 115, 185, 48, 49},	/* K */
	{ 0, 184, 185, 25, 49},	/* L */
	{ 0, 232, 185, 75, 49},	/* M */
	{ 0, 329, 185, 55, 49},	/* N */
	{ 0, 406, 185, 43, 49},	/* O */
	{ 0, 471, 185, 40, 49},	/* P */
	{ 0, 533, 185, 46, 49},	/* Q */
	{ 0, 600, 185, 41, 47},	/* R */
	{ 0,  59, 272, 34, 49},	/* S */
	{ 0, 115, 272, 31, 49},	/* T */
	{ 0, 168, 272, 42, 50},	/* U */
	{ 0, 232, 272, 36, 49},	/* V */
	{ 0, 290, 272, 56, 49},	/* W */
	{ 8, 368, 272, 49, 57},	/* X */
	{13, 429, 272, 46, 62},	/* Y */
	{ 0, 507, 272, 42, 49},	/* Z */
};

gcsFONTRANGE g_SmallFontRange[] =
{
	{' ', ' ', g_SmallFont_Special1},
	{',', '.', g_SmallFont_Special2},
	{'0', '9', g_SmallFont_Numbers},
	{'a', 'z', g_SmallFont_LowerCase},
	{'A', 'Z', g_SmallFont_UpperCase},
};

gcsFONTRANGE g_BigFontRange[] =
{
	{' ', ' ', g_BigFont_Special1},
	{',', '.', g_BigFont_Special2},
	{'0', '9', g_BigFont_Numbers},
	{'a', 'z', g_BigFont_LowerCase},
	{'A', 'Z', g_BigFont_UpperCase},
};

gcsFONT g_SmallFont =
{
	g_SmallFontRange,
	gcmCOUNTOF(g_SmallFontRange),
	2
};

gcsFONT g_BigFont =
{
	g_BigFontRange,
	gcmCOUNTOF(g_BigFontRange),
	3
};

gcsFONT_PTR g_Font[] =
{
	&g_SmallFont,
	&g_BigFont,
};

/*******************************************************************************
**
**  GetNextFont
**
**  Retrieve the next available font in a loop.
**
**  INPUT:
**
**		Test2D * Test
**          Pointer to an Test2D * structure.
**
**  OUTPUT:
**
**      Nothing.
*/
gcsFONT_PTR GetNextFont(
	Test2D *Test
	)
{
	gcsFONT_PTR font = g_Font[Test->font % gcmCOUNTOF(g_Font)];
	Test->font += 1;
	return font;
}

/*******************************************************************************
**
**  GetCharacter
**
**  Retrieve character font coordinates.
**
**  INPUT:
**
**		gcsFONT_PTR Font
**			Font descriptor.
**
**		char Character
**          Character to be found in the font.
**
**  OUTPUT:
**
**      Nothing.
*/
gcsFONTCHAR_PTR GetCharacter(
	gcsFONT_PTR Font,
	char Character
	)
{
	gctUINT i;

	for (i = 0; i < Font->count; i++)
	{
		gcsFONTRANGE_PTR range = &Font->font[i];

		if ((Character >= range->from) && (Character <= range->to))
		{
			gctINT index = Character - range->from;
			return &range->font[index];
		}
	}

	gcmFATAL("Unsupported character requested: %c.", Character);
	return gcvNULL;
}

/*******************************************************************************
**
**  GetTextSize
**
**  Get the size of the text in pixels.
**
**  INPUT:
**
**		gcsFONT_PTR Font
**			Font descriptor.
**
**		char* String
**			Pointer to the text string.
**
**  OUTPUT:
**
**      Nothing.
*/
gcsPOINT GetTextSize(
	gcsFONT_PTR Font,
	char* String
	)
{
	gctINT i;
	gcsPOINT size = {0, 0};
	gctINT minTop    = 0;
	gctINT maxBottom = 0;

	for (i = 0; String[i] != 0; i++)
	{
		gcsFONTCHAR_PTR character = GetCharacter(Font, String[i]);

		gctINT top    = character->baseOffset - character->height + 1;
		gctINT bottom = character->baseOffset;

		minTop    = gcmMIN(minTop,    top);
		maxBottom = gcmMAX(maxBottom, bottom);

		if (i > 0)
		{
			size.x += Font->spacing;
		}

		size.x += character->width;
	}

	size.y = maxBottom - minTop;
	return size;
}

/*******************************************************************************
**
**  DrawString
**
**  Draw text at specified coordinates.
**
**  INPUT:
**
**		Test2D * Test
**          Pointer to an Test2D structure.
**
**		gcsFONT_PTR Font
**			Font descriptor.
**
**		gcsPOINT_PTR Origin
**			Text origin.
**
**		char* String
**			String to draw.
**
**  OUTPUT:
**
**      Nothing.
*/
gceSTATUS DrawString(
	Test2D * Test,
	gcsFONT_PTR Font,
	gcsPOINT_PTR Origin,
	char* String,
	gctBOOL UseMaskedBlit
	)
{
	gctINT i;
	gceSTATUS status = gcvSTATUS_OK;
	gcsRECT srcRect;
	gcsRECT dstRect;
	gctPOINTER mask = gcvNULL;
	gceSURF_MONOPACK pack = gcvSURF_UNPACKED;
	gctINT dstX = Origin->x;
	gctINT dstY = Origin->y;

	for (i = 0; String[i] != 0; i++)
	{
		gcsFONTCHAR_PTR character = GetCharacter(Font, String[i]);

		srcRect.left = character->x;
		srcRect.top  = character->y - character->height + character->baseOffset + 1;

		dstRect.left   = dstX;
		dstRect.top    = dstY - character->height + character->baseOffset + 1;
		dstRect.right  = dstRect.left + character->width;
		dstRect.bottom = dstRect.top  + character->height;

		switch (i % 4)
		{
		case 0:
			mask = Test->maskUnpacked;
			pack = gcvSURF_UNPACKED;
			break;

		case 1:
			mask = Test->maskPacked8;
			pack = gcvSURF_PACKED8;
			break;

		case 2:
			mask = Test->maskPacked16;
			pack = gcvSURF_PACKED16;
			break;

		case 3:
			mask = Test->maskPacked32;
			pack = gcvSURF_PACKED32;
			break;
		}

        if ((dstRect.right >= 0) && (dstRect.bottom >= 0))
        {
            if (dstRect.left < 0)
            {
                srcRect.left -= dstRect.left;
                dstRect.left = 0;
            }

            if (dstRect.top < 0)
            {
                srcRect.top -= dstRect.top;
                dstRect.top = 0;
            }

            if ((dstRect.top < dstRect.bottom)
                && (dstRect.left < dstRect.right))
            {
		        if (UseMaskedBlit)
		        {
			        gcmERR_BREAK(gcoSURF_Blit(
				        Test->bitmap, Test->dstSurf,
				        1, &srcRect, &dstRect,
				        gcvNULL,
				        0xCC, 0xAA,
				        0, 0,				/* Not used with masks. */
				        mask, pack
				        ));
		        }
		        else
		        {
			        gcsPOINT sourceSize =
			        {
				        Test->maskWidth,
				        Test->maskHeight
			        };

			        gcsPOINT sourceOrigin =
			        {
				        srcRect.left,
				        srcRect.top
			        };

			        gcmERR_BREAK(gcoSURF_MonoBlit(
				        Test->dstSurf,
				        mask, pack,
				        &sourceSize,
				        &sourceOrigin,
				        &dstRect,
				        gcvNULL,			/* No brush. */
				        0xCC, 0xAA,
				        gcvTRUE,			/* Color conversion. */
				        0,					/* Transparent on 0. */
				        gcvSURF_SOURCE_MATCH,
				        0x003CBA00,
				        0
				        ));
		        }
            }
        }

		dstX += character->width + Font->spacing;
	}

	return status;
}

static gctBOOL CDECL Render(Test2D *t2d, gctUINT frameNo)
{
	gceSTATUS status;
	gcsRECT DstRect;
	DstRect.left = DstRect.top = 0;
	DstRect.right = t2d->dstWidth < t2d->maskWidth ? t2d->dstWidth : t2d->maskWidth;
	DstRect.bottom = t2d->dstHeight < t2d->maskHeight ? t2d->dstHeight : t2d->maskHeight;

    if (frameNo < 4)
    {
    	gctPOINTER mask = gcvNULL;
    	gceSURF_MONOPACK pack = gcvSURF_UNPACKED;

    	switch (frameNo % 4)
    	{
    	case 0:
    		mask = t2d->maskUnpacked;
    		pack = gcvSURF_UNPACKED;
    		break;

    	case 1:
    		mask = t2d->maskPacked8;
    		pack = gcvSURF_PACKED8;
    		break;

    	case 2:
    		mask = t2d->maskPacked16;
    		pack = gcvSURF_PACKED16;
    		break;

    	case 3:
    		mask = t2d->maskPacked32;
    		pack = gcvSURF_PACKED32;
    		break;
    	}

    	gcmONERROR(gcoSURF_Blit(
    		t2d->bitmap, t2d->dstSurf,
    		1, gcvNULL, gcvNULL,
    		gcvNULL,
    		0xCC, 0xCC,
    		gcvSURF_OPAQUE,
    		~0,
    		gcvNULL, 0
    		));

    	gcmONERROR(gcoSURF_Blit(
    		t2d->bitmap, t2d->dstSurf,
    		1, gcvNULL, &DstRect,
    		gcvNULL,
    		0x66, 0xAA,				/* 0x66 = src XOR dest. */
    		0, 0,					/* Not used with masks. */
    		mask, pack
    		));
    }
    else if (frameNo == 4)
    {
    	gcsFONT_PTR font = GetNextFont(t2d);
    	static char text[] = "...masked blit origin test...";

    	/* Get the size of the text. */
    	gcsPOINT textSize = GetTextSize(font, text);

    	/* Determine the origin. */
    	gcsPOINT textOrigin;
    	textOrigin.x = (t2d->dstWidth  - textSize.x) / 2;
    	textOrigin.y = (t2d->dstHeight - textSize.y) / 2;

    	/* Draw text. */
    	gcmONERROR(DrawString(
    		t2d,
    		font,
    		&textOrigin,
    		text,
    		gcvTRUE
    		));
    }
    else if (frameNo < 9)
    {
    	gctPOINTER source = gcvNULL;
    	gceSURF_MONOPACK pack = gcvSURF_UNPACKED;
    	char* packing = "unpacked";

    	gcsPOINT sourceSize =
    	{
            t2d->maskWidth,
    		t2d->maskHeight
    	};

    	switch ((frameNo - 5) % 4)
    	{
    	case 0:
    		packing = "unpacked";
    		source = t2d->maskUnpacked;
    		pack = gcvSURF_UNPACKED;
    		break;

    	case 1:
    		packing = "8-bit packed";
    		source = t2d->maskPacked8;
    		pack = gcvSURF_PACKED8;
    		break;

    	case 2:
    		packing = "16-bit packed";
    		source = t2d->maskPacked16;
    		pack = gcvSURF_PACKED16;
    		break;

    	case 3:
    		packing = "32-bit packed";
    		source = t2d->maskPacked32;
    		pack = gcvSURF_PACKED32;
    		break;
    	}

    	gcmONERROR(gcoSURF_MonoBlit(
    		t2d->dstSurf,
    		source, pack,
    		&sourceSize,
    		gcvNULL,			/* Default origin. */
    		&DstRect,			/* Entire destination. */
    		gcvNULL,			/* No brush. */
    		0xCC, 0xCC,
    		gcvTRUE,
    		0,
    		gcvSURF_OPAQUE,
    		0x003CBA00,
    		0x000D6CFF
    		));
    }
    else if (frameNo == 9)
    {
    	gcsFONT_PTR font = GetNextFont(t2d);
    	static char text[] = "...monoexpansion...";

    	/* Get the size of the text. */
    	gcsPOINT textSize = GetTextSize(font, text);

    	/* Determine the origin. */
    	gcsPOINT textOrigin;
    	textOrigin.x = ((gctINT32)t2d->dstWidth  - textSize.x) / 2;
    	textOrigin.y = ((gctINT32)t2d->dstHeight - textSize.y) / 2;

    	/* Draw text. */
    	gcmONERROR(DrawString(
    		t2d,
    		font,
    		&textOrigin,
    		text,
    		gcvFALSE
    		));
    }
    else if (frameNo == 10)
    {
    	gctUINT i;
    	gcsFONT_PTR font = &g_SmallFont;
    	gcsPOINT textSize;
    	gcsPOINT textOrigin;
    	gctUINT verSpacing;
    	gctINT height;

    	/* Determine vertical placement. */
    	verSpacing = 19;
    	height = ('z' - 'a' + 1) * verSpacing;
    	textOrigin.y = ((gctINT32)t2d->dstHeight - height) / 2;

    	for (i = 'a'; i <= 'z'; i++)
    	{
    		char buffer[60 + 1];
    		memset(buffer, i, gcmCOUNTOF(buffer) - 1);
    		buffer[gcmCOUNTOF(buffer) - 1] = 0;

    		/* Get the size of the text. */
    		textSize = GetTextSize(font, buffer);

    		/* Determine the origin. */
    		textOrigin.x = ((gctINT32)t2d->dstWidth  - textSize.x) / 2;

    		/* Draw text. */
    		gcmONERROR(DrawString(
    			t2d,
    			font,
    			&textOrigin,
    			buffer,
    			gcvFALSE
    			));

    		/* Update vertical position. */
    		textOrigin.y += verSpacing;
    	}
    }
    else
    {
        gcmONERROR(gcvSTATUS_INVALID_ARGUMENT);
    }

    gcmONERROR(gcoHAL_Commit(t2d->runtime->hal, gcvTRUE));

    return gcvTRUE;

OnError:
    GalOutput(GalOutputType_Error | GalOutputType_Console,
        "%s(%d) failed:%s\n",__FUNCTION__, __LINE__, gcoOS_DebugStatus2Name(status));
	return gcvFALSE;
}

static void CDECL Destroy(Test2D *t2d)
{
    gceSTATUS status = gcvSTATUS_OK;

    if ((t2d->dstSurf != gcvNULL) && (t2d->dstLgcAddr != gcvNULL))
    {
		if (gcmIS_ERROR(gcoSURF_Unlock(t2d->dstSurf, t2d->dstLgcAddr)))
		{
			GalOutput(GalOutputType_Error | GalOutputType_Console,
        		"%s(%d) failed:%s\n",__FUNCTION__, __LINE__, gcoOS_DebugStatus2Name(status));
		}
		t2d->dstLgcAddr = gcvNULL;
    }

	// destroy source surface
	if (t2d->bitmap != gcvNULL)
    {
        if (gcmIS_ERROR(gcoSURF_Destroy(t2d->bitmap)))
		{
			GalOutput(GalOutputType_Error | GalOutputType_Console, "Destroy Surf failed:%s\n", GalStatusString(status));
		}
    }

	// destroy mask
	if (t2d->maskUnpacked)
    {
		free(t2d->maskUnpacked);
    }

    if (t2d->maskPacked8)
    {
		free(t2d->maskPacked8);
    }

    if (t2d->maskPacked16)
    {
		free(t2d->maskPacked16);
    }

    if (t2d->maskPacked32)
    {
		free(t2d->maskPacked32);
    }

    free(t2d);
}

const gceFEATURE FeatureList[]=
{
    gcvFEATURE_ANDROID_ONLY,
};

static gctBOOL CDECL Init(Test2D *t2d, GalRuntime *runtime)
{
    gceSTATUS status;
	BMPINFO *pInfo;
	gctPOINTER sourcefile = "resource/VV_Background.bmp";
	gctPOINTER maskfile = "resource/Font.bmp";

    gctUINT32 k, listLen = sizeof(FeatureList)/sizeof(gctINT);
    gctBOOL featureStatus;
    char featureName[FEATURE_NAME_LEN], featureMsg[FEATURE_MSG_LEN];

    runtime->wholeDescription = (char*)malloc(FEATURE_NAME_LEN * listLen + strlen(s_CaseDescription) + 1);

    if (runtime->wholeDescription == gcvNULL)
    {
        gcmONERROR(gcvSTATUS_OUT_OF_MEMORY);
    }

    memcpy(runtime->wholeDescription, s_CaseDescription, strlen(s_CaseDescription) + 1);

    for(k = 0; k < listLen; k++)
    {
        gcmONERROR(GalQueryFeatureStr(FeatureList[k], featureName, featureMsg, &featureStatus));
        if (gcoHAL_IsFeatureAvailable(runtime->hal, FeatureList[k]) == featureStatus)
        {
            GalOutput(GalOutputType_Result | GalOutputType_Console, "%s is not supported.\n", featureMsg);
            runtime->notSupport = gcvTRUE;
        }
        strncat(runtime->wholeDescription, featureName, k==listLen-1 ? strlen(featureName)+1:strlen(featureName));
    }

    if (runtime->notSupport)
        return gcvFALSE;

    t2d->runtime = runtime;

	t2d->dstSurf    = runtime->target;
	t2d->dstFormat = runtime->format;
	t2d->dstWidth = 0;
	t2d->dstHeight = 0;
	t2d->dstStride = 0;
	t2d->dstPhyAddr = 0;
	t2d->dstLgcAddr = 0;

    t2d->bitmap = gcvNULL;
    t2d->maskUnpacked = t2d->maskPacked8 = t2d->maskPacked16 = t2d->maskPacked32 = gcvNULL;
    t2d->font = 0;

	// create source surface
    t2d->bitmap = GalLoadDIB2Surface(t2d->runtime->hal, sourcefile);
	if (t2d->bitmap == NULL)
	{
		GalOutput(GalOutputType_Error, "can not load %s\n", sourcefile);
		gcmONERROR(gcvSTATUS_NOT_FOUND);
	}

	// create mask surface
 	t2d->maskUnpacked = GalLoadDIBitmap(maskfile, &pInfo);
	if (t2d->maskUnpacked == gcvNULL)
	{
		// destroy source surface
		if (t2d->bitmap != gcvNULL)
	    {
            status = gcoSURF_Destroy(t2d->bitmap);
	        if (gcmIS_ERROR(status))
			{
				GalOutput(GalOutputType_Error | GalOutputType_Console, "Destroy Surf failed:%s\n", GalStatusString(status));
			}
            t2d->bitmap = gcvNULL;
	    }
		GalOutput(GalOutputType_Error, "can not open %s\n", maskfile);
		gcmONERROR(gcvSTATUS_NOT_FOUND);
	}

	t2d->maskWidth = pInfo->bmiHeader.biWidth;
	if (pInfo->bmiHeader.biHeight > 0)
	{
		gctINT i;
		gctINT32 Stride
			= pInfo->bmiHeader.biWidth
			* pInfo->bmiHeader.biBitCount
			/ 8;
		gctPOINTER temp;
		gctUINT8_PTR bits = t2d->maskUnpacked;

		Stride = gcmALIGN(Stride, 4);
		temp = malloc(Stride);
		t2d->maskHeight = pInfo->bmiHeader.biHeight;
		for (i = 0; i < pInfo->bmiHeader.biHeight/2; i++)
		{
			memcpy(temp, bits + i * Stride, Stride);
			memcpy(bits + i * Stride, bits + (pInfo->bmiHeader.biHeight - 1 - i) * Stride, Stride);
			memcpy(bits + (pInfo->bmiHeader.biHeight - 1 - i) * Stride, temp, Stride);
		}
		free(temp);
	}
	else
	{
		t2d->maskHeight = -pInfo->bmiHeader.biHeight;
	}

	free(pInfo);

    gcmONERROR(GalPackStream(
	    t2d->maskUnpacked,
	    t2d->maskWidth,
	    t2d->maskHeight,
	    gcvSURF_PACKED8,
	    &t2d->maskPacked8
	    ));

    gcmONERROR(GalPackStream(
	    t2d->maskUnpacked,
	    t2d->maskWidth,
	    t2d->maskHeight,
	    gcvSURF_PACKED16,
	    &t2d->maskPacked16
	    ));

    gcmONERROR(GalPackStream(
	    t2d->maskUnpacked,
	    t2d->maskWidth,
	    t2d->maskHeight,
	    gcvSURF_PACKED32,
	    &t2d->maskPacked32
	    ));

	// dst with dst surf
	gcmONERROR(gcoSURF_GetAlignedSize(t2d->dstSurf,
										&t2d->dstWidth,
										&t2d->dstHeight,
										&t2d->dstStride));

	gcmONERROR(gcoSURF_Lock(t2d->dstSurf, &t2d->dstPhyAddr, &t2d->dstLgcAddr));

	t2d->base.render     = (PGalRender)Render;
    t2d->base.destroy    = (PGalDestroy)Destroy;
    t2d->base.frameCount = 11;
	t2d->base.description = s_CaseDescription;

    return gcvTRUE;

OnError:
    GalOutput(GalOutputType_Error | GalOutputType_Console,
        "%s(%d) failed:%s\n",__FUNCTION__, __LINE__, gcoOS_DebugStatus2Name(status));
	return gcvFALSE;
}

GalTest * CDECL GalCreateTestObject(GalRuntime *runtime)
{
    Test2D *t2d = (Test2D *)malloc(sizeof(Test2D));

    if (!Init(t2d, runtime)) {
        free(t2d);
        return NULL;
    }

    return &t2d->base;
}
