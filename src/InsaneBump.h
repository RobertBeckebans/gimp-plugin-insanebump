/************************************************************************
 *   insanemap GIMP plugin
 *
 *   Copyright (C) 2013 Omar Emad
 *   Copyright (C) 2013 Derby Russell <jdrussell51@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; see the file COPYING.  If not, write to
 *   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *   Boston, MA 02111-1307, USA.
 ************************************************************************/

#ifndef INSANE_BUMP_HEADER_GUARD
#define INSANE_BUMP_HEADER_GUARD

typedef struct
{
    gint filter;
    gdouble minz;
    gdouble scale;
    gint wrap;
    gint height_source;
    gint alpha;
    gint conversion;
    gint dudv;
    gint xinvert;
    gint yinvert;
    gint swapRGB;
    gdouble contrast;
    gint32 alphamap_id;
} NormalmapVals;

extern NormalmapVals nmapvals;

enum FILTER_TYPE
{
   FILTER_NONE = 0, FILTER_SOBEL_3x3, FILTER_SOBEL_5x5, FILTER_PREWITT_3x3,
   FILTER_PREWITT_5x5, FILTER_3x3, FILTER_5x5, FILTER_7x7, FILTER_9x9,
   MAX_FILTER_TYPE
};

enum ALPHA_TYPE
{
   ALPHA_NONE = 0, ALPHA_HEIGHT, ALPHA_INVERSE_HEIGHT, ALPHA_ZERO, ALPHA_ONE,
   ALPHA_INVERT, ALPHA_MAP, MAX_ALPHA_TYPE
};

enum CONVERSION_TYPE
{
   CONVERT_NONE = 0, CONVERT_BIASED_RGB, CONVERT_RED, CONVERT_GREEN, 
   CONVERT_BLUE, CONVERT_MAX_RGB, CONVERT_MIN_RGB, CONVERT_COLORSPACE,
   CONVERT_NORMALIZE_ONLY, CONVERT_DUDV_TO_NORMAL, CONVERT_HEIGHTMAP,
   MAX_CONVERSION_TYPE
};

enum DUDV_TYPE
{
   DUDV_NONE, DUDV_8BIT_SIGNED, DUDV_8BIT_UNSIGNED, DUDV_16BIT_SIGNED,
   DUDV_16BIT_UNSIGNED,
   MAX_DUDV_TYPE
};

void blur(gint32 image_ID, gint32 diffuse_ID, gfloat width, gfloat height, gint32 passes, gint32 normal, gint defAO);
void doBaseMap(gint32 image_ID, gint32 diffuse_ID, gfloat Depth, gint32 passes, gint defAO);
void shapeRecognise(gint32 image_ID, gint32 normalmap_ID, gdouble strength, gint defAO);
void sharpen(gint32 image_ID, gint32 diffuse, gfloat depth, gint32 filterSize, gdouble strength);
gint32 normalmap(gint32 drawable_ID, gboolean preview_mode);
gint32 render(gint32 img_ID, PlugInVals *vals);
gint32 specularEdge(gint32 image_ID, const gchar *file_name, gint defin);
gint32 specularSmooth(gint32 image_ID, const gchar *file_name, gint defin);
void removeShading(gint32 image_ID);
void removeShadingPreview(gint32 image_ID, gint32 noise_val);
gint32 specularEdgeWorker(gint32 image_ID, gint defin, gint defAO, gint32 bShowProgress);
gint32 specularSmoothWorker(gint32 image_ID, gint defin, gint defAO, gint32 bShowProgress);
void removeGivenLayerFromImage(gint32 image_ID, gint32 layer_ID);

#endif /** INSANE_BUMP_HEADER_GUARD */

