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

/**
 * See /home/derby/Documents/InsaneBumpTutorialVideoWatched.txt
 * This explains all the other names of Previews that need to be added.
 */

/*  Constants  */


/*  Local function prototypes  */


/*  Global Variables  */
gchar _active = 'x';
PreviewDraws pDrawables;
GtkWidget *g_gwNormalsFrame = NULL;

/*  Local variables  */
static GtkWidget *btn_n = NULL;
static GtkWidget *btn_ln = NULL;
static GtkWidget *btn_mn = NULL;
static GtkWidget *btn_hn = NULL;
static GtkWidget *btn_sn = NULL;
static gint32 drawableBeginActiveLayer = -1;
static gchar gcNeedNormal = 'n';
static GtkWidget *gwNormalLabel = NULL;

/*  Private functions  */
static void draw_preview_area_update(GtkWidget *preview, GimpDrawable *drawable) {
    if(is_3D_preview_active())
    {
        /** Adding all references to preview for second release. */
        if ((local_vals.image_ID != 0) && (drawable != NULL))
        {
            update_preview = 0;
            if (drawable->bpp == 3) {
                GimpPixelRgn amap_rgn;
                gint rowbytes = PREVIEW_SIZE * 3;
                gint nbytes = drawable->width * drawable->height * 3;
                guchar *tmp = g_new(guchar, nbytes);
                guchar dst[PREVIEW_RGB_SIZE];

                gimp_pixel_rgn_init(&amap_rgn, drawable, 0, 0, drawable->width, drawable->height, 0, 0);
                gimp_pixel_rgn_get_rect(&amap_rgn, tmp, 0, 0, drawable->width, drawable->height);

                scale_pixels(dst, PREVIEW_SIZE, PREVIEW_SIZE, tmp, drawable->width, drawable->height, drawable->bpp);

                gimp_preview_area_draw(GIMP_PREVIEW_AREA(preview), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE,
                                       GIMP_RGB_IMAGE, dst, rowbytes);
                gtk_widget_queue_draw (preview);
                g_free (tmp);
                _active = 'w';
            } else if (drawable->bpp == 4) {
                GimpPixelRgn amap_rgn;
                gint rowbytes = PREVIEW_SIZE * 4;
                gint nbytes = drawable->width * drawable->height * 4;
                guchar *tmp = g_new(guchar, nbytes);
                guchar dst[PREVIEW_RGBA_SIZE];

                gimp_pixel_rgn_init(&amap_rgn, drawable, 0, 0, drawable->width, drawable->height, 0, 0);
                gimp_pixel_rgn_get_rect(&amap_rgn, tmp, 0, 0, drawable->width, drawable->height);

                scale_pixels(dst, PREVIEW_SIZE, PREVIEW_SIZE, tmp, drawable->width, drawable->height, drawable->bpp);

                gimp_preview_area_draw(GIMP_PREVIEW_AREA(preview), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE,
                                     GIMP_RGBA_IMAGE, dst, rowbytes);
                
                gtk_widget_queue_draw (preview);
                g_free (tmp);
                _active = 'w';
            }
        }
    }
}

static void preview_diffuse_only(gint32 image_ID) {
    gint32 noiselayer_ID = -1;
    gint32 drawable_ID = -1;
    gint32 drawableDiffuse_ID = -1;
    
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.0);
    gtk_label_set_text(GTK_LABEL(progress_label), "Begin");

    drawableBeginActiveLayer = gimp_image_get_active_layer(image_ID);
    drawable_ID = gimp_layer_copy (drawableBeginActiveLayer);
    gimp_image_add_layer(image_ID, drawable_ID, -1);
    gimp_image_set_active_layer(image_ID, drawable_ID);
    /** Here I should hide previous active layer, make not visible. */
    gimp_drawable_set_visible(drawableBeginActiveLayer, FALSE);

    /**
     * For preview do nothing here. 
     *   if(local_vals.Resizie)
     *   {
     *   }
     */
    
    if (local_vals.Noise)
    {
        /** Already have active layer in drawable_ID. */
        noiselayer_ID = gimp_layer_copy (drawable_ID);

        gimp_image_add_layer(image_ID, noiselayer_ID, -1);
        gimp_image_set_active_layer(image_ID, noiselayer_ID);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.03);
        gtk_label_set_text(GTK_LABEL(progress_label), "Noise");

        /**
         * Filter "RGB Noise" applied
         * Standard plug-in. Source code ships with GIMP.
         * 
         * Add the "f" here to signify float in c language.
         */
        if (plug_in_rgb_noise_connector(image_ID, noiselayer_ID, 1, 1, 0.20f, 0.20f, 0.20f, 0.0f) != 1) return;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.04);
        
        gimp_layer_set_mode(noiselayer_ID, GIMP_VALUE_MODE);
        gimp_image_merge_down(image_ID, noiselayer_ID, 0);
        
        gtk_label_set_text(GTK_LABEL(gwNormalLabel), "Noise affects every redraw!");
    } else {
        gtk_label_set_text(GTK_LABEL(gwNormalLabel), "");
        gtk_label_set_text(GTK_LABEL(progress_label), "Noise added");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.04);
    }
         
    if(local_vals.RemoveLighting)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Remove Shading");
        removeShadingPreview(image_ID, local_vals.Noise);
        /**
         * See notes inside of removeShadingPreview
         * for explanation of next line.
         */
        
        /**
         * If Noise is on, then noiselayer_ID was merged down into drawable_ID.
         * You cannot remove drawable_ID in this case, as it is the only 
         * layer left!
         * 
         * However, if Noise is Yes and RemoveLighting is Yes,
         * Then there is an extra layer floating around!
         * Delete noiselayer_ID?  I thought it was merged!
         * No, I was right noiselayer_ID is already gone!
         */
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.05);
    } else {
        gtk_label_set_text(GTK_LABEL(progress_label), "Stretch");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.05);
    }
		
    drawableDiffuse_ID = gimp_image_get_active_layer(image_ID);
    gimp_levels_stretch(drawableDiffuse_ID);
    if(local_vals.Tile)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Making Seamless");
        /**
         * Filter "Tile Seamless" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        if (plug_in_make_seamless_connector(image_ID, drawableDiffuse_ID) != 1) return;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.07);
    } else {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.07);
    }
    
    /** Here I should un hide previously hidden layer, make visible. */
    
    pDrawables.drawable_d = gimp_drawable_get(drawableDiffuse_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.1);
}

static void preview_alt_normal(gint32 image_ID) {
/*******************************************************************************
 * Begin H and Normal
 ******************************************************************************/    
    gint32 mergedLayer_ID = -1;
    gint32 normalmap_ID = -1;
    gfloat wsize = (gfloat)gimp_image_width(image_ID);
    gfloat hsize = (gfloat)gimp_image_width(image_ID);
    /** Get active layer. */
    gint32 drawable_temp_ID = gimp_image_get_active_layer(image_ID);
    /** Copy active layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    gint32 diffuse_ID = gimp_layer_copy (drawable_temp_ID);
    /** Add new layer to image. */
    gimp_image_add_layer(image_ID, diffuse_ID, -1);
    /** Set new layer as active. */
    gimp_image_set_active_layer(image_ID, diffuse_ID);
    /** Here I should hide previous active layer, make not visible. */
    gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");

    /**
     * Since copied, don't need this here.
     * blur seems to not create an extra layer.
     */
    blur(image_ID, diffuse_ID, wsize, hsize, local_vals.LargeDetails, 0, local_vals.ao);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.15);
    
    normalmap_ID = gimp_image_get_active_layer(image_ID);
    if(local_vals.smoothstep)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
        /**
         * Filter "Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.20);
    }
    else
    {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.20);        
    }

    if(local_vals.invh)
    {
        /**
         * Colors Menu->Invert
         * Standard plug-in. Source code ships with GIMP.
         */
        if (plug_in_vinvert_connector(image_ID, normalmap_ID) != 1) return;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.25);
    }
    else
    {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.25);
    }
    
    /** Here is _p Displacement drawable. */
    pDrawables.drawable_p = gimp_drawable_get(normalmap_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.30);

    /** Extra layer here. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    gtk_label_set_text(GTK_LABEL(progress_label), "Base Mapping");
    doBaseMap(image_ID, diffuse_ID, local_vals.Depth, local_vals.LargeDetails, local_vals.ao);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.35);
    normalmap_ID = gimp_image_get_active_layer(image_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.40);
    
    /** Here is _ln low normal. l l l l l l l l l l l l l l l l */
    if (gcNeedNormal == 'l') {
        pDrawables.drawable_n = gimp_drawable_get(normalmap_ID);
    }

    /** Creates an extra layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    shapeRecognise(image_ID, normalmap_ID, local_vals.ShapeRecog, local_vals.ao);
    if(local_vals.smoothstep)
    {
        normalmap_ID = gimp_image_get_active_layer(image_ID);

        gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
        /**
         * Filter "Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.45);
    }
    else
    {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.45);
    }
    
    /** Here is _sn super normal. s s s s s s s s s s s s s s s s */
    if (gcNeedNormal == 's') {
        pDrawables.drawable_n = gimp_drawable_get(normalmap_ID);
    }
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.50);
    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen");

    /** Creates an extra layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    sharpen(image_ID, diffuse_ID, local_vals.Depth, 0, local_vals.SmallDetails);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.55);
    normalmap_ID = gimp_image_get_active_layer(image_ID);

    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen more");
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.65);
    
    /**
    * Filter Enhance "Sharpen" applied
    * Standard plug-in. Source code ships with GIMP.
    */
    if (plug_in_sharpen_connector(image_ID, normalmap_ID, 20) != 1) return;
    
    /** Here is _hn high normal. h h h h h h h h h h h h h h h h */
    if (gcNeedNormal == 'h') {
        pDrawables.drawable_n = gimp_drawable_get(normalmap_ID);
    }

    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen again");
    /** Creates an extra layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    sharpen(image_ID, diffuse_ID, local_vals.Depth, 6, local_vals.MediumDetails);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.70);
    normalmap_ID = gimp_image_get_active_layer(image_ID);
    
    gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
    /**
     * Filter "Blur" applied
     * Standard plug-in. Source code ships with GIMP.
     */
    if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.75);
    
    /** Here is _mn medium normal m m m m m m m m m m m m m m m m */
    if (gcNeedNormal == 'm') {
        pDrawables.drawable_n = gimp_drawable_get(normalmap_ID);
    }
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.78);

    gimp_drawable_set_visible(diffuse_ID, 0);
    
    /**
     * Don't do the next line:
     * 
     * gimp_image_merge_visible_layers(image_ID, 0);
     * 
     * Do this instead for preview:
     */
    mergedLayer_ID = gimp_layer_new_from_visible(image_ID, image_ID, "temp");
    /** Add copied layer to image. */
    gimp_image_add_layer(image_ID, mergedLayer_ID, -1);
}

static void preview_normal_only(gint32 image_ID) {
    gint32 drawableNormal_ID = -1;
    /**
     * Do not do this for preview.
     * gint32 diffuse_ID = gimp_image_get_active_layer(image_ID);
     * gimp_drawable_set_visible(diffuse_ID, 0);
     * gimp_image_merge_visible_layers(image_ID, 0);
     * 
     * The theory here is to leave the original layer untouched, 
     * copy the active original layer, make the copy active, 
     * then modify the active layer.  Show this layer to the 
     * preview slot, then delete the active layer, Leaving
     * the original drawing untouched for previews.
     */

    preview_diffuse_only(image_ID);
    preview_alt_normal(image_ID);
    
    /** Just grab. */
    drawableNormal_ID = gimp_image_get_active_layer(image_ID);

    /**
     * We just copied the active layer and made it active, don't need to do it again.
     * 
     * drawableNormalPrev_ID = gimp_image_get_active_layer(image_ID);
     */

    /**
     * Filter "Normalmap" applied
     * Non-Standard plug-in. Source code included.
     * 
     * original values:
     * plug_in_normalmap_derby(image, drawable, 0, 0.0, 1.0, 0, 0, 0, 8, 0, 0, 0, 0, 0.0, drawable)
     */
    nmapvals.filter = 0;
    nmapvals.minz = 0.0f;
    nmapvals.scale = 1.0f;
    nmapvals.wrap = 0;
    nmapvals.height_source = 0;
    nmapvals.alpha = 0;
    nmapvals.conversion = 8;
    nmapvals.dudv = 0;
    nmapvals.xinvert = 0;
    nmapvals.yinvert = 0;
    nmapvals.swapRGB = 0;
    nmapvals.contrast = 0.0f;
    nmapvals.alphamap_id = drawableNormal_ID;
    gtk_label_set_text(GTK_LABEL(progress_label), "Normal map");
    normalmap(drawableNormal_ID, 0);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.80);

    if (gcNeedNormal == 'n') {
        pDrawables.drawable_n = gimp_drawable_get(drawableNormal_ID);
    }
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.85);
}

static void preview_ambient_occlusion_only(gint32 image_ID) {
    gint32 drawable_ID = -1;
    gint32 drawableAO_ID = -1;
    
    gtk_label_set_text(GTK_LABEL(progress_label), "Mixing Colors");
    // preview_diffuse_only(image_ID);
    preview_normal_only(image_ID);
    // if (preview_normal_only(image_ID) != NULL) return NULL;
    drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy active layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    drawableAO_ID = gimp_layer_copy (drawable_ID);
    /** Add new layer to image. */
    gimp_image_add_layer(image_ID, drawableAO_ID, -1);
    /** Set new layer as active. */
    gimp_image_set_active_layer(image_ID, drawableAO_ID);

    /**
     * Colors ->  Components -> "Channel Mixer" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     * Removed 0.0 on first param and changed to 0 because boolean.
     */
    
    /** Explained on line ~1300 of InsaneBump.c */
    // if (plug_in_colors_channel_mixer_connector(image_ID, drawableAO_ID, 0, -200.0f, 0.0f, 0.0f, 0.0f, -200.0f, 0.0f, 0.0f, 0.0f, 1.0f) != 1) return;
    if (plug_in_colors_channel_mixer_connector(image_ID, drawableAO_ID, 0, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f) != 1) return;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.87);
    
    gimp_desaturate(drawableAO_ID);
    gimp_levels_stretch(drawableAO_ID);
    
    pDrawables.drawable_ao = gimp_drawable_get(drawableAO_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.90);
}

static void preview_specular_only(gint32 image_ID) {
    gint32 drawableSpecular_ID = -1;
    gint32 nResult = 0;
    
    preview_ambient_occlusion_only(image_ID);

    if(local_vals.EdgeSpecular)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Specular Edging");
        drawableSpecular_ID = specularEdgeWorker(image_ID, local_vals.defSpecular, local_vals.ao, FALSE);
        if (drawableSpecular_ID == -1) {
            gimp_message("Specular Edge Worker returned -1!");
            nResult = 0;
        } else nResult = 1;
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Specular Smoothing");
        drawableSpecular_ID = specularSmoothWorker(image_ID, local_vals.defSpecular, local_vals.ao, FALSE);
        if (drawableSpecular_ID == -1) {
            gimp_message("Specular Smooth Worker returned -1!");
            nResult = 0;
        } else nResult = 1;
    }
    
    gtk_label_set_text(GTK_LABEL(progress_label), "Idle...");
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 1.0);

    if (nResult == 1) {
        pDrawables.drawable_s = gimp_drawable_get(drawableSpecular_ID);
    } else {
        pDrawables.drawable_s = NULL;
    }
}

/*  Public functions  */
/** Adding the preview area for the second release. */
void init_drawables(void)
{
    pDrawables.drawable_d = NULL;
    pDrawables.drawable_ao = NULL;
    pDrawables.drawable_s = NULL;
    pDrawables.drawable_n = NULL;
    pDrawables.drawable_p = NULL;
    pDrawables.preview_d = NULL;
    pDrawables.preview_ao = NULL;
    pDrawables.preview_s = NULL;
    pDrawables.preview_n = NULL;
    pDrawables.preview_p = NULL;

    btn_n = NULL;
    btn_ln = NULL;
    btn_mn = NULL;
    btn_hn = NULL;
    btn_sn = NULL;
    drawableBeginActiveLayer = -1;
    gcNeedNormal = 'n';
    
    _active = 'x' ;
}

void preview_redraw(void)
{
    if (!dialog_is_init) return;
    if (is_3D_preview_active()) return;
    if (local_vals.prev == 1)
    {
        // gimp_progress_init("Creating InsaneBump...");
        _active = 't'; // for start
        if (pDrawables.drawable_d != NULL) {
            gimp_drawable_detach(pDrawables.drawable_d);
            pDrawables.drawable_d = NULL;
        }
        if (pDrawables.drawable_ao != NULL) {
            gimp_drawable_detach(pDrawables.drawable_ao);
            pDrawables.drawable_ao = NULL;
        }
        if (pDrawables.drawable_s != NULL) {
            gimp_drawable_detach(pDrawables.drawable_s);
            pDrawables.drawable_s = NULL;
        }
        if (pDrawables.drawable_n != NULL) {
            gimp_drawable_detach(pDrawables.drawable_n);
            pDrawables.drawable_n = NULL;
        }
        if (pDrawables.drawable_p != NULL) {
            gimp_drawable_detach(pDrawables.drawable_p);
            pDrawables.drawable_p = NULL;
        }
        
        preview_specular_only(local_vals.image_ID);

        if (pDrawables.drawable_d != NULL) {
            _active = 'd' ;
            draw_preview_area_update(pDrawables.preview_d, pDrawables.drawable_d);
        } else {
            gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_d), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
            _active = 'd' ;
        }
        if (pDrawables.drawable_ao != NULL) {
            _active = 'a' ;
            draw_preview_area_update(pDrawables.preview_ao, pDrawables.drawable_ao);
        } else {
            gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_ao), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
            _active = 'a' ;
        }
        if (pDrawables.drawable_s != NULL) {
            _active = 's' ;
            draw_preview_area_update(pDrawables.preview_s, pDrawables.drawable_s);
        } else {
            gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_s), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
            _active = 's' ;
        }
        if (pDrawables.drawable_p != NULL) {
            _active = 'p' ;
            draw_preview_area_update(pDrawables.preview_p, pDrawables.drawable_p);
        } else {
            gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_p), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
            _active = 'p' ;
        }
        if (pDrawables.drawable_n != NULL) {
            _active = 'n' ;
            draw_preview_area_update(pDrawables.preview_n, pDrawables.drawable_n);
        } else {
            gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_n), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
        }

        removeAllLayersExceptMain();
        preview_progress_reset = 0 ;
    }
}

void preview_clicked(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0)
    {
        local_vals.prev = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Preview On");
        preview_redraw();
    }
    else
    {
        local_vals.prev = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "Preview Off");

        // html color: #f2f1f0
        gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_d), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
        gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_ao), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
        gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_s), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
        gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_p), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);
        gimp_preview_area_fill(GIMP_PREVIEW_AREA(pDrawables.preview_n), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0xF2, 0xF1, 0xF0);

        _active = 'x';
        return;
    }
}

/**
 * Set all the usable values to the startup default
 * values.  Take care here to preserve the image ID.  The copyPlugInVals
 * function strips out the image ID.
 * 
 * @param widget - not used but required by gtk signal
 * @param data   - not used but required by gtk signal
 */
void restore_defaults_clicked(GtkWidget *widget, gpointer data)
{
    PlugInVals pivTemp;
    if (!dialog_is_init) return;

    copyPlugInVals(&local_vals, &pivTemp);
    copyPlugInVals(&default_vals, &local_vals);
    /** Restore Button Toggles. */
    if (pivTemp.RemoveLighting != local_vals.RemoveLighting) {
        /** Toggle the Remove Lighting button. */
        gtk_button_clicked(GTK_BUTTON(g_gwRemoveLtgBtn));
        remlightbtn_clicked(g_gwRemoveLtgBtn, NULL);
    }
    if (pivTemp.Resizie != local_vals.Resizie) {
        /** Toggle the Upscale(HD) button. */
        gtk_button_clicked(GTK_BUTTON(g_gwUpscaleBtn));
        upscale_HD_clicked(g_gwUpscaleBtn, NULL);
    }
    if (pivTemp.Tile != local_vals.Tile) {
        /** Toggle the Tile button. */
        gtk_button_clicked(GTK_BUTTON(g_gwTileBtn));
        tile_clicked(g_gwTileBtn, NULL);
    }
    if (pivTemp.EdgeSpecular != local_vals.EdgeSpecular) {
        /** Toggle the Edge Specular Map button. */
        gtk_button_clicked(GTK_BUTTON(g_gwEdgeBtn));
        edge_enhancing_specular_clicked(g_gwEdgeBtn, NULL);
    }
    if (pivTemp.smoothstep != local_vals.smoothstep) {
        /** Toggle the Smooth Step button. */
        gtk_button_clicked(GTK_BUTTON(g_gwSmoothBtn));
        smooth_step_clicked(g_gwSmoothBtn, NULL);
    }
    if (pivTemp.Noise != local_vals.Noise) {
        /** Toggle the Noise button. */
        gtk_button_clicked(GTK_BUTTON(g_gwNoiseBtn));
        noise_clicked(g_gwNoiseBtn, NULL);
    }
    if (pivTemp.invh != local_vals.invh) {
        /** Toggle the Invert Height Map button. */
        gtk_button_clicked(GTK_BUTTON(g_gwInvertBtn));
        invert_height_map_clicked(g_gwInvertBtn, NULL);
    }
    /** Restore spinner edit controls. */
    if (pivTemp.Depth != local_vals.Depth) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwDepthSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.Depth);
    }
    if (pivTemp.LargeDetails != local_vals.LargeDetails) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwLargeDSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.LargeDetails);
    }
    if (pivTemp.MediumDetails != local_vals.MediumDetails) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwMediumDSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.MediumDetails);
    }
    if (pivTemp.SmallDetails != local_vals.SmallDetails) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwSmallDSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.SmallDetails);
    }
    if (pivTemp.ShapeRecog != local_vals.ShapeRecog) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwShapeSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.ShapeRecog);
    }
    if (pivTemp.ao != local_vals.ao) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwAOSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.ao);
    }
    if (pivTemp.defSpecular != local_vals.defSpecular) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwSpecDefSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.defSpecular);
    }
    if (pivTemp.newWidth != local_vals.newWidth) {
        GtkAdjustment * gaTemp = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(g_gwWidthSpin));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(gaTemp), (gdouble)local_vals.newWidth);
    }
    copyPlugInVals(&default_vals, &local_vals);
    local_vals.image_ID = pivTemp.image_ID;
    local_vals.prev = pivTemp.prev;
    
    if (local_vals.prev == 1)
    {
        preview_redraw();
    }
}

/** Adding the preview area for the second release. */
void preview_clicked_normal(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0) return;
    int nChecked = 0 ;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) nChecked = 1;
    
    if ((local_vals.prev == 1) && (nChecked == 1))
    {
        GtkWidget *gLabelWidge = gtk_frame_get_label_widget(GTK_FRAME(g_gwNormalsFrame));
        gtk_label_set_text(GTK_LABEL(gLabelWidge), "Normal");
        gcNeedNormal = 'n';
        preview_redraw();
    }
    else if ((local_vals.prev == 1) && (nChecked == 0))
    {
        return;
    }
}

void preview_clicked_lownormal(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0) return;
    int nChecked = 0 ;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) nChecked = 1;
    
    if ((local_vals.prev == 1) && (nChecked == 1))
    {
        GtkWidget *gLabelWidge = gtk_frame_get_label_widget(GTK_FRAME(g_gwNormalsFrame));
        gtk_label_set_text(GTK_LABEL(gLabelWidge), "Low Normal");
        gcNeedNormal = 'l';
        preview_redraw();
    }
    else if ((local_vals.prev == 1) && (nChecked == 0))
    {
        return;
    }
}

void preview_clicked_mediumnormal(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0) return;
    int nChecked = 0 ;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) nChecked = 1;
    
    if ((local_vals.prev == 1) && (nChecked == 1))
    {
        GtkWidget *gLabelWidge = gtk_frame_get_label_widget(GTK_FRAME(g_gwNormalsFrame));
        gtk_label_set_text(GTK_LABEL(gLabelWidge), "Medium Normal");
        gcNeedNormal = 'm';
        preview_redraw();
    }
    else if ((local_vals.prev == 1) && (nChecked == 0))
    {
        return;
    }
}

void preview_clicked_highnormal(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0) return;
    int nChecked = 0 ;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) nChecked = 1;
    
    if ((local_vals.prev == 1) && (nChecked == 1))
    {
        GtkWidget *gLabelWidge = gtk_frame_get_label_widget(GTK_FRAME(g_gwNormalsFrame));
        gtk_label_set_text(GTK_LABEL(gLabelWidge), "High Normal");
        gcNeedNormal = 'h';
        preview_redraw();
    }
    else if ((local_vals.prev == 1) && (nChecked == 0))
    {
        return;
    }
}

void preview_clicked_supernormal(GtkWidget *widget, gpointer data)
{
    if (!dialog_is_init) return;
    if (local_vals.prev == 0) return;
    int nChecked = 0 ;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) nChecked = 1;
    
    if ((local_vals.prev == 1) && (nChecked == 1))
    {
        GtkWidget *gLabelWidge = gtk_frame_get_label_widget(GTK_FRAME(g_gwNormalsFrame));
        gtk_label_set_text(GTK_LABEL(gLabelWidge), "Super Normal");
        gcNeedNormal = 's';
        preview_redraw();
    }
    else if ((local_vals.prev == 1) && (nChecked == 0))
    {
        return;
    }
}

void removeAllLayersExceptMain(void) {
    gint *pnLayers = NULL;
    gint numLayers = 0 ;
    gint nIndex = 0 ;
    gimp_image_set_active_layer(local_vals.image_ID, drawableBeginActiveLayer);
    pnLayers = gimp_image_get_layers(local_vals.image_ID, &numLayers);
    
    for (nIndex=0;nIndex< numLayers;nIndex++) {
        if (pnLayers[nIndex] != drawableBeginActiveLayer) {
            if (gimp_layer_is_floating_sel(pnLayers[nIndex]))
            {
                gimp_floating_sel_remove(pnLayers[nIndex]);
           } else {
                gimp_image_remove_layer(local_vals.image_ID, pnLayers[nIndex]);
            }
        }
    }
    
    gimp_drawable_set_visible(drawableBeginActiveLayer, TRUE);
}


int is_3D_preview_active(void)
{
    if (_active == 'w') {
        return FALSE;
    }
    else if (_active != 'x') {
        return TRUE;
    }
    return FALSE;
}

static GtkWidget *CreateOnePreviewFrame(GtkWidget *vbox, GtkWidget **preview, const gchar *szName)
{
    GtkWidget *frame = NULL;
    GtkWidget *labelFrame = NULL;
    GtkWidget *abox = NULL;
    
    if (vbox == NULL) return NULL;

    labelFrame = gtk_frame_new(szName);
    gtk_box_pack_start(GTK_BOX(vbox), labelFrame, 0, 0, 0);
    gtk_widget_show(labelFrame);

    abox = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
    gtk_container_set_border_width(GTK_CONTAINER (abox), 4);
    gtk_container_add(GTK_CONTAINER(labelFrame), abox);
    gtk_widget_show(abox);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(abox), frame);
    gtk_widget_show(frame);

    *preview = gimp_preview_area_new();
    gimp_preview_area_set_max_size(GIMP_PREVIEW_AREA(*preview), PREVIEW_SIZE, PREVIEW_SIZE);
    gtk_drawing_area_size(GTK_DRAWING_AREA(*preview), PREVIEW_SIZE, PREVIEW_SIZE);
    gtk_container_add(GTK_CONTAINER(frame), *preview);
    gtk_widget_show(*preview);
    
    return labelFrame;
}

void CreateLeftPreviewFrames(GtkWidget *hbox)
{
    GtkWidget *vbox = NULL;
    GtkWidget *btn = NULL;

    if (hbox == NULL) return;

    vbox = gtk_vbox_new(0, 8);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, 1, 1, 0);
    gtk_widget_show(vbox);
    
    CreateOnePreviewFrame(vbox, &pDrawables.preview_d, "Diffuse");
    CreateOnePreviewFrame(vbox, &pDrawables.preview_ao, "Occlusion");
    CreateOnePreviewFrame(vbox, &pDrawables.preview_s, "Specular");
    
    btn = gtk_button_new_with_label("Restore Defaults");
    if (btn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn), "clicked",
                           GTK_SIGNAL_FUNC(restore_defaults_clicked), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn, 0, 0, 0);
        gtk_widget_show(btn);
    }
}

void CreateRightPreviewToggleButton(GtkWidget *hbox)
{
    GtkWidget *vbox = NULL;
    GtkWidget *btn = NULL;
    
    if (hbox == NULL) return;

    vbox = gtk_vbox_new(0, 8);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, 1, 1, 0);
    gtk_widget_show(vbox);

    CreateOnePreviewFrame(vbox, &pDrawables.preview_p, "Displacement");
    g_gwNormalsFrame = CreateOnePreviewFrame(vbox, &pDrawables.preview_n, "Normal");

    if (local_vals.prev == 1) {
        btn = gtk_toggle_button_new_with_label("Preview On");
    } else {
        btn = gtk_toggle_button_new_with_label("Preview Off");
    }
    if (btn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn, 0, 0, 0);
        gtk_widget_show(btn);
        if (local_vals.prev == 1)
        {
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn), TRUE);
        }
    }
    
    btn_n = gtk_radio_button_new_with_label_from_widget(NULL, "Normal");
    if (btn_n != NULL)
    {
        // g_object_set_data(G_OBJECT(btn_n), "drawable", drawable);
    
        gtk_signal_connect(GTK_OBJECT(btn_n), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked_normal), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn_n, 0, 0, 0);
        gtk_widget_show(btn_n);
        // if ((local_vals.prev == 1) && (_normal_button == 1))
        // {
        //    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn_n), TRUE);
        // }
    }

    btn_ln = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(btn_n), "low normal");
    if (btn_ln != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn_ln), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked_lownormal), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn_ln, 0, 0, 0);
        gtk_widget_show(btn_ln);
    }

    btn_mn = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(btn_n), "medium normal");
    if (btn_mn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn_mn), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked_mediumnormal), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn_mn, 0, 0, 0);
        gtk_widget_show(btn_mn);
    }

    btn_hn = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(btn_n), "high normal");
    if (btn_hn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn_hn), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked_highnormal), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn_hn, 0, 0, 0);
        gtk_widget_show(btn_hn);
    }

    btn_sn = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(btn_n), "super normal");
    if (btn_sn != NULL)
    {
        gtk_signal_connect(GTK_OBJECT(btn_sn), "clicked",
                           GTK_SIGNAL_FUNC(preview_clicked_supernormal), 0);

        gtk_box_pack_start(GTK_BOX(vbox), btn_sn, 0, 0, 0);
        gtk_widget_show(btn_sn);
    }
    
    gwNormalLabel = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), gwNormalLabel, 0, 0, 0);
    gtk_widget_show(gwNormalLabel);
}

