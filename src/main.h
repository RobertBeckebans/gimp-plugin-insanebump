/* GIMP Plug-in InsaneBump
 * Copyright (C) 2013  Derby Russsell <jdrussell51@gmail.com> (the "Author").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Author of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Author.
 */

#ifndef __MAIN_H__
#define __MAIN_H__

/**
 * Changed from 1.0.4 to 1.0.5 on 12/4/2013 because Channel Mixer
 * values were corrected for -1.0 to 1.0
 * Changed from 1.0.5 to 1.0.6 on 12/7/2013 because Many Issues fixed.
 */
#define NAME_AND_VERSION "Insane Bump 1.0.6"

/**
 * Structure to house the plugin values.
 */
typedef struct
{
    /**
     * Remove Lighting toggle TRUE (1) or FALSE (0)
     */
    gint32 RemoveLighting;

    /**
     * Upscale(HD) toggle TRUE (1) or FALSE (0)
     */
    gint32 Resizie;

    /**
     * Tile toggle TRUE (1) or FALSE (0)
     */
    gint32 Tile;

    /**
     * New Width integer spinner
     */
    gint32 newWidth;

    /**
     * Edge Specular Map toggle TRUE (1) or FALSE (0)
     */
    gint32 EdgeSpecular;

    /**
     * Specular Definition integer spinner
     */
    gint32 defSpecular;

    /**
     * Depth(+/-) integer spinner
     */
    gfloat Depth;

    /**
     * Large Detail Size integer spinner
     */
    gint32 LargeDetails;

    /**
     * Medium Detail Size integer spinner
     */
    gint32 MediumDetails;

    /**
     * Small Detail Size integer spinner
     */
    gint32 SmallDetails;

    /**
     * Shape Recognition(%) integer spinner
     */
    gint32 ShapeRecog;

    /**
     * Smooth Step toggle TRUE (1) or FALSE (0)
     */
    gint32 smoothstep;

    /**
     * Noise toggle TRUE (1) or FALSE (0)
     */
    gint32 Noise;

    /**
     * Invert Height Map toggle TRUE (1) or FALSE (0)
     */
    gint32 invh;

    /**
     * AO integer spinner
     */
    gint32 ao;

    /**
     * The main image Identifier.  This value is set at the beginning of
     * the dialog creation.
     */
    gint32 image_ID;

    /**
     * The Preview toggle TRUE (1) or FALSE (0).  Used to determine if the
     * user has activated the preview pane or not.  If not, all preview drawing
     * is turned off.
     */
    gint32 prev;
} PlugInVals;

/*  Global Variables  */
extern GtkWidget *progress;
extern GtkWidget *progress_label;

/*  Default values  */

extern const PlugInVals default_vals;

void copyPlugInVals(const PlugInVals *some_vals_source, PlugInVals *some_vals_destination);

#endif /* __MAIN_H__ */
