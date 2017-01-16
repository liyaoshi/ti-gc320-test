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
 *  Feature:    ColorSource - width & stride & color format
 *  API:        gco2D_SetSource gco2D_SetColorSource/gco2D_SetColorSourceAdvanced
 *				gco2D_SetColorSource is only working with old PE (<2.0) and
 *				gco2D_SetColorSourceAdvanced is only working with PE 2.0 and above.
 *  Check:
*/
#include <galUtil.h>

static gctCONST_STRING s_CaseDescription = \
"Case gal2DColorSource008\n" \
"Operation: Check the format A8 input/output through BitBlit.\n" \
"2D API: gco2D_SetSource gco2D_SetColorSource/gco2D_SetColorSourceAdvanced\n" \
"Src: Size        [640x480]\n"\
"     Rect        [0,0,640,480]\n"\
"     Format      [A8]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Dst: Size        [640x480]\n"\
"     Rect        [0,0,640,480]\n"\
"     Format      [A8]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Brush: [None]\n"\
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

	//source surface
    gcoSURF			srcSurf;
	gceSURF_FORMAT	srcFormat;
	gctUINT			srcWidth;
	gctUINT			srcHeight;
	gctINT			srcStride;
	gctUINT32		srcPhyAddr;
	gctPOINTER		srcLgcAddr;

} Test2D;

static gctBOOL CDECL Render(Test2D *t2d, gctUINT frameNo)
{
	gceSTATUS status;
	gctBOOL	ret = gcvTRUE;
	gcoSURF tmpSurf;
	gceSURF_FORMAT	tmpFormat;
	gctUINT			tmpWidth;
	gctUINT			tmpHeight;
	gctINT			tmpStride;
	gctUINT32		tmpPhyAddr;
	gctPOINTER		tmpLgcAddr;
	gcsRECT Rect;
	gco2D egn2D = t2d->runtime->engine2d;

	// create tmp surf
	if (gcmIS_ERROR(gcoSURF_Construct(t2d->runtime->hal, t2d->srcWidth,
		t2d->srcHeight, 1, gcvSURF_BITMAP, gcvSURF_INDEX8, gcvPOOL_DEFAULT,
		&tmpSurf)))
	{
		GalOutput(GalOutputType_Error, "create src surface failed\n");
		return gcvFALSE;
	}
	gcmONERROR(gcoSURF_GetAlignedSize(tmpSurf,
										gcvNULL,
										gcvNULL,
										&tmpStride));

	gcmONERROR(gcoSURF_GetSize(tmpSurf,
								&tmpWidth,
								&tmpHeight,
								gcvNULL));

	gcmONERROR(gcoSURF_GetFormat(tmpSurf, gcvNULL, &tmpFormat));
	tmpFormat = gcvSURF_A8;

	gcmONERROR(gcoSURF_Lock(tmpSurf, &tmpPhyAddr, &tmpLgcAddr));

	// blit A8 to A8
	if (t2d->runtime->pe20)
	{
		gcmONERROR(gco2D_SetColorSourceAdvanced(egn2D, t2d->srcPhyAddr, t2d->srcStride, t2d->srcFormat,
						gcvSURF_0_DEGREE, t2d->srcWidth, t2d->srcHeight, gcvFALSE));
	}
	else
	{
		gcmONERROR(gco2D_SetColorSource(egn2D, t2d->srcPhyAddr, t2d->srcStride, t2d->srcFormat,
						gcvSURF_0_DEGREE, t2d->srcWidth, gcvFALSE, gcvSURF_OPAQUE, 0));
	}

	Rect.left = 0;
	Rect.top = 0;
	Rect.right = t2d->srcWidth;
	Rect.bottom = t2d->srcHeight;
	gcmONERROR(gco2D_SetSource(egn2D, &Rect));

	gcmONERROR(gco2D_SetTarget(egn2D, tmpPhyAddr, tmpStride, 0, tmpWidth));

	gcmONERROR(gco2D_SetClipping(egn2D, &Rect));

	gcmONERROR(gco2D_Blit(egn2D, 1, &Rect, 0xCC, 0xCC, tmpFormat));

	gcmONERROR(gco2D_Flush(egn2D));
	gcmONERROR(gcoHAL_Commit(t2d->runtime->hal, gcvTRUE));

	// check the result here
	if (memcmp(t2d->srcLgcAddr, tmpLgcAddr, t2d->srcWidth*t2d->srcHeight))
	{
		GalOutput(GalOutputType_Error, "result error\n");
	}

	gcmONERROR(gcoSURF_Unlock(tmpSurf, tmpLgcAddr));

	gcmONERROR(gcoSURF_Blit(tmpSurf, t2d->dstSurf, 1, gcvNULL, gcvNULL, gcvNULL,
		0xCC, 0xCC, gcvSURF_OPAQUE, 0, gcvNULL, gcvSURF_UNPACKED));

	gcmONERROR(gco2D_Flush(egn2D));
	gcmONERROR(gcoHAL_Commit(t2d->runtime->hal, gcvTRUE));

	if (gcmIS_ERROR(gcoSURF_Destroy(tmpSurf)))
	{
		GalOutput(GalOutputType_Error | GalOutputType_Console, "Destroy Surf failed:%s\n", GalStatusString(status));
	}

    return ret;

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

	// destroy source surface
	if (t2d->srcSurf != gcvNULL)
    {
		if (t2d->srcLgcAddr)
		{
			if (gcmIS_ERROR(gcoSURF_Unlock(t2d->srcSurf, t2d->srcLgcAddr)))
			{
				GalOutput(GalOutputType_Error | GalOutputType_Console, "Unlock srcSurf failed:%s\n", GalStatusString(status));
			}
			t2d->srcLgcAddr = 0;
		}

        if (gcmIS_ERROR(gcoSURF_Destroy(t2d->srcSurf)))
		{
			GalOutput(GalOutputType_Error | GalOutputType_Console, "Destroy Surf failed:%s\n", GalStatusString(status));
		}
    }

    free(t2d);
}

const gceFEATURE FeatureList[]=
{
    gcvFEATURE_2DPE20,
    gcvFEATURE_2D_A8_TARGET,
    gcvFEATURE_2D_NO_COLORBRUSH_INDEX8,
};

static gctBOOL CDECL Init(Test2D *t2d, GalRuntime *runtime)
{
	gceSTATUS status;
	gctSTRING sourcefile = "resource/index8_A8.bmp";

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

	t2d->srcSurf    = gcvNULL;
	t2d->srcWidth = 0;
	t2d->srcHeight = 0;
	t2d->srcStride = 0;
	t2d->srcPhyAddr = 0;
	t2d->srcLgcAddr = 0;
	t2d->srcFormat = gcvSURF_UNKNOWN;

	gcmONERROR(gcoSURF_GetAlignedSize(t2d->dstSurf,
										&t2d->dstWidth,
										&t2d->dstHeight,
										&t2d->dstStride));

	gcmONERROR(gcoSURF_Lock(t2d->dstSurf, &t2d->dstPhyAddr, &t2d->dstLgcAddr));

	// create source surface
	t2d->srcSurf = GalLoadDIB2Surface(t2d->runtime->hal, sourcefile);
	if (t2d->srcSurf == NULL)
	{
		GalOutput(GalOutputType_Error, "can not load %s\n", sourcefile);
		gcmONERROR(gcvSTATUS_NOT_FOUND);
	}
	gcmONERROR(gcoSURF_GetAlignedSize(t2d->srcSurf,
										gcvNULL,
										gcvNULL,
										&t2d->srcStride));

	gcmONERROR(gcoSURF_GetSize(t2d->srcSurf,
								&t2d->srcWidth,
								&t2d->srcHeight,
								gcvNULL));

	//gcmONERROR(gcoSURF_GetFormat(t2d->srcSurf, gcvNULL, &t2d->srcFormat));
	t2d->srcFormat = gcvSURF_A8;

	gcmONERROR(gcoSURF_Lock(t2d->srcSurf, &t2d->srcPhyAddr, &t2d->srcLgcAddr));

    t2d->base.render     = (PGalRender)Render;
    t2d->base.destroy    = (PGalDestroy)Destroy;
    t2d->base.frameCount = 1;
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
