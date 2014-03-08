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

#include <glib.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "InsaneBump.h"
#include "PluginConnectors.h"

/** Preview 5 windows in dialog box is enabled in this fourth release. */

/**
 * Builds a file name from the suffix.
 * 
 * Uses memory math.
 * 
 * Pointer szStr points to the memory location at the beginning
 * of the given filename.
 * 
 * Pointer szPtr points to the memory location at the decimal point.
 * 
 * The decimal point will always, in this case, be at a memory
 * location farther down than the start which means the decimal point
 * is located at a memory location value higher than the start.
 * 
 * So, for fence.bmp at 0x003000 the decimal point would be at
 * 0x003005 there for subtract 0x003000 from 0x003005 and 
 * you will get location 5 in the string
 * 
 * Then if you insert a "_n" as a suffix you will get filename:
 * fence_n.bmp!
 */
GString *getFilename(const gchar *filename, const gchar *suffix)
{
    GString * newfilename = g_string_new(filename);
    gchar *szStr = newfilename->str;
    char *szPtr = strchr((char *)szStr, '.');
    int nLocate = szPtr - szStr;
    g_string_insert(newfilename, nLocate, suffix);
    return newfilename;
}

void saveLastOperation(gint32 image_ID, const GString *filename)
{
    gint32 layer_ID = gimp_image_get_active_layer(image_ID);
    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, layer_ID, filename->str, filename->str);
}

void set_progress_label_as_file(GString *file_name_given, const gchar *szSuffix)
{
    GString *string_temp = g_string_new(file_name_given->str);
    gchar *szStr = string_temp->str;
    char *szPtr = strrchr((char *)szStr, '\\');
    if (szPtr == NULL)
    {
        /** Probably Not Windows. Linux? */
        szPtr = strrchr((char *)szStr, '/');
    }
    if (szPtr != NULL)
    {
        szPtr++ ;
        GString *string_sub = g_string_new(szPtr);
        
        g_string_printf(string_temp, "Saving %s %s", szSuffix, string_sub->str);
        gtk_label_set_text(GTK_LABEL(progress_label), string_temp->str);
        g_string_free(string_temp, TRUE);
    }
    else
    {
        g_string_printf(string_temp, "Saving %s %s", szSuffix, file_name_given->str);
        gtk_label_set_text(GTK_LABEL(progress_label), string_temp->str);
        g_string_free(string_temp, TRUE);
    }
}

gint32 specularEdgeWorker(gint32 image_ID, gint defin, gint defAO, gint32 bShowProgress)
{
    gfloat secondRadius = defAO * 0.16f;
    /** Get the active layer. */
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy the active layer. */
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.92);
    }
    
    /** Add the copied layer to the image file. */
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    /** Set the copied layer to active. */
    gimp_image_set_active_layer(image_ID, newlayer_ID);

    /**
     * Desaturate the active layer.
     *
     * This procedure desaturates the contents of the active layer.
     * This procedure only works on drawables of type RGB color.
     * 
     * By using the Desaturate command, you can convert all of the colors on the active layer
     * to corresponding shades of gray. This differs from converting the image to grayscale in
     * two respects. First, it only operates on the active layer and second, the colors on the
     * layer are still RGB values with three components. This means that you can paint on the
     * layer, or individual parts of it, using color at a later time.
     */
    gimp_desaturate(newlayer_ID);

    /** Get the active layer again. */
    drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy the active layer again. */
    newlayer_ID = gimp_layer_copy (drawable_ID);
    /** Add the copied layer to the image file again. */
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    /** Set the copied layer to active again. */
    gimp_image_set_active_layer(image_ID, newlayer_ID);

    /** Desaturate the active layer again. (see above) */
    gimp_desaturate(newlayer_ID);
    
    /**
     * Filter "Difference of Gaussians" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Dog the active layer.
     * 
     * Add the "f" here to signify float in c language.
     * Radius Parameters 2 and 3 are in pixels.
     * 
     * Apply the AO value here.  50 will give 1.0 and 8.0
     * 30 will give 1.0 and 5.0,  90 will give 1.0 and 12.0
     * 
     * So multiply the AO times 0.16 and place inside the second radius
     * 
     * Could also play with contrast/brightness in the future.
     * 
     * These adjustments also apply to specularSmoothWorker too.
     * 
     */
    // if (plug_in_dog_connector(image_ID, newlayer_ID, 1.0, 8.0, 1, 0) != 1) return 0;
    if (plug_in_dog_connector(image_ID, newlayer_ID, 1.0, secondRadius, 1, 0) != 1) return 0;
    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.94);
    }
    
    /**
     * Originally removed by Omar Emad
     * 
     * plug_in_vinvert(image_ID,newlayer_ID);
     */    

    /** 7 should be replaced with the correct enum from GimpLayerModeEffects. */
    // gimp_layer_set_mode(newlayer_ID, 7);
    /** Set addition mode to layer. */
    gimp_layer_set_mode(newlayer_ID, GIMP_ADDITION_MODE);
    /** Merge layer down. Changed 0 to GIMP_EXPAND_AS_NECESSARY enum. */
    gimp_image_merge_down(image_ID, newlayer_ID, GIMP_EXPAND_AS_NECESSARY);
    /** Get the active layer again. */
    drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Adjust the layer brightness and contrast. */
    gimp_brightness_contrast(drawable_ID, 0, 64);
    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.96);
    }

    /** Modify intensity levels of active layer. */
    gimp_levels(drawable_ID, 0, defin, 255, 1, 0, 255);
    
    return drawable_ID;
}

gint32 specularSave(GString *sFileName, gint32 image_ID, gint32 drawable_ID)
{
    if (sFileName != NULL) {
        set_progress_label_as_file(sFileName, "_s");
        gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, drawable_ID, sFileName->str, sFileName->str);
    }
    
    /**
     * 
     * Clear selected area of drawable.
     *
     * This procedure clears the specified drawable. If the drawable has an alpha channel,
     * the cleared pixels will become transparent. If the drawable does not have an alpha channel,
     * cleared pixels will be set to the background color. This procedure only affects regions
     * within a selection if there is a selection active.
     */
    gimp_edit_clear(drawable_ID);
    
    return 1;
}

gint32 specularEdgeOriginal(gint32 image_ID, const gchar *file_name, gint defin)
{
    GString *file_name_temp = NULL;
    if (file_name != NULL) {
        file_name_temp = getFilename(file_name, "_s");
    }
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    if (file_name != NULL) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.92);
    }
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    gimp_image_set_active_layer(image_ID, newlayer_ID);
    gimp_desaturate(newlayer_ID);

    drawable_ID = gimp_image_get_active_layer(image_ID);
    newlayer_ID = gimp_layer_copy (drawable_ID);
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    gimp_image_set_active_layer(image_ID, newlayer_ID);
    gimp_desaturate(newlayer_ID);
    
    /**
     * Filter "Difference of Gaussians" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     */
    if (plug_in_dog_connector(image_ID, newlayer_ID, 1.0, 8.0, 1, 0) != 1) return 0;
    if (file_name != NULL) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.94);
    }
    
    /**
     * Originally removed by Omar Emad
     * 
     * plug_in_vinvert(image_ID,newlayer_ID);
     */    
    
    gimp_layer_set_mode(newlayer_ID, 7);
    gimp_image_merge_down(image_ID, newlayer_ID, 0);
    drawable_ID = gimp_image_get_active_layer(image_ID);
    gimp_brightness_contrast(drawable_ID, 0, 64);
    if (file_name != NULL) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.96);
    }

    gimp_levels(drawable_ID, 0, defin, 255, 1, 0, 255);
    set_progress_label_as_file(file_name_temp, "_s");
    if (file_name != NULL) {
        gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, drawable_ID, file_name_temp->str, file_name_temp->str);
        g_string_free(file_name_temp, TRUE);
    }
    gimp_edit_clear(drawable_ID);
    
    return 1;
}	

gint32 specularSmoothWorker(gint32 image_ID, gint defin, gint defAO, gint32 bShowProgress)
{
    gfloat firstRadius = defAO * 0.16;
    /** Get active layer. */
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy active layer. */
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    /** Add the copied layer to the image file. */
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    /** Set the copied layer to active. */
    gimp_image_set_active_layer(image_ID, newlayer_ID);

    /**
     * Desaturate the active layer.
     *
     * This procedure desaturates the contents of the active layer.
     * This procedure only works on drawables of type RGB color.
     * 
     * By using the Desaturate command, you can convert all of the colors on the active layer
     * to corresponding shades of gray. This differs from converting the image to grayscale in
     * two respects. First, it only operates on the active layer and second, the colors on the
     * layer are still RGB values with three components. This means that you can paint on the
     * layer, or individual parts of it, using color at a later time.
     */
    gimp_desaturate(newlayer_ID);
    
    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.92);
    }

    /** Get active layer again. */
    drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy active layer again. */
    newlayer_ID = gimp_layer_copy (drawable_ID);
    /** Add the copied layer to the image file again. */
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    /** Set the copied layer to active. */
    gimp_image_set_active_layer(image_ID, newlayer_ID);
    
    /** Desaturate copied layer again. (see above)*/
    gimp_desaturate(newlayer_ID);

    /**
     * Filter "Difference of Gaussians" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * DOG the active layer.
     * 
     * Add the "f" here to signify float in c language.
     */
    // if (plug_in_dog_connector(image_ID, newlayer_ID, 8.0f, 1.0f, 1, 0) != 1) return 0;
    if (plug_in_dog_connector(image_ID, newlayer_ID, firstRadius, 1.0f, 1, 0) != 1) return 0;
    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.94);
    }

    /**
     * Originally removed by Omar Emad
     * 
     * plug_in_vinvert(image_ID,newlayer);
     */    
	
    /** 7 should be replaced with the correct enum from GimpLayerModeEffects. */
    // gimp_layer_set_mode(newlayer_ID, 7);
    /** Set addition mode to layer. */
    gimp_layer_set_mode(newlayer_ID, GIMP_ADDITION_MODE);
    /** Merge layer down. Changed 0 to GIMP_EXPAND_AS_NECESSARY enum. */
    gimp_image_merge_down(image_ID, newlayer_ID, GIMP_EXPAND_AS_NECESSARY);

    /** Get active layer again. */
    drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Adjust the color brightness and contrast. */
    gimp_brightness_contrast(drawable_ID, 0, 64);
    /** Adjust the color levels. */
    gimp_levels(drawable_ID, 0, defin, 255, 1, 0, 255);

    if (bShowProgress == TRUE) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.96);
    }
    
    /**
     * Raise the specified layer in the image's layer stack to top of stack
     * 
     * This procedure raises the specified layer to top of the existing layer stack.
     * It will not move the layer if there is no layer above it.
     */
    gimp_image_raise_layer_to_top(image_ID, drawable_ID);
    
    return drawable_ID;
}

gint32 specularDo(gint32 image_ID, const gchar *file_name, gint defin, gint defAO, gint32 edgeSpecular)
{
    GString *file_name_temp = NULL;
    gint32 drawable_ID = -1;
    if (file_name != NULL) {
        file_name_temp = getFilename(file_name, "_s");
    }
    
    if (edgeSpecular) {
        drawable_ID = specularEdgeWorker(image_ID, defin, defAO, TRUE);
    } else {
        drawable_ID = specularSmoothWorker(image_ID, defin, defAO, TRUE);
    }
    
    if (drawable_ID != -1) {
        return specularSave(file_name_temp, image_ID, drawable_ID);
    } else {
        return 0;
    }
}

gint32 specularSmoothOriginal(gint32 image_ID, const gchar *file_name, gint defin)
{
    GString *file_name_temp = NULL;
    if (file_name != NULL) {
        file_name_temp = getFilename(file_name, "_s");
    }
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    gimp_image_set_active_layer(image_ID, newlayer_ID);
    gimp_desaturate(newlayer_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.92);

    drawable_ID = gimp_image_get_active_layer(image_ID);
    newlayer_ID = gimp_layer_copy (drawable_ID);
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    gimp_image_set_active_layer(image_ID, newlayer_ID);
    gimp_desaturate(newlayer_ID);

    /**
     * Filter "Difference of Gaussians" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     */
    if (plug_in_dog_connector(image_ID, newlayer_ID, 8.0f, 1.0f, 1, 0) != 1) return 0;
    if (file_name != NULL) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.94);
    }

    /**
     * Originally removed by Omar Emad
     * 
     * plug_in_vinvert(image_ID,newlayer);
     */    
	
    /** 7 should be replaced with the correct enum from GimpLayerModeEffects. */
    // gimp_layer_set_mode(newlayer_ID, 7);
    /** Set addition mode to layer. */
    gimp_layer_set_mode(newlayer_ID, GIMP_ADDITION_MODE);
    /** Merge layer down. Changed 0 to GIMP_EXPAND_AS_NECESSARY enum. */
    gimp_image_merge_down(image_ID, newlayer_ID, GIMP_EXPAND_AS_NECESSARY);

    drawable_ID = gimp_image_get_active_layer(image_ID);
    gimp_brightness_contrast(drawable_ID, 0, 64);
    gimp_levels(drawable_ID, 0, defin, 255, 1, 0, 255);
    if (file_name != NULL) {
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.96);
    }
    gimp_image_raise_layer_to_top(image_ID, drawable_ID);
    if (file_name != NULL) {
        set_progress_label_as_file(file_name_temp, "_s");
        gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, drawable_ID, file_name_temp->str, file_name_temp->str);
        g_string_free(file_name_temp, TRUE);
    }
    gimp_edit_clear(drawable_ID);
    
    return 1;
}

void removeGivenLayerFromImage(gint32 image_ID, gint32 layer_ID)
{
    int *pnLayers = NULL;
    int nNumberOfLayers = 0 ;
    int nIndex = 0 ;
    pnLayers = gimp_image_get_layers(image_ID, &nNumberOfLayers);
    if ((nNumberOfLayers > 1) && (pnLayers != NULL)) {
        for (nIndex=0;nIndex < nNumberOfLayers;nIndex++) {
            if (pnLayers[nIndex] != layer_ID) {
                gimp_image_set_active_layer(image_ID, pnLayers[nIndex]);
                break;
            }
        }

        // gimp_edit_clear(layer_ID);
        // gimp_image_merge_down(image_ID, layer_ID, GIMP_EXPAND_AS_NECESSARY);
        // gimp_message("Layer cleared and merged down.");
        if (gimp_layer_is_floating_sel(layer_ID))
        {
          gimp_floating_sel_remove(layer_ID);
        } else {
            if (gimp_image_remove_layer(image_ID, layer_ID) != TRUE) {
                gimp_message("Removing layer failed!");
            }
        }

        // gimp_image_flush(image_ID);

    } else {
        gimp_message("Not removing layer!");
    }
}

void removeShading(gint32 image_ID)
{
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    gimp_image_set_active_layer(image_ID, newlayer_ID);

    /**
     * Filter "Gaussian Blur" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     */
    if (plug_in_gauss_connector(image_ID, newlayer_ID, 20.0f, 20.0f, 0) != 1) return;

    /**
     * Colors Menu->Invert
     * Standard plug-in. Source code ships with GIMP.
     */
    if (plug_in_vinvert_connector(image_ID, newlayer_ID) != 1) return;

    /** 5 is GIMP_OVERLAY_MODE replaced. */
    gimp_layer_set_mode(newlayer_ID, GIMP_OVERLAY_MODE);

    /** This command is causing a problem in preview mode. */
    gimp_image_merge_visible_layers(image_ID, 0);
}

void removeShadingPreview(gint32 image_ID, gint32 noise_val)
{
    gint32 mergedLayer_ID = -1;
    // gint layerPos = 0 ;
    // gint mergeLayerPos = 0 ;
    /** Get active layer. */
    gint32 drawable_ID = gimp_image_get_active_layer(image_ID);
    /** Copy active layer. */
    gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
    /** Add copied layer to image. */
    gimp_image_add_layer(image_ID, newlayer_ID, -1);
    /** Make copied layer the active layer. */
    gimp_image_set_active_layer(image_ID, newlayer_ID);

    /**
     * Filter "Gaussian Blur" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Gauss the active new layer.
     * 
     * Add the "f" here to signify float in c language.
     */
    if (plug_in_gauss_connector(image_ID, newlayer_ID, 20.0f, 20.0f, 0) != 1) return;

    /**
     * Colors Menu->Invert
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Invert the active layer.
     */
    if (plug_in_vinvert_connector(image_ID, newlayer_ID) != 1) return;

    /** Place the active layer into overlay mode. */
    /** 5 is GIMP_OVERLAY_MODE replaced. */
    gimp_layer_set_mode(newlayer_ID, GIMP_OVERLAY_MODE);

    /** This command is causing a problem in preview mode. */
    // gimp_image_merge_visible_layers(image_ID, 0);
    
    /**
     * What I want to try here is instead of merge visible layers
     * which deletes all layers except one, I want to merge visible into
     * a new layer.  That will require new from layer, set new layer just 
     * under active layer (see above) and then merge active layer down.
     * So, here goes:
     */
    // layerPos = gimp_image_get_layer_position(image_ID, newlayer_ID);
    mergedLayer_ID = gimp_layer_new_from_visible(image_ID, image_ID, "temp_shadow");
    /** Add copied layer to image. */
    gimp_image_add_layer(image_ID, mergedLayer_ID, -1);
    
    /**
     * Nope, I think just merging visible to a new layer is all that is needed.
     * Because, Now all is merged.  And we have a new layer.  Actually deleting
     * the previous layer is probably necessary!(newlayer_ID)
     */
    
    /**
     * Ok, let's try that.  Because there were too many layers after this
     * preview.  In Diffuse Preview code after this removeShadingPreview
     * I deleted the previous layer and now original image is completely
     * returned to normal after this call and no extra layers!
     */
    // removeGivenLayerFromImage(image_ID, newlayer_ID);
    
    /**
     * Ok, that helped.  Now at least the original is returned.  So each iteration
     * of this command is not getting darker and darker, etc...  But there is
     * still one too many layers after this command.
     */
    
//    mergeLayerPos = gimp_image_get_layer_position(image_ID, mergedLayer_ID);
//    if (layerPos < mergeLayerPos) {
//        gint nIndex = layerPos ;
//        while (nIndex < mergeLayerPos) {
//            /** Raise the original modified layer. */
//            gimp_image_raise_layer(image_ID, newlayer_ID);
//            nIndex++;
//        }
//    } else if ((layerPos - 1) > mergeLayerPos) {
//        gint nIndex = mergeLayerPos ;
//        while (nIndex < (layerPos - 1)) {
//            /** Raise the new merged layer. */
//            gimp_image_raise_layer(image_ID, mergedLayer_ID);
//            nIndex++;
//        }
//    }
//    layerPos = gimp_image_get_layer_position(image_ID, newlayer_ID);
//    mergeLayerPos = gimp_image_get_layer_position(image_ID, mergedLayer_ID);
//    if ((layerPos - 1) == mergeLayerPos) {

    /** Make copied layer the active layer. */
    gimp_image_set_active_layer(image_ID, mergedLayer_ID);
    
    // if (noise_val) removeGivenLayerFromImage(image_ID, drawable_ID);
}

void blur(gint32 image_ID, gint32 diffuse_ID, gfloat width, gfloat height, gint32 passes, gint32 normal, gint defAO)
{
    gint32 drawable_ID = 0 ;
    gint32 i = 0 ;

    gfloat fMultiplier = defAO / 1000.0f;
    /** Copy current layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    gint32 desatdiffuse_ID = gimp_layer_copy (diffuse_ID);
    /** Add the new layer to the image. */
    gimp_image_add_layer(image_ID, desatdiffuse_ID, -1);
    /** Set the new layer as the active layer. */
    gimp_image_set_active_layer(image_ID, desatdiffuse_ID);
    if (normal == 0)
    {
        gimp_desaturate(desatdiffuse_ID);
    }
    
    /** This gets repeated Large Details times!!! A new layer each time. */
    for (i = 0; i < passes; i++)
    {
        /** Get the active layer. */
        drawable_ID = gimp_image_get_active_layer(image_ID);
        /** Copy the active layer. */
        /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
        /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
        /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
        gint32 newlayer_ID = gimp_layer_copy (drawable_ID);
        /** Add the new layer to the image. */
        gimp_image_add_layer(image_ID, newlayer_ID, -1);
        /** Set the new layer as the active layer. */
        gimp_image_set_active_layer(image_ID, newlayer_ID);
        
        /**
         * Filter "Gaussian Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         * 
         * Add the "f" here to signify float in c language.
         */
        if (plug_in_gauss_connector(image_ID, newlayer_ID, width * fMultiplier, height * fMultiplier, 0) != 1) return;

        /** 5 is GIMP_OVERLAY_MODE replaced. */
        gimp_layer_set_mode(newlayer_ID, GIMP_OVERLAY_MODE);
        /** 0 is GIMP_EXPAND_AS_NECESSARY replaced. */
        gimp_image_merge_down(image_ID, newlayer_ID, GIMP_EXPAND_AS_NECESSARY);
        /** So, the formula here is copy, overlay then merge.  Always results in 1 layer finally. */
        /** MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
        /** MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
        /** MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
    }
    
    drawable_ID = gimp_image_get_active_layer(image_ID);
    
    if (normal == 1)
    {
        gfloat fScalar = defAO / 50.0f;
        
        /**
         * Filter "Normalmap" applied
         * Non-Standard plug-in. Source code included.
         * 
         * original values:
                 * plug_in_normalmap_derby(image_ID, drawable_ID, 0, 0.0f, 1.0f, 0, 0, 0, 8, 0, 0, 0, 0, 0.0f, drawable_ID)
         */
        nmapvals.filter = 0;
        nmapvals.minz = 0.0f;
        // nmapvals.scale = 1.0f;
        nmapvals.scale = fScalar;
        nmapvals.wrap = 0;
        nmapvals.height_source = 0;
        nmapvals.alpha = 0;
        nmapvals.conversion = 8;
        nmapvals.dudv = 0;
        nmapvals.xinvert = 0;
        nmapvals.yinvert = 0;
        nmapvals.swapRGB = 0;
        nmapvals.contrast = 0.0f;
        nmapvals.alphamap_id = drawable_ID;
        normalmap(drawable_ID, 0);

        /** 5 is GIMP_OVERLAY_MODE replaced. */
        gimp_layer_set_mode(drawable_ID, GIMP_OVERLAY_MODE);
        /** 0 is GIMP_EXPAND_AS_NECESSARY replaced. */
        gimp_image_merge_down(image_ID, drawable_ID, GIMP_EXPAND_AS_NECESSARY);
        /** MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
    }
}				

void sharpen(gint32 image_ID, gint32 diffuse, gfloat depth, gint32 filterSize, gdouble strength)
{
    /** Copy given layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    gint32 sharpnormal_ID = gimp_layer_copy (diffuse);
    /** Add the copied layer to the image. */
    gimp_image_add_layer(image_ID, sharpnormal_ID, -1);
    /** Set the new layer as the active layer. */
    gimp_image_set_active_layer(image_ID, sharpnormal_ID);
	
    /**
     * Filter "Normalmap" applied
     * Non-Standard plug-in. Source code included.
     * 
     * original values:
     * plug_in_normalmap_derby(image_ID, sharpnormal_ID, 0, 0.0, depth, filterSize, 0, 0, 0, 0, 0, 1, 0, 0.0, sharpnormal_ID);
     * Looks like an error 12-5-2013 2:03pm  filterSize should be for filter not wrap!  wrap should just be 0
     * plug_in_normalmap_derby(image_ID, sharpnormal_ID, 0, 0.0, depth, filterSize, 0, 0, 0, 0, 0, 1, 0, 0.0, sharpnormal_ID);
     */
    // nmapvals.filter = 0;
    nmapvals.filter = filterSize; // 12-5-2013 2:03pm
    nmapvals.minz = 0.0f;
    nmapvals.scale = depth;
    // nmapvals.wrap = filterSize;
    nmapvals.wrap = 0; // 12-5-2013 2:03pm
    nmapvals.height_source = 0;
    nmapvals.alpha = 0;
    nmapvals.conversion = 0;
    nmapvals.dudv = 0;
    nmapvals.xinvert = 0;
    nmapvals.yinvert = 1;
    nmapvals.swapRGB = 0;
    nmapvals.contrast = 0.0f;
    nmapvals.alphamap_id = sharpnormal_ID;
    normalmap(sharpnormal_ID, 0);

    /**
     * Originally removed by Omar Emad
     * 
     * gimp_levels_stretch(sharpnormal_ID);
     */    

    gimp_image_set_active_layer(image_ID, sharpnormal_ID);
    /** 5 is GIMP_OVERLAY_MODE replaced. */
    gimp_layer_set_mode(sharpnormal_ID, GIMP_OVERLAY_MODE);

    /**
     * Originally removed by Omar Emad
     * 
     * gimp_file_save(image, sharpnormal_ID, getFilename(file_name, "_hn"), getFilename(file_name, "_hn"));
     */    

    gimp_layer_set_opacity(sharpnormal_ID, strength);
    gimp_image_raise_layer_to_top(image_ID, sharpnormal_ID);
}

void shapeRecognise(gint32 image_ID, gint32 normalmap_ID, gdouble strength, gint defAO)
{
    gfloat fGaussFactor = defAO * 0.40f;
    gfloat fScalar = defAO / 50.0f;
    /** Copy given layer. */
    /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
    gint32 blurnormal_ID = gimp_layer_copy(normalmap_ID);
    /** Add new layer to image. */
    gimp_image_add_layer(image_ID, blurnormal_ID, -1);
    /** Set new layer as the active layer. */
    gimp_image_set_active_layer(image_ID, blurnormal_ID);
    
    /**
     * Filter "Gaussian Blur" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the ".0f" here to signify float in c language.
     */
    // if (plug_in_gauss_connector(image_ID, blurnormal_ID, 20.0f, 20.0f, 0) != 1) return;
    if (plug_in_gauss_connector(image_ID, blurnormal_ID, fGaussFactor, fGaussFactor, 0) != 1) return;

    /**
     * Colors ->  Components -> "Channel Mixer" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     * Removed 0.0 on first param and changed to 0 because boolean.
     */

    /** Explained on line ~1300 */
    // if (plug_in_colors_channel_mixer_connector(image_ID, blurnormal_ID, 0, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -200.0f) != 1) return;
    if (plug_in_colors_channel_mixer_connector(image_ID, blurnormal_ID, 0, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f) != 1) return;

    /**
     * Filter "Normalmap" applied
     * Non-Standard plug-in. Source code included.
     * 
     * original values:
     * plug_in_normalmap_derby(image_ID, blurnormal_ID, 0, 0.0, 1.0, 0, 0, 0, 8, 0, 0, 0, 0, 0.0, blurnormal_ID);
     */
    nmapvals.filter = 0;
    nmapvals.minz = 0.0f;
    // nmapvals.scale = 1.0f;
    nmapvals.scale = fScalar;
    nmapvals.wrap = 0;
    nmapvals.height_source = 0;
    nmapvals.alpha = 0;
    nmapvals.conversion = 8;
    nmapvals.dudv = 0;
    nmapvals.xinvert = 0;
    nmapvals.yinvert = 0;
    nmapvals.swapRGB = 0;
    nmapvals.contrast = 0.0f;
    nmapvals.alphamap_id = blurnormal_ID;
    normalmap(blurnormal_ID, 0);

    /**
     * Colors ->  Components -> "Channel Mixer" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     * Removed 0.0 on first param and changed to 0 because boolean.
     */
    
    /** Explained on line ~1300 */
    // if (plug_in_colors_channel_mixer_connector(image_ID, blurnormal_ID, 0, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -200.0f) != 1) return;
    if (plug_in_colors_channel_mixer_connector(image_ID, blurnormal_ID, 0, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f) != 1) return;

    gimp_layer_set_mode(blurnormal_ID, 5);
    gimp_layer_set_opacity(blurnormal_ID, strength);
}
	
// not used anywhere ... commenting out (Derby Russell)
//void analyze(gint32 layer_ID)
//{
//	gboolean highlights = gimp_histogram(layer_ID, 0, 179, 256, NULL, NULL, NULL, NULL, NULL, NULL);
//	gboolean midtones = gimp_histogram(layer_ID, 0, 77, 178, NULL, NULL, NULL, NULL, NULL, NULL);
//	gboolean shadows = gimp_histogram(layer_ID, 0, 0, 76, NULL, NULL, NULL, NULL, NULL, NULL);
//	FILE *hFile = fopen("aidata.txt", "w");
//  GString *strTemp = g_string_new("");
//  g_string_printf(strTemp, "%d %d %d", shadows[0], shadows[1], shadows[5]);
//  strcpy(strTemp, itoa();
//  fwrite(str(int(shadows[0])) + " " + str(int(shadows[1])) + " " + str(int(shadows[5] * 100)) + " ")
//	fwrite(str(int(midtones[0])) + " " + str(int(midtones[1])) + " " + str(int(midtones[5] * 100)) + " ")
//	fwrite(str(int(highlights[0])) + " " + str(int(highlights[1])) + " " + str(int(highlights[5] * 100)) + " ")
//}

void doBaseMap(gint32 image_ID, gint32 diffuse_ID, gfloat Depth, gint32 passes, gint defAO)
{
    gfloat fGausianFactor = defAO * 0.06f;
    gfloat fScalar = defAO / 50.0f;
    gint32 i = 0 ;
    
    for (i = 0; i < passes; i++)
    {
        /** Copy active layer. */
        /** LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL */
        gint32 newlayer_ID = gimp_layer_copy (diffuse_ID);
        gfloat ok = (gfloat)(i + 1) * (gfloat)(i + 1);
        /** Add the new layer to the image. */
        gimp_image_add_layer(image_ID, newlayer_ID, -1);
        /** Set new layer to the active layer. */
        gimp_image_set_active_layer(image_ID, newlayer_ID);
        
        /**
         * Filter "Gaussian Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         * 
         * Add the ".0f" here to signify float in c language.
         */
        // if (plug_in_gauss_connector(image_ID, newlayer_ID, ok * 3.0f, ok * 3.0f, 0) != 1) return;
        if (plug_in_gauss_connector(image_ID, newlayer_ID, ok * fGausianFactor, ok * fGausianFactor, 0) != 1) return;

        /**
         * Filter "Normalmap" applied
         * Non-Standard plug-in. Source code included.
         * 
         * original values:
                 * plug_in_normalmap_derby(image_ID, newlayer_ID, 5, 0.0, Depth * ok, 0, 0, 0, 0, 0, 0, 1, 0, 0.0, newlayer_ID)
         */
        nmapvals.filter = 5;
        nmapvals.minz = 0.0f;
        nmapvals.scale = Depth * ok;
        nmapvals.wrap = 0;
        nmapvals.height_source = 0;
        nmapvals.alpha = 0;
        nmapvals.conversion = 0;
        nmapvals.dudv = 0;
        nmapvals.xinvert = 0;
        nmapvals.yinvert = 1;
        nmapvals.swapRGB = 0;
        nmapvals.contrast = 0.0f;
        nmapvals.alphamap_id = newlayer_ID;
        normalmap(newlayer_ID, 0);
		
        if (i > 0)
        {
            /**
             * Originally removed by Omar Emad
             * 
             * gimp_layer_set_opacity(newlayer,50-(i*10))
             */
             
            gimp_layer_set_mode(newlayer_ID, 5);
            /** MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM */
            gimp_image_merge_down(image_ID, newlayer_ID, 0);
            newlayer_ID = gimp_image_get_active_layer(image_ID);

            /**
             * Filter "Normalmap" applied
             * Non-Standard plug-in. Source code included.
             * 
             * original values:
             * plug_in_normalmap_derby(image_ID, newlayer_ID, 0, 0.0, 1.0, 0, 0, 0, 8, 0, 0, 0, 0, 0.0, newlayer_ID)
             */
            nmapvals.filter = 0;
            nmapvals.minz = 0.0f;
            // nmapvals.scale = 1.0f;
            nmapvals.scale = fScalar;
            nmapvals.wrap = 0;
            nmapvals.height_source = 0;
            nmapvals.alpha = 0;
            nmapvals.conversion = 8;
            nmapvals.dudv = 0;
            nmapvals.xinvert = 0;
            nmapvals.yinvert = 0;
            nmapvals.swapRGB = 0;
            nmapvals.contrast = 0.0f;
            nmapvals.alphamap_id = newlayer_ID;
            normalmap(newlayer_ID, 0);
        }
    }
    
    /** The result of doBaseMap will be one extra layer. */
    
    /**
     * Removed by Derby Russell
     * Not Used any where and no
     * modifications are being made.
     * drawable = gimp_image_get_active_layer(image_ID)
     */
    
    /**
     * Originally removed by Omar Emad
     * 
     *  newlayer=gimp_layer_copy (drawable, 1)
     *  gimp_image_add_layer(image,newlayer,-1)
     *  gimp_image_set_active_layer(image,newlayer)
     *  gimp_levels_stretch(newlayer)
     *  gimp_layer_set_opacity(newlayer,100)
     *  gimp_layer_set_mode(newlayer,5)
     *  gimp_image_merge_down(image,newlayer,0)		 
     */
}		

/**
 * The run function for Insanemap
 * 
 * @param img_ID         - gint32 image id of image to process with Insanemap.
 * 
 * imapvals provides all these parameters:
 * @param RemoveLighting - gint32 BOOL TRUE OR FALSE to remove lighting.
 * @param Resizie        - gint32 BOOL TRUE OR FALSE to resize image with a multiple of newWidth Integer.
 * @param Tile           - gint32 Tile images BOOL TRUE OR FALSE.
 * @param newWidth       - gint32 if Resizie is TRUE this value will increase both the width and height by this factor.
 * @param EdgeSpecular   - gint32 Edge Enhancing Specular BOOL TRUE OR FALSE.
 * @param defSpecular    - gint32 Specular Definition (0-255)
 * @param Depth          - gfloat Depth(+/-)
 * @param LargeDetails   - gint32 Large Detail Size (~3).
 * @param MediumDetails  - gint32 Medium Detail Intensity(%)
 * @param SmallDetails   - gint32 Small Detail Intensity(%)
 * @param ShapeRecog     - gint32 Shape Recognition(%)
 * @param smoothstep     - gint32 Smooth Step BOOL TRUE OR FALSE
 * @param Noise          - gint32 Apply Noise BOOL TRUE OR FALSE
 * @param invh           - gint32 Invert Height Map BOOL TRUE OR FALSE
 * @param ao             - gint32 ao ? not used anywhere, WHY?
 * @param prev           - gint32 Preview BOOL TRUE OR FALSE
 */
gint32 render(gint32 img_ID, PlugInVals *vals)
{
    gchar *file_name = gimp_image_get_filename(img_ID);
    gint32 image_ID = img_ID;
    gint size = gimp_image_width(image_ID);
    gfloat wsize = 0.0f;
    gfloat hsize = 0.0f;
    gint32 diffuse_ID = 0;
    gint32 normalmap_ID = 0;
    GString *file_name_temp = NULL;
    gint32 drawable_ID = 0;
    gint32 nResult = 0;
    gfloat fScalar = 0.0f;

    
/*******************************************************************************
 * Begin Diffuse.
 ******************************************************************************/    
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.0);
    gtk_label_set_text(GTK_LABEL(progress_label), "Begin");

    if(vals->Resizie)
    {
        // gimp_message("Resizing!");
        /**
         * Originally removed by Omar Emad
         * 
         * upScale(getImgPath(),file_name,newWidth,size)
         */

        gboolean hiresimg = gimp_image_resize(image_ID, (gint)(vals->newWidth * size), (gint)(vals->newWidth * size), 0, 0);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.01);
        gtk_label_set_text(GTK_LABEL(progress_label), "Resizing");

        /**
         * Since hiresimg is boolean and not an image id,
         * I am using it here as a double check.
         * I know this is not what was intended.
         * 
         * See below.
         */
        if (hiresimg)
        {
            /**
             * Originally removed by Omar Emad
             * 
             * hiresimg = gimp_file_load( getFilename(file_name,"_hd"), file_name)
             */

            /**
             * I believe this was an error.  hiresimg is a boolean value.
             * It looks as if it is being used as an image id.  gimp_image_resize causes the given
             * image id to be resized.  So, just using image_ID here is all that is needed after
             * the resize.  I am changing this code to do just that.
             * - Derby Russell
             * 
             * Old code:
             * gint32 drawable_ID = gimp_image_get_active_layer(hiresimg)
             */
            drawable_ID = gimp_image_get_active_layer(image_ID);
            gimp_layer_scale(drawable_ID, (gint)(vals->newWidth * size), (gint)(vals->newWidth * size), FALSE);
            // gimp_layer_resize_to_image_size(drawable_ID);
            gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.02);
        }
    }
    else
    {
        drawable_ID = gimp_image_get_active_layer(image_ID);
        gtk_label_set_text(GTK_LABEL(progress_label), "Stretch");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.03);
    }
    
    if (vals->Noise) {
        gfloat fGausianNoiseFactor = vals->ao * 0.0040f;
        gint32 noiselayer_ID = gimp_layer_copy (drawable_ID);
        /**
         * Old code:
         * gimp_image_add_layer(hiresimg, noiselayer, -1)
         * gimp_image_set_active_layer(hiresimg, noiselayer)
         * plug_in_rgb_noise(hiresimg, noiselayer, 1, 1, 0.20, 0.20, 0.20, 0)
         */
        gimp_image_add_layer(image_ID, noiselayer_ID, -1);
        gimp_image_set_active_layer(image_ID, noiselayer_ID);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.03);

        /**
         * Filter "RGB Noise" applied
         * Standard plug-in. Source code ships with GIMP.
         * 
         * Add the "f" here to signify float in c language.
         */
        gtk_label_set_text(GTK_LABEL(progress_label), "Noise");
        // if (plug_in_rgb_noise_connector(image_ID, noiselayer_ID, 1, 1, 0.20f, 0.20f, 0.20f, 0.0f) != 1) return 0;
        if (plug_in_rgb_noise_connector(image_ID, noiselayer_ID, 1, 1, fGausianNoiseFactor, fGausianNoiseFactor, fGausianNoiseFactor, 0.0f) != 1) return 0;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.04);

        /**
         * Originally removed by Omar Emad
         * 
         * pdb.plug_in_blur(hiresimg,noiselayer)
         */

        gimp_layer_set_mode(noiselayer_ID, 14);

        /**
         * Old code:
         * gimp_image_merge_down(hiresimg, noiselayer, 0)
         */
        gimp_image_merge_down(image_ID, noiselayer_ID, 0);

        /**
         * Old code:
         * image = hiresimg
         * 
         * As I suspected.  This line of code was needed due
         * to the possible error above.  I am removing
         * this line and the error should be resolved.
         * - Derby Russell
         */

        /**
         * This next line is not needed due to the fact
         * that the line after that is commented out.
         * I am commenting out this line now.
         * - Derby Russell
         * 
         * drawable_ID = gimp_image_get_active_layer(image_ID);
         */

        /**
         * Originally removed by Omar Emad
         * 
         * ###########################analyze(drawable)
         */
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Noise added");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.04);
    }
         
    if(vals->RemoveLighting)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Remove Shading");
        removeShading(image_ID);
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.05);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Stretch");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.05);
    }
		
    diffuse_ID = gimp_image_get_active_layer(image_ID);
    gimp_levels_stretch(diffuse_ID);
    file_name_temp = getFilename(file_name, "_d");
    if(vals->Tile)
    {
        /**
         * Filter "Tile Seamless" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        gtk_label_set_text(GTK_LABEL(progress_label), "Making Seamless");
        if (plug_in_make_seamless_connector(image_ID, diffuse_ID) != 1) return 0;
        set_progress_label_as_file(file_name_temp, "_d");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.07);
    }
    else
    {
        set_progress_label_as_file(file_name_temp, "_d");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.07);
    }
    
    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, diffuse_ID, file_name_temp->str, file_name_temp->str);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.1);

/*******************************************************************************
 * Begin H and Normal
 ******************************************************************************/    

    /**
    * Originally removed by Omar Emad
    * 
     * drawable = gimp_image_get_active_layer(image)
     * gimp_levels(drawable,0,64,128,1,0,255)
    */

    wsize = (gfloat)gimp_image_width(image_ID);
    hsize = (gfloat)gimp_image_width(image_ID);
    gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
    blur(image_ID, diffuse_ID, wsize, hsize, vals->LargeDetails, 0, vals->ao);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.15);

    file_name_temp = getFilename(file_name, "_h");
    normalmap_ID = gimp_image_get_active_layer(image_ID);
    if(vals->smoothstep)
    {
        /**
         * Filter "Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
        if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return 0;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.20);
    }
    else
    {
        set_progress_label_as_file(file_name_temp, "_h");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.20);
    }
    if(vals->invh)
    {
        /**
         * Colors Menu->Invert
         * Standard plug-in. Source code ships with GIMP.
         */
        if (plug_in_vinvert_connector(image_ID, normalmap_ID) != 1) return 0;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.25);
    }
    else
    {
        set_progress_label_as_file(file_name_temp, "_h");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.25);
    }

    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, normalmap_ID, file_name_temp->str, file_name_temp->str);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.30);
	
    /**
     * Originally removed by Omar Emad
     * 
     * lfnormal=gimp_layer_copy (diffuse, 1)
     * gimp_image_add_layer(image,lfnormal,-1)
     * gimp_image_raise_layer_to_top(image,lfnormal)
     * pdb.plug_in_normalmap_derby(image, lfnormal,8,0.0,Depth,0,0,0,0,0,0,0,0,0.0,normalmap)
     * blur(image,lfnormal,LargeDetails,1)
     * normalmap = gimp_image_get_active_layer(image)
     */

    gtk_label_set_text(GTK_LABEL(progress_label), "Base Mapping");
    doBaseMap(image_ID, diffuse_ID, vals->Depth, vals->LargeDetails, vals->ao);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.35);
    normalmap_ID = gimp_image_get_active_layer(image_ID);

    file_name_temp = getFilename(file_name, "_ln");
    set_progress_label_as_file(file_name_temp, "_ln");
    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, normalmap_ID, file_name_temp->str, file_name_temp->str);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.40);

    file_name_temp = getFilename(file_name, "_sn");
    shapeRecognise(image_ID, normalmap_ID, vals->ShapeRecog, vals->ao);
    if(vals->smoothstep)
    {
        normalmap_ID = gimp_image_get_active_layer(image_ID);

        /**
         * Filter "Blur" applied
         * Standard plug-in. Source code ships with GIMP.
         */
        gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
        if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return 0;
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.45);
    }
    else
    {
        set_progress_label_as_file(file_name_temp, "_sn");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.45);
    }

    saveLastOperation(image_ID, file_name_temp);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.50);

    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen");
    sharpen(image_ID, diffuse_ID, vals->Depth, 0, vals->SmallDetails);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.55);
    normalmap_ID = gimp_image_get_active_layer(image_ID);

    /**
    * Filter Enhance "Sharpen" applied
    * Standard plug-in. Source code ships with GIMP.
    */
    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen more");
    if (plug_in_sharpen_connector(image_ID, normalmap_ID, 20) != 1) return 0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.60);

    file_name_temp = getFilename(file_name, "_hn");
    set_progress_label_as_file(file_name_temp, "_hn");
    saveLastOperation(image_ID, file_name_temp);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.65);

    gtk_label_set_text(GTK_LABEL(progress_label), "Sharpen again");
    sharpen(image_ID, diffuse_ID, vals->Depth, 6, vals->MediumDetails);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.70);
    normalmap_ID = gimp_image_get_active_layer(image_ID);
    
    /**
     * Filter "Blur" applied
     * Standard plug-in. Source code ships with GIMP.
     */
    gtk_label_set_text(GTK_LABEL(progress_label), "Smoothing");
    if (plug_in_blur_connector(image_ID, normalmap_ID) != 1) return 0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.75);

    file_name_temp = getFilename(file_name, "_mn");
    set_progress_label_as_file(file_name_temp, "_mn");
    saveLastOperation(image_ID, file_name_temp);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.78);
	
    gimp_drawable_set_visible(diffuse_ID, 0);
    gimp_image_merge_visible_layers(image_ID, 0);

    drawable_ID = gimp_image_get_active_layer(image_ID);
    
/*******************************************************************************
 * Begin Normal
 ******************************************************************************/    

    /**
     * Filter "Normalmap" applied
     * Non-Standard plug-in. Source code included.
     * 
     * original values:
     * plug_in_normalmap_derby(image, drawable, 0, 0.0, 1.0, 0, 0, 0, 8, 0, 0, 0, 0, 0.0, drawable)
     */
    fScalar = vals->ao / 50.0f;
    nmapvals.filter = 0;
    nmapvals.minz = 0.0f;
    // nmapvals.scale = 1.0f;
    nmapvals.scale = fScalar;
    nmapvals.wrap = 0;
    nmapvals.height_source = 0;
    nmapvals.alpha = 0;
    nmapvals.conversion = 8;
    nmapvals.dudv = 0;
    nmapvals.xinvert = 0;
    nmapvals.yinvert = 0;
    nmapvals.swapRGB = 0;
    nmapvals.contrast = 0.0f;
    nmapvals.alphamap_id = drawable_ID;
    gtk_label_set_text(GTK_LABEL(progress_label), "Normal map");
    normalmap(drawable_ID, 0);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.80);

    file_name_temp = getFilename(file_name, "_n");
    set_progress_label_as_file(file_name_temp, "_n");
    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, drawable_ID, file_name_temp->str, file_name_temp->str);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.85);

/*******************************************************************************
 * Begin Ambient Occlusion
 ******************************************************************************/    

    /**
     * Colors ->  Components -> "Channel Mixer" applied
     * Standard plug-in. Source code ships with GIMP.
     * 
     * Add the "f" here to signify float in c language.
     * Removed 0.0 on first param and changed to 0 because boolean.
     */
    gtk_label_set_text(GTK_LABEL(progress_label), "Mixing Colors");
    /**
     * In Gimp the Channel Mixer is found in the menu system at:
     * 
     * Colors->Components->Channel Mixer
     * 
     * Parameters for colors channel mixer:                                             SETTINGS  Corrected
     * { GIMP_PDB_INT32,    "run-mode",   "Interactive, non-interactive" },
     * { GIMP_PDB_IMAGE,    "image",      "Input image (unused)" },
     * { GIMP_PDB_DRAWABLE, "drawable",   "Input drawable" },
     * { GIMP_PDB_INT32,    "monochrome", "Monochrome (TRUE or FALSE)" },                FALSE    FALSE
     * { GIMP_PDB_FLOAT,    "rr-gain",    "Set the red gain for the red channel" },     -200.0f   -1.0f
     * { GIMP_PDB_FLOAT,    "rg-gain",    "Set the green gain for the red channel" },    0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "rb-gain",    "Set the blue gain for the red channel" },     0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "gr-gain",    "Set the red gain for the green channel" },    0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "gg-gain",    "Set the green gain for the green channel" }, -200.0f   -1.0f
     * { GIMP_PDB_FLOAT,    "gb-gain",    "Set the blue gain for the green channel" },   0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "br-gain",    "Set the red gain for the blue channel" },     0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "bg-gain",    "Set the green gain for the blue channel" },   0.0f      0.0f
     * { GIMP_PDB_FLOAT,    "bb-gain",    "Set the blue gain for the blue channel" }     1.0f      1.0f
     * 
     * view as matrix:
     * (plug-in-colors-channel-mixer RUN-NONINTERACTIVE image layer FALSE
     *                         -1  0  0
     *                          0 -1  0
     *                          0  0  1 )
     * 
     */
    // if (plug_in_colors_channel_mixer_connector(image_ID, drawable_ID, 0, -200.0f, 0.0f, 0.0f, 0.0f, -200.0f, 0.0f, 0.0f, 0.0f, 1.0f) != 1) return 0;
    if (plug_in_colors_channel_mixer_connector(image_ID, drawable_ID, 0, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f) != 1) return 0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.87);

    gimp_desaturate(drawable_ID);
    gimp_levels_stretch(drawable_ID);
    file_name_temp = getFilename(file_name, "_a");
    set_progress_label_as_file(file_name_temp, "_a");
    gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_ID, drawable_ID, file_name_temp->str, file_name_temp->str);
    g_string_free(file_name_temp, TRUE);
    file_name_temp = NULL;

    gimp_drawable_set_visible(diffuse_ID, 1);
    gimp_image_set_active_layer(image_ID, diffuse_ID);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 0.90);

/*******************************************************************************
 * Begin Specular
 ******************************************************************************/    
	
    if(vals->EdgeSpecular)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Specular Edging");
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Specular Smoothing");
    }

    nResult = specularDo(image_ID, file_name, vals->defSpecular, vals->ao, vals->EdgeSpecular);

    if(vals->EdgeSpecular)
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Idle...");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 1.0);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(progress_label), "Idle...");
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 1.0);
    }

    return nResult;
}

