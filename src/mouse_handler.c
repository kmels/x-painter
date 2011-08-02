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
    gboolean save_dragging;
  } mouseState;
  gint type = GPOINTER_TO_INT(t);
    
  switch(type){
    case MOUSE_CLICK:{	
      GdkEventButton *event = (GdkEventButton*) e;
      switch(event->type) {
         case GDK_BUTTON_PRESS: {
	   mouseState.ispainting = TRUE;
	   mouseState.cr = gdk_cairo_create(widget->window);	   
	   save_current_surface(cairo_get_target(mouseState.cr));
	   cairo_set_source_rgb(mouseState.cr, 0, 0, 0);
	   cairo_set_source_rgb(mouseState.cr, 0.3, 0.4, 0.6);
	   cairo_set_line_width(mouseState.cr, 1);

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
	mouseState.ispainting = FALSE;

	switch(current_tool){
	  case XPainter_UNDO_TOOL: break;
	case XPainter_LINE_TOOL: line(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y);
	default: {
	  save_current_surface(cairo_get_target(mouseState.cr));
	  save_current_surface_in_history(); 
	}break;
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
      
      mouseState.save_dragging = TRUE;

      if (!mouseState.ispainting) //are we doing painting?
	return TRUE;
      
      switch(current_tool){
      case XPainter_BRUSH_TOOL: brush(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x, event->y); break; 
      case XPainter_LINE_TOOL: {	
	mouseState.save_dragging = FALSE;
	paint_current_surface_on_canvas(mouseState.cr);	
	line(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);
      } break;
         default: break;
      }
      
      if (mouseState.save_dragging){
	mouseState.coordinates[mouseState.coordinates_size].x = event->x;
	mouseState.coordinates[mouseState.coordinates_size].y = event->y;      
	mouseState.coordinates_size++;
      }

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
