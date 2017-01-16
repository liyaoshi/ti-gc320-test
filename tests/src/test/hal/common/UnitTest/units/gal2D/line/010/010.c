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
 *  Feature:    Draw line.
 *  API:        gco2D_ColorLine
 *  Check:      Line count, position and color.
 */
#include <galUtil.h>

static gctCONST_STRING s_CaseDescription = \
"Case gal2DLine010\n" \
"Operation: Test gco2D_ColorLine interface.\n" \
"2D API: gco2D_ColorLine\n" \
"Src: [None]\n"\
"Dst: Size        [configurable]\n"\
"     Rect        [configurable, 8 rectangles]\n"\
"     Format      [configurable]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Brush: [None]\n"\
"Alphablend: [disable]\n" \
"HW feature dependency: ";

typedef struct Test2D {
    GalTest     base;
    GalRuntime  *runtime;

	/* Dst surface. */
    gcoSURF			dstSurf;
	gceSURF_FORMAT	dstFormat;
	gctUINT			dstWidth;
	gctUINT			dstHeight;
	gctINT			dstStride;
	gctUINT32		dstPhyAddr;
	gctPOINTER		dstLgcAddr;

	/* Line color. */
	gcsRECT			lines[8];
	gctUINT32		lineColor;
} Test2D;


static gctBOOL CDECL Render(Test2D *t2d, gctUINT frameNo)
{
	gcsRECT   dstRect = { 0, 0, t2d->dstWidth, t2d->dstHeight };
	gco2D     egn2D   = t2d->runtime->engine2d;
	gceSTATUS status;

	gcmONERROR(gco2D_SetClipping(egn2D, &dstRect));

	/* clear the dst surface. */
	gcmONERROR(Gal2DCleanSurface(t2d->runtime->hal, t2d->dstSurf, COLOR_ARGB8(0, 0, 0, 0)));

	gcmONERROR(gco2D_SetTarget(egn2D, t2d->dstPhyAddr, t2d->dstStride, gcvSURF_0_DEGREE, t2d->dstWidth));

	t2d->lines[0].left   = 0;
	t2d->lines[0].top    = 0;
	t2d->lines[0].right  = t2d->dstWidth;
	t2d->lines[0].bottom = t2d->dstHeight;

	t2d->lines[1].left   = t2d->dstWidth;
	t2d->lines[1].top    = 0;
	t2d->lines[1].right  = 0;
	t2d->lines[1].bottom = t2d->dstHeight;

	t2d->lines[2].left   = 0;
	t2d->lines[2].top    = t2d->dstHeight/2;
	t2d->lines[2].right  = t2d->dstWidth;
	t2d->lines[2].bottom = t2d->dstHeight/2;

	t2d->lines[3].left   = t2d->dstWidth/2;
	t2d->lines[3].top    = 0;
	t2d->lines[3].right  = t2d->dstWidth/2;
	t2d->lines[3].bottom = t2d->dstHeight;

	t2d->lines[4].left   = t2d->dstWidth/4;
	t2d->lines[4].top    = 0;
	t2d->lines[4].right  = t2d->dstWidth/4;
	t2d->lines[4].bottom = t2d->dstHeight;

	t2d->lines[5].left   = t2d->dstWidth*3/4;
	t2d->lines[5].top    = 0;
	t2d->lines[5].right  = t2d->dstWidth*3/4;
	t2d->lines[5].bottom = t2d->dstHeight;

	t2d->lines[6].left   = 0;
	t2d->lines[6].top    = t2d->dstHeight/4;
	t2d->lines[6].right  = t2d->dstWidth;
	t2d->lines[6].bottom = t2d->dstHeight/4;

	t2d->lines[7].left   = 0;
	t2d->lines[7].top    = t2d->dstHeight*3/4;
	t2d->lines[7].right  = t2d->dstWidth;
	t2d->lines[7].bottom = t2d->dstHeight*3/4;

	/* Set the line color (ARGB8888). */
	t2d->lineColor = 0x00536170;

	gcmONERROR(gco2D_ColorLine(egn2D, 8, t2d->lines, t2d->lineColor, 0xCC, 0xCC, t2d->dstFormat));

	gcmONERROR(gco2D_Flush(egn2D));

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
			GalOutput(GalOutputType_Error | GalOutputType_Console, "Unlock desSurf failed:%s\n", GalStatusString(status));
		}
		t2d->dstLgcAddr = gcvNULL;
    }

    free(t2d);
}

const gceFEATURE FeatureList[]=
{
    gcvFEATURE_ANDROID_ONLY,
};

static gctBOOL CDECL Init(Test2D *t2d, GalRuntime *runtime)
{
	gceSTATUS status = gcvSTATUS_OK;

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

    t2d->runtime    = runtime;

	t2d->dstSurf    = runtime->target;
	t2d->dstFormat  = runtime->format;
	t2d->dstWidth   = 0;
	t2d->dstHeight  = 0;
	t2d->dstStride  = 0;
	t2d->dstPhyAddr = 0;
	t2d->dstLgcAddr = 0;

	gcmONERROR(gcoSURF_GetAlignedSize(t2d->dstSurf,
										&t2d->dstWidth,
										&t2d->dstHeight,
										&t2d->dstStride));

	gcmONERROR(gcoSURF_Lock(t2d->dstSurf, &t2d->dstPhyAddr, &t2d->dstLgcAddr));

	t2d->base.render      = (PGalRender)Render;
    t2d->base.destroy     = (PGalDestroy)Destroy;
    t2d->base.frameCount  = 1;
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
