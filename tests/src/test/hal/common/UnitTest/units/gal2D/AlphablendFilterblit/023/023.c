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
 *  Feature:    Filter Blit - kernel size
 *  API:        gco2D_FilterBlit
 *  Check:
 */
#include <galUtil.h>

static gctCONST_STRING s_CaseDescription = \
"Case gal2DAlphablendFilterBlit023\n" \
"Operation: Test kernel size on hardware with ANDROID_ONLY feature.\n" \
"2D API: gco2D_FilterBlit gco2D_EnableAlphaBlend/gco2D_EnableAlphaBlendAdvanced\n" \
"Src: Size        [400x400]\n"\
"     Rect        [0,0,400,400]\n"\
"     Format      [ARGB8888]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Dst: Size        [configurable]\n"\
"     Rect        [configurable]\n"\
"     Format      [configurable]\n"\
"     Rotation    [0]\n"\
"     Tile        [linear]\n"\
"     Compression [None]\n" \
"Brush: [None]\n"\
"KernelSize: [1/3/5]\n" \
"Alphablend: [enable]\n" \
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
    gcoSURF         dstTempSurf;

	//source surface
    gcoSURF			srcSurf;
	gceSURF_FORMAT	srcFormat;
	gctUINT			srcWidth;
	gctUINT			srcHeight;
	gctINT			srcStride;
	gctUINT32		srcPhyAddr;
	gctPOINTER		srcLgcAddr;
	gctPOINTER		srcULgcAddr;
	gctUINT32		srcUPhyAddr;
	gctINT			srcUStride;
	gctPOINTER		srcVLgcAddr;
	gctUINT32		srcVPhyAddr;
	gctINT			srcVStride;
} Test2D;


static gctBOOL CDECL Render(Test2D *t2d, gctUINT frameNo)
{
	gctUINT8 horKernel, verKernel;
	gcsRECT srcRect = {0, 0, t2d->srcWidth, t2d->srcHeight};
	gco2D egn2D = t2d->runtime->engine2d;
	gceSTATUS status;
	gcsRECT dstRect = {0, 0, t2d->dstWidth, t2d->dstHeight}, destSubRect;
    gctINT8 srcAlpha = (frameNo * 50) & 0xFF;
    gctINT8 dstAlpha = 0xFF - srcAlpha;

    // copy dest image
    gcmONERROR(gcoSURF_Blit(t2d->dstTempSurf, t2d->dstSurf, 1,
        gcvNULL, gcvNULL, gcvNULL, 0xCC, 0xCC, 0, 0, gcvNULL, 0));

    // set clipping
	gcmONERROR(gco2D_SetClipping(egn2D, gcvNULL));

	// enalbe alphablend
	if (t2d->runtime->pe20)
	{
		gcmONERROR(gco2D_SetSourceGlobalColorAdvanced(egn2D, srcAlpha << 24));
		gcmONERROR(gco2D_SetTargetGlobalColorAdvanced(egn2D, dstAlpha << 24));
		gcmONERROR(gco2D_EnableAlphaBlendAdvanced(egn2D,
					gcvSURF_PIXEL_ALPHA_STRAIGHT, gcvSURF_PIXEL_ALPHA_STRAIGHT,
					gcvSURF_GLOBAL_ALPHA_ON, gcvSURF_GLOBAL_ALPHA_ON,
					gcvSURF_BLEND_STRAIGHT, gcvSURF_BLEND_STRAIGHT));

	}
	else
	{
		gcmONERROR(gco2D_EnableAlphaBlend(egn2D,
					srcAlpha, dstAlpha,
					gcvSURF_PIXEL_ALPHA_STRAIGHT, gcvSURF_PIXEL_ALPHA_STRAIGHT,
					gcvSURF_GLOBAL_ALPHA_ON, gcvSURF_GLOBAL_ALPHA_ON,
					gcvSURF_BLEND_STRAIGHT, gcvSURF_BLEND_STRAIGHT,
					gcvSURF_COLOR_STRAIGHT, gcvSURF_COLOR_STRAIGHT));
	}

	switch (frameNo)
	{
	case 0:
		horKernel = 1;
		verKernel = 1;
		break;

	case 1:
		horKernel = 3;
		verKernel = 3;
		break;

	case 2:
		horKernel = 5;
		verKernel = 5;
		break;

	default:
		return gcvFALSE;
	}

	// set kernel size
	gcmONERROR(gco2D_SetKernelSize(egn2D, horKernel, verKernel));

	destSubRect.left   = 0;
	destSubRect.top    = 0;
	destSubRect.right  = dstRect.right  - dstRect.left;
	destSubRect.bottom = dstRect.bottom - dstRect.top;

    gcmONERROR(gco2D_FilterBlit(egn2D,
            t2d->srcPhyAddr, t2d->srcStride,
			t2d->srcUPhyAddr, t2d->srcUStride,
			t2d->srcVPhyAddr, t2d->srcVStride,
			t2d->srcFormat, gcvSURF_0_DEGREE, t2d->srcWidth, &srcRect,
            t2d->dstPhyAddr, t2d->dstStride, t2d->dstFormat, gcvSURF_0_DEGREE, t2d->dstWidth, &dstRect,
            &destSubRect));

	gcmONERROR(gco2D_Flush(egn2D));

    // disalbe alphablend
	gcmONERROR(gco2D_DisableAlphaBlend(egn2D));

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

    if (t2d->dstTempSurf != gcvNULL)
    {
        if (gcmIS_ERROR(gcoSURF_Destroy(t2d->dstTempSurf)))
		{
			GalOutput(GalOutputType_Error | GalOutputType_Console, "Destroy Surf failed:%s\n", GalStatusString(status));
		}
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
    gcvFEATURE_SCALER,
    gcvFEATURE_2D_FILTERBLIT_PLUS_ALPHABLEND,
};

static gctBOOL CDECL Init(Test2D *t2d, GalRuntime *runtime)
{
	gceSTATUS status;
	char * sourcefile = "resource/source.bmp";
    char * destfile = "resource/VV_Background.bmp";

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
	t2d->srcULgcAddr = 0;
	t2d->srcUPhyAddr = 0;
	t2d->srcUStride = 0;
	t2d->srcVLgcAddr = 0;
	t2d->srcVPhyAddr = 0;
	t2d->srcVStride = 0;
	t2d->srcFormat = gcvSURF_UNKNOWN;
    t2d->dstTempSurf = gcvNULL;

    // create temp dst surface
    t2d->dstTempSurf = GalLoadDIB2Surface(t2d->runtime->hal, destfile);
	if (t2d->dstTempSurf == NULL)
	{
		GalOutput(GalOutputType_Error, "can not load %s\n", destfile);
		gcmONERROR(gcvSTATUS_NOT_FOUND);
	}

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

	gcmONERROR(gcoSURF_GetFormat(t2d->srcSurf, gcvNULL, &t2d->srcFormat));

	gcmONERROR(gcoSURF_Lock(t2d->srcSurf, &t2d->srcPhyAddr, &t2d->srcLgcAddr));

	// dst with dst surf
	gcmONERROR(gcoSURF_GetAlignedSize(t2d->dstSurf,
										&t2d->dstWidth,
										&t2d->dstHeight,
										&t2d->dstStride));

	gcmONERROR(gcoSURF_Lock(t2d->dstSurf, &t2d->dstPhyAddr, &t2d->dstLgcAddr));

    t2d->base.render     = (PGalRender)Render;
    t2d->base.destroy    = (PGalDestroy)Destroy;
    t2d->base.frameCount = 3;
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
