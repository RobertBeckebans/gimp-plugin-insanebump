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

#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"
#include "InsaneBump.h"

#include "plugin-intl.h"


/*  Constants  */

#define DATA_KEY_VALS    "plug_in_insanebump"

#define PLUG_IN_PROC    "plug-in-insanebump"
#define PLUG_IN_BINARY  "gimp_plugin_insanebump"
#define PLUG_IN_VERSION "1.0.6 - 7 December 2013"
#define PLUG_IN_LOCALDIR "<Image>/Filters/Map"
#define PLUG_IN_AUTHOR_LINEONE "Derby Russell <jdrussell51@gmail.com>"
#define PLUG_IN_AUTHOR_LINETWO "Copyright 2013 Derby Russell"


/*  Local function prototypes  */

static void   query (void);
static void   run   (const gchar      *name,
                     gint              nparams,
                     const GimpParam  *param,
                     gint             *nreturn_vals,
                     GimpParam       **return_vals);


/*  Local variables  */

NormalmapVals nmapvals =
{
    0,
    0.0,
    1.0,
    0,
    0,
    ALPHA_NONE,
    CONVERT_NONE,
    DUDV_NONE,
    0,
    0,
    0,
    0.0,
    0
};

const PlugInVals default_vals =
{
    0,
    0,
    0,
    2,
    1,
    64,
    20.00f,
    3,
    50,
    50,
    50,
    1,
    0,
    0,
    50,
    0,
    1
};

static PlugInVals vals;


GimpPlugInInfo PLUG_IN_INFO =
{
    NULL,  /* init_proc  */
    NULL,  /* quit_proc  */
    query, /* query_proc */
    run,   /* run_proc   */
};

MAIN ()

static void
query (void)
{
    // gchar *help_path;
    // gchar *help_uri;

    static GimpParamDef args[] =
    {
        {GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive"},
        {GIMP_PDB_IMAGE, "image", "Input image"},
        {GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
        {GIMP_PDB_INT32, "remove_lighting", "Remove Lighting TRUE or FALSE default FALSE"},
        {GIMP_PDB_INT32, "resizie", "Upscale(HD) TRUE or FALSE default FALSE"},
        {GIMP_PDB_INT32, "tile", "Tile TRUE or FALSE default FALSE"},
        {GIMP_PDB_INT32, "new_width", "New Width(Integer Times larger) default 2"},
        {GIMP_PDB_INT32, "edge_specular", "Edge Enhancing Specular TRUE or FALSE default TRUE"},
        {GIMP_PDB_INT32, "def_specular", "Specular Definition(0-255) default 64"},
        {GIMP_PDB_FLOAT, "depth", "Depth(+/-) default 20.00"},
        {GIMP_PDB_INT32, "large_details", "Large Detail Size default 3"},
        {GIMP_PDB_INT32, "medium_details", "Medium Detail Intensity(%) default 50"},
        {GIMP_PDB_INT32, "small_details", "Small Detail Intensity(%) default 50"},
        {GIMP_PDB_INT32, "shape_recog", "Shape Recognition(%) default 50"},
        {GIMP_PDB_INT32, "smoothstep", "Smooth Step TRUE or FALSE default TRUE"},
        {GIMP_PDB_INT32, "noise", "Noise TRUE or FALSE default FALSE"},
        {GIMP_PDB_INT32, "invh", "Invert Height Map TRUE or FALSE default FALSE"},
        {GIMP_PDB_INT32, "ao", "ao default 50"},
        {GIMP_PDB_INT32, "prev", "Preview TRUE or FALSE default TRUE"},
    };

    gimp_plugin_domain_register (PLUG_IN_PROC, LOCALEDIR);

    // help_path = g_build_filename (DATADIR, "help", NULL);
    // help_uri = g_filename_to_uri (help_path, NULL, NULL);
    // g_free (help_path);
    // 
    // gimp_plugin_help_register ("http://developer.gimp.org/plug-in-insanebump/help",
    //                            help_uri);

    gimp_install_procedure (PLUG_IN_PROC,
                            N_("Produce images needed for Graphical Rendering"),
                            "This plug-in automatically produces specular, "
                            "normal and other images from current image. ",
                            PLUG_IN_AUTHOR_LINEONE,
                            PLUG_IN_AUTHOR_LINETWO,
                            "2013",
                            N_("_InsaneBump..."),
                            "RGB*, GRAY*, INDEXED*",
                            GIMP_PLUGIN,
                            G_N_ELEMENTS (args), 0,
                            args, NULL);

    gimp_plugin_menu_register (PLUG_IN_PROC, PLUG_IN_LOCALDIR);
}

static void
run (const gchar      *name,
     gint              n_params,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
    static GimpParam   values[1];
    GimpDrawable      *drawable = NULL;
    gint32             image_ID;
    GimpRunMode        run_mode;
    GimpPDBStatusType  status = GIMP_PDB_SUCCESS;
    gint nDlgResponse = -5;
    
    init_drawables();

    *nreturn_vals = 1;
    *return_vals  = values;

    /*  Initialize i18n support  */
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
    textdomain (GETTEXT_PACKAGE);

    run_mode = param[0].data.d_int32;
    image_ID = param[1].data.d_int32;
    drawable = gimp_drawable_get (param[2].data.d_drawable);
    gimp_tile_cache_ntiles (2 * drawable->ntile_cols);


    /*  Initialize with default values  */
    copyPlugInVals(&default_vals, &vals);

    vals.image_ID = image_ID;

    if (strcmp (name, PLUG_IN_PROC) == 0)
    {
        switch (run_mode)
        {
        case GIMP_RUN_NONINTERACTIVE:
            if (n_params != 16)
            {
                status = GIMP_PDB_CALLING_ERROR;
            }
            else
            {
                vals.RemoveLighting = param[3].data.d_int32;
                vals.Resizie = param[4].data.d_int32;
                vals.Tile = param[5].data.d_int32;
                vals.newWidth = param[6].data.d_int32;
                vals.EdgeSpecular = param[7].data.d_int32;
                vals.defSpecular = param[8].data.d_int32;
                vals.Depth = param[9].data.d_float;
                vals.LargeDetails = param[10].data.d_int32;
                vals.MediumDetails = param[11].data.d_int32;
                vals.SmallDetails = param[12].data.d_int32;
                vals.ShapeRecog = param[13].data.d_int32;
                vals.smoothstep = param[14].data.d_int32;
                vals.Noise = param[15].data.d_int32;
                vals.invh = param[16].data.d_int32;
                vals.ao = param[17].data.d_int32;
                vals.prev = param[18].data.d_int32;
            }
            break;

        case GIMP_RUN_INTERACTIVE:
            gimp_ui_init(PLUG_IN_BINARY, 0);
            /*  Possibly retrieve data  */
            gimp_get_data (DATA_KEY_VALS,    &vals);
            nDlgResponse = InsaneBumpDialog(drawable, PLUG_IN_BINARY, &vals);
            switch(nDlgResponse)
            {
            case 0:
                status = GIMP_PDB_EXECUTION_ERROR;
                values[0].type = GIMP_PDB_STATUS;
                values[0].data.d_status = status;
                gimp_drawable_detach(drawable);
                return;
            case 1:
                gimp_set_data(DATA_KEY_VALS, &vals, sizeof(vals));
            default:
                values[0].type = GIMP_PDB_STATUS;
                values[0].data.d_status = status;
                gimp_drawable_detach(drawable);
                gimp_displays_flush();
                return;
            }
            break;

        case GIMP_RUN_WITH_LAST_VALS:
            /*  Possibly retrieve data  */
            gimp_get_data (DATA_KEY_VALS, &vals);
            break;

        default:
            break;
        }
    }
    else
    {
        status = GIMP_PDB_CALLING_ERROR;
    }

    if (status == GIMP_PDB_SUCCESS)
    {
        render(image_ID, &vals);

        if (run_mode != GIMP_RUN_NONINTERACTIVE)
            gimp_displays_flush ();

        if (run_mode == GIMP_RUN_INTERACTIVE)
        {
            gimp_set_data(DATA_KEY_VALS, &vals, sizeof (vals));
        }

        gimp_drawable_detach (drawable);
    }

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = status;
}

void copyPlugInVals(const PlugInVals *some_vals_source, PlugInVals *some_vals_destination)
{
    if (some_vals_source == NULL) return;
    if (some_vals_destination == NULL) return;
    some_vals_destination->RemoveLighting = some_vals_source->RemoveLighting;
    some_vals_destination->Resizie        = some_vals_source->Resizie;
    some_vals_destination->Tile           = some_vals_source->Tile;
    some_vals_destination->newWidth       = some_vals_source->newWidth;
    some_vals_destination->EdgeSpecular   = some_vals_source->EdgeSpecular;
    some_vals_destination->defSpecular    = some_vals_source->defSpecular;
    some_vals_destination->Depth          = some_vals_source->Depth;
    some_vals_destination->LargeDetails   = some_vals_source->LargeDetails;
    some_vals_destination->MediumDetails  = some_vals_source->MediumDetails;
    some_vals_destination->SmallDetails   = some_vals_source->SmallDetails;
    some_vals_destination->ShapeRecog     = some_vals_source->ShapeRecog;
    some_vals_destination->smoothstep     = some_vals_source->smoothstep;
    some_vals_destination->Noise          = some_vals_source->Noise;
    some_vals_destination->invh           = some_vals_source->invh;
    some_vals_destination->ao             = some_vals_source->ao;
    some_vals_destination->prev           = some_vals_source->prev;
    some_vals_destination->image_ID       = some_vals_source->image_ID;
}

