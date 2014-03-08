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

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"
#include "scale.h"
#include "InsaneBump.h"

#include "plugin-intl.h"
#include "PluginConnectors.h"

/*  Constants  */


/*  Local function prototypes  */

/*  Global variables  */
PlugInVals local_vals;
int update_preview = 0;
int preview_progress_reset = 1;
gint dialog_is_init = 0 ;
GtkWidget *g_gwRemoveLtgBtn = NULL;
GtkWidget *g_gwUpscaleBtn = NULL;
GtkWidget *g_gwTileBtn = NULL;
GtkWidget *g_gwEdgeBtn = NULL;
GtkWidget *g_gwSmoothBtn = NULL;
GtkWidget *g_gwNoiseBtn = NULL;
GtkWidget *g_gwInvertBtn = NULL;
GtkWidget *g_gwWidthSpin = NULL;
GtkWidget *g_gwSpecDefSpin = NULL;
GtkWidget *g_gwDepthSpin = NULL;
GtkWidget *g_gwLargeDSpin = NULL;
GtkWidget *g_gwMediumDSpin = NULL;
GtkWidget *g_gwSmallDSpin = NULL;
GtkWidget *g_gwShapeSpin = NULL;
GtkWidget *g_gwAOSpin = NULL;

/*  Local variables  */
static GtkWidget *dialog = NULL;
GtkWidget *progress = NULL;
GtkWidget *progress_label = NULL;
gint runme = 0;
int preview_progress_timer = 0 ;


/*  Private functions  */
static void do_cleanup(gpointer data)
{
   gtk_main_quit();
   _active = 'x';
}

static gint idle_callback(gpointer data)
{
   if ((update_preview) && (_active == 'x'))
   {
        preview_redraw();
   }
   if (preview_progress_reset == 0) {
       if (preview_progress_timer < 5) {
           preview_progress_timer++;
       } else {
           preview_progress_reset = 1;
           preview_progress_timer = 0 ;
           gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.0);
       }
   }
   return(1);
}

static void new_width_changed(GtkWidget *widget, gpointer data)
{
   local_vals.newWidth = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void def_specular_changed(GtkWidget *widget, gpointer data)
{
   local_vals.defSpecular = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void depth_changed(GtkWidget *widget, gpointer data)
{
   local_vals.Depth = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void large_detail_size_changed(GtkWidget *widget, gpointer data)
{
   local_vals.LargeDetails = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void medium_detail_intensity_changed(GtkWidget *widget, gpointer data)
{
   local_vals.MediumDetails = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void small_detail_intensity_changed(GtkWidget *widget, gpointer data)
{
   local_vals.SmallDetails = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void shape_recognition_changed(GtkWidget *widget, gpointer data)
{
   local_vals.ShapeRecog = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void ao_changed(GtkWidget *widget, gpointer data)
{
   local_vals.ao = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   _active = 'x';
   update_preview = 1;
}

static void insanebump_dialog_response(GtkWidget *widget, gint response_id,
                                      gpointer data)
{
   switch(response_id)
   {
      case GTK_RESPONSE_OK:
         // gimp_progress_init("Creating InsaneBump...");
         runme = 2;
         if (render(local_vals.image_ID, &local_vals) == 0)
         {
             runme = 1;
         }
         removeAllLayersExceptMain();
         preview_progress_reset = 0 ;
         return;
      default:
         runme = 2;
         gtk_widget_destroy(widget);
         _active = 'x';
         break;
   }
}

/**
 * To understand how to setup this function you must understand:
 * 
 * gimp_table_attach_aligned
 * 
 * To understand that you will find information in only one spot:
 * 
 * c:\gimp-2.6.11\libgimpwidgets\gimpwidgets.c on or about line number 1118
 * which states:
 * 
 **
 * gimp_table_attach_aligned:
 * @table:      The #GtkTable the widgets will be attached to.
 * @column:     The column to start with.
 * @row:        The row to attach the widgets.
 * @label_text: The text for the #GtkLabel which will be attached left of
 *              the widget.
 * @xalign:     The horizontal alignment of the #GtkLabel.
 * @yalign:     The vertival alignment of the #GtkLabel.
 * @widget:     The #GtkWidget to attach right of the label.
 * @colspan:    The number of columns the widget will use.
 * @left_align: %TRUE if the widget should be left-aligned.
 *
 * Note that the @label_text can be %NULL and that the widget will be
 * attached starting at (@column + 1) in this case, too.
 *
 * Returns: The created #GtkLabel.
 **
 * GtkWidget *
 * gimp_table_attach_aligned (GtkTable    *table,
 *                            gint         column,
 *                            gint         row,
 *                            const gchar *label_text,
 *                            gfloat       xalign,
 *                            gfloat       yalign,
 *                            GtkWidget   *widget,
 *                            gint         colspan,
 *                            gboolean     left_align);
 * 
 */
static GtkWidget *CreateToggleButton(GtkWidget *table, gint row, GCallback *function, const gchar *szName, gint32 nVal)
{
    GtkWidget *btn = NULL;
    if (nVal == 1)
    {
       btn = gtk_toggle_button_new_with_label("Yes");
    }
    else
    {
       btn = gtk_toggle_button_new_with_label("No");
    }
    if (btn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn), "clicked",
                          GTK_SIGNAL_FUNC(function), 0);
        gimp_table_attach_aligned(GTK_TABLE(table), 0, row, szName, 0, 0.5,
                                 btn, 1, 0);
        gtk_widget_show(btn);
        if (nVal == 1)
        {
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn), TRUE);
        }
    }
    return btn;
}

/**
 * To understand how to setup this function you must understand:
 * 
 * gimp_table_attach_aligned
 * 
 * To understand that you will find information in only one spot:
 * 
 * c:\gimp-2.6.11\libgimpwidgets\gimpwidgets.c on or about line number 1118
 * which states:
 * 
 **
 * gimp_table_attach_aligned:
 * @table:      The #GtkTable the widgets will be attached to.
 * @column:     The column to start with.
 * @row:        The row to attach the widgets.
 * @label_text: The text for the #GtkLabel which will be attached left of
 *              the widget.
 * @xalign:     The horizontal alignment of the #GtkLabel.
 * @yalign:     The vertival alignment of the #GtkLabel.
 * @widget:     The #GtkWidget to attach right of the label.
 * @colspan:    The number of columns the widget will use.
 * @left_align: %TRUE if the widget should be left-aligned.
 *
 * Note that the @label_text can be %NULL and that the widget will be
 * attached starting at (@column + 1) in this case, too.
 *
 * Returns: The created #GtkLabel.
 **
 * GtkWidget *
 * gimp_table_attach_aligned (GtkTable    *table,
 *                            gint         column,
 *                            gint         row,
 *                            const gchar *label_text,
 *                            gfloat       xalign,
 *                            gfloat       yalign,
 *                            GtkWidget   *widget,
 *                            gint         colspan,
 *                            gboolean     left_align);
 * 
 */
static GtkWidget *CreateInteger255SpinButton(GtkWidget *table, gint row, GCallback *function, const gchar *szName, gint32 nVal, gint32 allow_neg)
{
   GtkObject *adj = NULL;
   GtkWidget *spin = NULL;
   if (allow_neg)
   {
       adj = gtk_adjustment_new((gdouble)nVal, -1000, 1000, 1, 10, 0);
   }
   else
   {
       adj = gtk_adjustment_new((gdouble)nVal, 0, 1000, 1, 10, 0);
   }
   spin = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
   gtk_widget_show(spin);
   gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin), GTK_UPDATE_IF_VALID);
   gtk_signal_connect(GTK_OBJECT(spin), "value_changed",
                      GTK_SIGNAL_FUNC(function), 0);
   gimp_table_attach_aligned(GTK_TABLE(table), 0, row, szName, 0, 0.5,
                             spin, 1, 0);
   
   return spin;
}

/**
 * table_attach_progress:
 * @table:      The #GtkTable the widgets will be attached to.
 * @column:     The column to start with.
 * @row:        The row to attach the widgets.
 * @label_text: The text for the #GtkLabel which will be attached left of
 *              the widget.
 * @xalign:     The horizontal alignment of the #GtkLabel.
 * @yalign:     The vertival alignment of the #GtkLabel.
 * @widget:     The #GtkWidget to attach right of the label.
 * @colspan:    The number of columns the widget will use.
 * @left_align: %TRUE if the widget should be left-aligned.
 *
 * Note that the @label_text can be %NULL and that the widget will be
 * attached starting at (@column + 1) in this case, too.
 *
 * Returns: The created #GtkLabel.
 **/
static void table_attach_progress(GtkTable    *table,
                           gint         column,
                           gint         row,
                           const gchar *label_text,
                           gfloat       xalign,
                           gfloat       yalign,
                           GtkWidget   *widget,
                           gint         colspan,
                           gboolean     left_align)
{
    if (label_text)
    {
        progress_label = gtk_label_new_with_mnemonic (label_text);
        gtk_misc_set_alignment (GTK_MISC (progress_label), xalign, yalign);
        gtk_label_set_justify (GTK_LABEL (progress_label), GTK_JUSTIFY_LEFT);
        gtk_table_attach (table, progress_label,
                          column, column + 1,
                          row, row + 1,
                          GTK_FILL, GTK_FILL, 0, 0);
        gtk_widget_show (progress_label);
    }

    if (left_align)
    {
        GtkWidget *hbox = gtk_hbox_new (FALSE, 0);

        gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 0);
        gtk_widget_show (widget);

        widget = hbox;
    }

    gtk_table_attach (table, widget,
                      column, column + 1 + colspan + 1,
                      row + 1, row + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    gtk_widget_show (widget);
}

/*  Public functions  */

gint InsaneBumpDialog(GimpDrawable *drawable,
                      const gchar *szBinary,
                      PlugInVals *vals)
{
    /** Adding the preview area for the second release*/
    GtkWidget *hbox;
    GtkWidget *table;
    
    hbox = NULL;

    if(_active != 'x') return 2;

    copyPlugInVals(vals, &local_vals);

    gimp_ui_init(szBinary, TRUE);

    dialog = gimp_dialog_new(NAME_AND_VERSION, szBinary,
                             0, 0, gimp_standard_help_func, 0,
                             GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL,
                             GTK_STOCK_EXECUTE, GTK_RESPONSE_OK,
                             NULL);

    gtk_signal_connect(GTK_OBJECT(dialog), "response",
                       GTK_SIGNAL_FUNC(insanebump_dialog_response),
                       0);
    gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
                       GTK_SIGNAL_FUNC(do_cleanup),
                       0);

    hbox = gtk_hbox_new(0, 8);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox, 1, 1, 0);
    gtk_widget_show(hbox);

    /** Adding the preview area for the second release. */   
    /**
     * Create the Preview area.
     */
    /** Creates a new vbox put into hbox */
    CreateLeftPreviewFrames(hbox);
    /** Creates a new vbox put into hbox */
    CreateRightPreviewToggleButton(hbox);

    /**
     * Create a table to place all the right hand items into.
     * Really could just be a horizontal box with all packed into.
     * Copied other persons code.
     */
    table = gtk_table_new(18, 1, 0);
    gtk_widget_show(table);
    gtk_box_pack_start(GTK_BOX(hbox), table, 1, 1, 0);
    gtk_table_set_row_spacings(GTK_TABLE(table), 8);
    gtk_table_set_col_spacings(GTK_TABLE(table), 8);

    /**
     * Remove Lighting
     * [         No           ]
     * button
     */
    g_gwRemoveLtgBtn = CreateToggleButton(table, 1, (GCallback *)&remlightbtn_clicked, "Remove Lighting", vals->RemoveLighting);

    /**
     * Upscale(HD)
     * [         No           ]
     * button
     */
    g_gwUpscaleBtn = CreateToggleButton(table, 2, (GCallback *)&upscale_HD_clicked, "Upscale(HD)", vals->Resizie);

    /**
     * Tile
     * [*********Yes***********]
     * button
     */
    g_gwTileBtn = CreateToggleButton(table, 3, (GCallback *)&tile_clicked, "Tile", vals->Tile);

    /**
     * New Width(Integer Times larger)
     * [2                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwWidthSpin = CreateInteger255SpinButton(table, 4, (GCallback *)&new_width_changed, "New Width(Integer Times larger)", vals->newWidth, 0);

    /**
     * Edge Enhaning Specular
     * [*********Yes***********]
     * button
     */
    g_gwEdgeBtn = CreateToggleButton(table, 5, (GCallback *)&edge_enhancing_specular_clicked, "Edge Enhancing Specular", vals->EdgeSpecular);

    /**
     * Specular Definition(0-255)
     * [64                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwSpecDefSpin = CreateInteger255SpinButton(table, 6, (GCallback *)&def_specular_changed, "Specular Definition(0-255)", vals->defSpecular, 0);

    /**
     * Depth(+/-)
     * [20                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwDepthSpin = CreateInteger255SpinButton(table, 7, (GCallback *)&depth_changed, "Depth(+/-)", vals->Depth, 1);

    /**
     * Large Detail Size
     * [3                      ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwLargeDSpin = CreateInteger255SpinButton(table, 8, (GCallback *)&large_detail_size_changed, "Large Detail Size", vals->LargeDetails, 0);

    /**
     * Medium Detail Intensity(%)
     * [50                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwMediumDSpin = CreateInteger255SpinButton(table, 9, (GCallback *)&medium_detail_intensity_changed, "Medium Detail Intensity(%)", vals->MediumDetails, 0);

    /**
     * Small Detail Intensity(%)
     * [50                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwSmallDSpin = CreateInteger255SpinButton(table, 10, (GCallback *)&small_detail_intensity_changed, "Small Detail Intensity(%)", vals->SmallDetails, 0);

    /**
     * Shape Recognition(%)
     * [50                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwShapeSpin = CreateInteger255SpinButton(table, 11, (GCallback *)&shape_recognition_changed, "Shape Recognition(%)", vals->ShapeRecog, 0);

    /**
     * Smooth Step
     * [*********Yes***********]
     * button
     */
    g_gwSmoothBtn = CreateToggleButton(table, 12, (GCallback *)&smooth_step_clicked, "Smooth Step", vals->smoothstep);

    /**
     * Noise
     * [         No           ]
     * button
     */
    g_gwNoiseBtn = CreateToggleButton(table, 13, (GCallback *)&noise_clicked, "Noise", vals->Noise);

    /**
     * Invert Height Map
     * [         No           ]
     * button
     */
    g_gwInvertBtn = CreateToggleButton(table, 14, (GCallback *)&invert_height_map_clicked, "Invert Height Map", vals->invh);

    /**
     * ao
     * [50                     ][^][v]
     * edit control with spinner arrows at end of control
     */
    g_gwAOSpin = CreateInteger255SpinButton(table, 15, (GCallback *)&ao_changed, "ao", vals->ao, 0);

    progress = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.0);
    table_attach_progress(GTK_TABLE(table), 0, 17, "Idle...", 0, 0.5,
                          progress, 1, 0);
    gtk_widget_show(progress);

    gtk_widget_show(dialog);

    update_preview = 1;

    gtk_timeout_add(300, idle_callback, drawable);
   
    runme = 0;

    dialog_is_init = 1 ;
    
    _active = 'x';
    
    gtk_main();

    return(runme);
}

/**
 * The button handler callback for the Remove Lighting toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.RemoveLighting. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Remove Lighting button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void remlightbtn_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.RemoveLighting == 0)
    {
        local_vals.RemoveLighting = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.RemoveLighting = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Upscale(HD) toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.Resizie. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Upscale(HD) button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void upscale_HD_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.Resizie == 0)
    {
        local_vals.Resizie = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.Resizie = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Tile toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.Tile. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Tile button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void tile_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.Tile == 0)
    {
        local_vals.Tile = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.Tile = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Edge Enhancing Specular toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.EdgeSpecular. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Edge Enhancing Specular
 *                            button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void edge_enhancing_specular_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.EdgeSpecular == 0)
    {
        local_vals.EdgeSpecular = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.EdgeSpecular = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Smooth Step toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.smoothstep. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Smooth Step button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void smooth_step_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.smoothstep == 0)
    {
        local_vals.smoothstep = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.smoothstep = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Noise toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.Noise. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Noise button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void noise_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.Noise == 0)
    {
        local_vals.Noise = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.Noise = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}

/**
 * The button handler callback for the Invert Height Map toggle button.
 * The signal handler will return a handle to the button in the
 * widget parameter.<br/>
 * This handler will change the text from "Yes" to "No" and
 * vise a versa.  Depends on the value of local_vals.invh. If
 * this value is 1 then change to "No" and set the value to 0.
 * If the value is 0 then change to "Yes" and set the value to 1.<br/>
 * <br/>
 * You can manually call this function by inserting
 * the handle to the button in the widget parameter and setting the 
 * data parameter to NULL.<br/>
 * 
 * @param GtkWidget *widget - The button pointer for the Invert Height Map
 *                            button.
 * @param GtkWidget *data   - not used, required by gtk signal.
 */
void invert_height_map_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.invh == 0)
    {
        local_vals.invh = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Yes");
    }
    else
    {
        local_vals.invh = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "No");
    }
    _active = 'x';
    update_preview = 1;
}


