#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include "types/xpainter_toolitem_id.h"
#include "types/coordinate.h"
#include "tools.h"
#include <string.h> /* memset */

gboolean handle_mouse(GtkWidget *widget, void *e, gpointer *t){
  static struct {        
    gboolean ispainting;    
    cairo_t *cr;    
    //double x, y;
    Coordinate coordinates[5000];
    int coordinates_size;
  } mouseState;
  gint type = GPOINTER_TO_INT(t);
    
  switch(type){
    case MOUSE_CLICK:{	
      GdkEventButton *event = (GdkEventButton*) e;
      switch(event->type) {
         case GDK_BUTTON_PRESS: {
	   mouseState.ispainting = TRUE;
	   mouseState.cr = gdk_cairo_create(widget->window);	   
	   //printf("%p\n",mouseState.cr);
	   cairo_set_source_rgb(mouseState.cr, 0, 0, 0);
	   cairo_set_source_rgb(mouseState.cr, 0.3, 0.4, 0.6);
	   cairo_set_line_width(mouseState.cr, 1);


	   printf("coordinates_size: %d\n",mouseState.coordinates_size);
	   mouseState.coordinates[mouseState.coordinates_size].x = event->x;
	   mouseState.coordinates[mouseState.coordinates_size].y = event->y;
	   mouseState.coordinates_size++;
	   	   	   
	   switch(current_tool){
	      case XPainter_UNDO_TOOL: undo(mouseState.cr); break;
	      default: break;
	   }
	 }
	   break;
      case GDK_BUTTON_RELEASE: {
	mouseState.ispainting = 0;

	switch(current_tool){
	  case XPainter_UNDO_TOOL: break;
	  default: save_surface_in_history(cairo_get_target(mouseState.cr)); break;
	}
	
	cairo_destroy(mouseState.cr);
	memset(&mouseState, 0, sizeof(mouseState));
      }
	break;
      default:
	break;
      }
    } //end case MOUSE_CLICK
      break;
    case MOUSE_DRAG:{
      GdkEventMotion *event = (GdkEventMotion*) e;
      static	GdkWindow *gdkWindow;
      
      if (!gdkWindow){	// initialize
	  gdkWindow = gdk_get_default_root_window();
	  memset(&mouseState, 0, sizeof(mouseState));
	  return TRUE;
      }
      
      if (!mouseState.ispainting) //are we doing paintint?
	return TRUE;
      
      switch(current_tool){
      case XPainter_BRUSH_TOOL: brush(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x, event->y); break; 
      case XPainter_LINE_TOOL: brush(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x, event->y); break; 
         default: break;
      }
      
      mouseState.coordinates[mouseState.coordinates_size].x = event->x;
      mouseState.coordinates[mouseState.coordinates_size].y = event->y;      
      mouseState.coordinates_size++;

      // even though we don't use the resulting information from this call, 
      // calling it is an indication to the main_loop() 
      // that we are ready to receive the next mouse motion notify event
      gint x, y;
      GdkModifierType state;
      gdk_window_get_pointer(gdkWindow, &x, &y, &state);      
    }
      break;
  default:
    break;
  }

  return TRUE;
}
