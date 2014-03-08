/************************************************************************
 *   insanemap GIMP plugin
 *
 *   Copyright (C) 2002-2008 Shawn Kirst <skirst@insightbb.com>
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
 
#include <stdlib.h>
#include <string.h>

#include "PluginConnectors.h"

/* 
 * I found this function in curve-bend.c in the gimp-2.6.11/plug-ins/common
 * directory of the source code for gimp.
 * - Derby Russell
 * 
 * ============================================================================
 * p_pdb_procedure_available
 *   if requested procedure is available in the PDB return the number of args
 *      (0 upto n) that are needed to call the procedure.
 *   if not available return -1
 * ============================================================================
 * @return gint - number of parameters for success, else -1
 */

static gint p_pdb_procedure_available (const gchar *proc_name)
{
    gint             l_nparams;
    gint             l_nreturn_vals;
    GimpPDBProcType  l_proc_type;
    gchar           *l_proc_blurb;
    gchar           *l_proc_help;
    gchar           *l_proc_author;
    gchar           *l_proc_copyright;
    gchar           *l_proc_date;
    GimpParamDef    *l_params;
    GimpParamDef    *l_return_vals;

    /* Query the gimp application's procedural database
    *  regarding a particular procedure.
    */
    if (gimp_procedural_db_proc_info (proc_name,
                                      &l_proc_blurb,
                                      &l_proc_help,
                                      &l_proc_author,
                                      &l_proc_copyright,
                                      &l_proc_date,
                                      &l_proc_type,
                                      &l_nparams, &l_nreturn_vals,
                                      &l_params, &l_return_vals))
    {
        /* procedure found in PDB */
        return l_nparams;
    }
    return -1;
}

/**
 * Colors ->  Components -> "Channel Mixer" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_colors_channel_mixer(image_ID, drawable_ID, 0, -200.0f, 0.0f, 0.0f, 0.0f, -200.0f, 0.0f, 0.0f, 0.0f, 1.0f);
 * 
 * From channel-mixer.c in the gimp-2.6.11/plug-ins/common directory.
 * 
 *   static const GimpParamDef args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode",   "Interactive, non-interactive" },
 *     { GIMP_PDB_IMAGE,    "image",      "Input image (unused)" },
 *     { GIMP_PDB_DRAWABLE, "drawable",   "Input drawable" },
 *     { GIMP_PDB_INT32,    "monochrome", "Monochrome (TRUE or FALSE)" },
 *     { GIMP_PDB_FLOAT,    "rr-gain",    "Set the red gain for the red channel" },
 *     { GIMP_PDB_FLOAT,    "rg-gain",    "Set the green gain for the red channel" },
 *     { GIMP_PDB_FLOAT,    "rb-gain",    "Set the blue gain for the red channel" },
 *     { GIMP_PDB_FLOAT,    "gr-gain",    "Set the red gain for the green channel" },
 *     { GIMP_PDB_FLOAT,    "gg-gain",    "Set the green gain for the green channel" },
 *     { GIMP_PDB_FLOAT,    "gb-gain",    "Set the blue gain for the green channel" },
 *     { GIMP_PDB_FLOAT,    "br-gain",    "Set the red gain for the blue channel" },
 *     { GIMP_PDB_FLOAT,    "bg-gain",    "Set the green gain for the blue channel" },
 *     { GIMP_PDB_FLOAT,    "bb-gain",    "Set the blue gain for the blue channel" }
 *   };
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_colors_channel_mixer_connector(gint32 image_ID,
             gint32 drawable_ID, gint32 isMonochrome,
             gfloat rr_gain, gfloat rg_gain, gfloat rb_gain,
             gfloat gr_gain, gfloat gg_gain, gfloat gb_gain,
             gfloat br_gain, gfloat bg_gain, gfloat bb_gain)
{
    /** This name comes from the definition in channel-mixer.c */
    static gchar *l_channel_mixer_proc = "plug-in-colors-channel-mixer";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_channel_mixer_proc);
    if (l_nparams == 13)
    {
        return_vals = gimp_run_procedure (l_channel_mixer_proc,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, drawable_ID,
                                          GIMP_PDB_INT32, isMonochrome,
                                          GIMP_PDB_FLOAT, rr_gain,
                                          GIMP_PDB_FLOAT, rg_gain,
                                          GIMP_PDB_FLOAT, rb_gain,
                                          GIMP_PDB_FLOAT, gr_gain,
                                          GIMP_PDB_FLOAT, gg_gain,
                                          GIMP_PDB_FLOAT, gb_gain,
                                          GIMP_PDB_FLOAT, br_gain,
                                          GIMP_PDB_FLOAT, bg_gain,
                                          GIMP_PDB_FLOAT, bb_gain,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter "Difference of Gaussians" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_dog(image_ID, newlayer_ID, 8.0f, 1.0f, 1, 0);
 * 
 * From the edge-dog.c file in the gimp-2.6.11/plug-ins/common directory:
 * 
 *   static const GimpParamDef args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode",  "Interactive, non-interactive" },
 *     { GIMP_PDB_IMAGE,    "image",     "Input image" },
 *     { GIMP_PDB_DRAWABLE, "drawable",  "Input drawable" },
 *     { GIMP_PDB_FLOAT,    "inner",     "Radius of inner gaussian blur (in pixels, > 0.0)" },
 *     { GIMP_PDB_FLOAT,    "outer",     "Radius of outer gaussian blur (in pixels, > 0.0)" },
 *     { GIMP_PDB_INT32,    "normalize", "True, False" },
 *     { GIMP_PDB_INT32,    "invert",    "True, False" }
 *   };
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_dog_connector(gint32 image_ID, gint32 newlayer_ID, gfloat inner, gfloat outer, gint32 isNormalize, gint32 isInvert)
{
    /** This name comes from the definition in edge-dog.c */
    static gchar *l_dog = "plug-in-dog";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_dog);
    if (l_nparams == 7)
    {
        return_vals = gimp_run_procedure (l_dog,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, newlayer_ID,
                                          GIMP_PDB_FLOAT, inner,
                                          GIMP_PDB_FLOAT, outer,
                                          GIMP_PDB_INT32, isNormalize,
                                          GIMP_PDB_INT32, isInvert,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter "Gaussian Blur" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_gauss(image_ID, newlayer_ID, 20.0f, 20.0f, 0)
 * 
 * 
 * From the blur-gauss.c file in the gimp-2.6.11/plug-ins/common directory:
 * 
 *   static const GimpParamDef args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode",   "Interactive, non-interactive" },
 *     { GIMP_PDB_IMAGE,    "image",      "Input image" },
 *     { GIMP_PDB_DRAWABLE, "drawable",   "Input drawable" },
 *     { GIMP_PDB_FLOAT,    "horizontal", "Horizontal radius of gaussian blur (in pixels, > 0.0)" },
 *     { GIMP_PDB_FLOAT,    "vertical",   "Vertical radius of gaussian blur (in pixels, > 0.0)" },
 *     { GIMP_PDB_INT32,    "method",     "IIR (0) or RLE (1)" }
 *   };
 * 
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_gauss_connector(gint32 image_ID, gint32 newlayer_ID, gfloat horizontal, gfloat vertical, gint32 isMethod)
{
    static gchar *l_gauss = "plug-in-gauss";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_gauss);
    if (l_nparams == 6)
    {
        return_vals = gimp_run_procedure (l_gauss,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, newlayer_ID,
                                          GIMP_PDB_FLOAT, horizontal,
                                          GIMP_PDB_FLOAT, vertical,
                                          GIMP_PDB_INT32, isMethod,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Colors Menu->Invert
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_vinvert(image_ID, newlayer_ID);
 * 
 * From the value-invert.c file in the gimp-2.6.11/plug-ins/common directory:
 * 
 *   static const GimpParamDef args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode", "Interactive, non-interactive"          },
 *     { GIMP_PDB_IMAGE,    "image",    "Input image (used for indexed images)" },
 *     { GIMP_PDB_DRAWABLE, "drawable", "Input drawable"                        }
 *   };
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_vinvert_connector(gint32 image_ID, gint32 newlayer_ID)
{
    static gchar *l_vinvert = "plug-in-vinvert";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_vinvert);
    if (l_nparams == 3)
    {
        return_vals = gimp_run_procedure (l_vinvert,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, newlayer_ID,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter "RGB Noise" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_rgb_noise(image_ID, noiselayer_ID, 1, 1, 0.20f, 0.20f, 0.20f, 0.0f);
 * 
 * From the noise-rgb.c file in the gimp-2.6.11/plug-ins/common directory:
 * 
 *   static const GimpParamDef scatter_args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode",    "Interactive, non-interactive" },
 *     { GIMP_PDB_IMAGE,    "image",       "Input image (unused)" },
 *     { GIMP_PDB_DRAWABLE, "drawable",    "Input drawable" },
 *     { GIMP_PDB_INT32,    "independent", "Noise in channels independent" },
 *     { GIMP_PDB_INT32,    "correlated",  "Noise correlated (i.e. multiplicative not additive)" },
 *     { GIMP_PDB_FLOAT,    "noise-1",     "Noise in the first channel (red, gray)" },
 *     { GIMP_PDB_FLOAT,    "noise-2",     "Noise in the second channel (green, gray_alpha)" },
 *     { GIMP_PDB_FLOAT,    "noise-3",     "Noise in the third channel (blue)" },
 *     { GIMP_PDB_FLOAT,    "noise-4",     "Noise in the fourth channel (alpha)" }
 *   };
 * 
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_rgb_noise_connector(gint32 image_ID, gint32 noiselayer_ID, gint32 independent,
             gint32 correlated, gfloat noise_1 , gfloat noise_2, gfloat noise_3, gfloat noise_4)
{
    static gchar *l_noise = "plug-in-rgb-noise";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_noise);
    if (l_nparams == 9)
    {
        return_vals = gimp_run_procedure (l_noise,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, noiselayer_ID,
                                          GIMP_PDB_INT32, independent,
                                          GIMP_PDB_INT32, correlated,
                                          GIMP_PDB_FLOAT, noise_1,
                                          GIMP_PDB_FLOAT, noise_2,
                                          GIMP_PDB_FLOAT, noise_3,
                                          GIMP_PDB_FLOAT, noise_4,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter "Tile Seamless" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_make_seamless(image_ID, diffuse_ID);
 * 
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_make_seamless_connector(gint32 image_ID, gint32 diffuse_ID)
{
    static gchar *l_make_seamless = "plug-in-make-seamless";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_make_seamless);
    if (l_nparams == 3)
    {
        return_vals = gimp_run_procedure (l_make_seamless,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, diffuse_ID,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter "Blur" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_blur(image_ID, normalmap_ID);
 * 
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_blur_connector(gint32 image_ID, gint32 normalmap_ID)
{
    static gchar *l_blur = "plug-in-blur";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_blur);
    if (l_nparams == 3)
    {
        return_vals = gimp_run_procedure (l_blur,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, normalmap_ID,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Filter Enhance "Sharpen" applied
 * Standard plug-in. Source code ships with GIMP.
 * 
 * usage example of plug-in:
 * plug_in_sharpen(image_ID, normalmap_ID, 20);
 * 
 * From the sharpen.c file in the gimp-2.6.11/plug-ins/common directory:
 * 
 *   static const GimpParamDef   args[] =
 *   {
 *     { GIMP_PDB_INT32,    "run-mode", "Interactive, non-interactive"      },
 *     { GIMP_PDB_IMAGE,    "image",    "Input image"                       },
 *     { GIMP_PDB_DRAWABLE, "drawable", "Input drawable"                    },
 *     { GIMP_PDB_INT32,    "percent",  "Percent sharpening (default = 10)" }
 *   };
 * 
 * @return gint32 - 1 for success, else 0
 */
gint32 plug_in_sharpen_connector(gint32 image_ID, gint32 normalmap_ID, gint32 percent)
{
    static gchar *l_sharpen = "plug-in-sharpen";
    GimpParam    *return_vals;
    gint          nreturn_vals;
    gint          l_nparams;

    l_nparams = p_pdb_procedure_available (l_sharpen);
    if (l_nparams == 4)
    {
        return_vals = gimp_run_procedure (l_sharpen,
                                          &nreturn_vals,
                                          GIMP_PDB_INT32, GIMP_RUN_NONINTERACTIVE,
                                          GIMP_PDB_IMAGE, image_ID,
                                          GIMP_PDB_DRAWABLE, normalmap_ID,
                                          GIMP_PDB_INT32, percent,
                                          GIMP_PDB_END);

        if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
        {
            return 1;
        }
    }
    
    return 0;
}

