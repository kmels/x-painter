#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include "types/xpainter_toolitem_id.h"
#include "tools.h"
#include <string.h> /* memset */

int isdragging = 0;

gboolean handle_mouse(GtkWidget *widget, void *e, gpointer *t){
  static struct {
    gboolean isdragging;
    cairo_t *cr;
    double x, y;
  } mouseState;
  gint type = GPOINTER_TO_INT(t);
  
  switch(type){
    case MOUSE_CLICK:{
      GdkEventButton *event = (GdkEventButton*) e;
      switch(event->type) {
         case GDK_BUTTON_PRESS: {
	   mouseState.isdragging = TRUE;
	   mouseState.cr = gdk_cairo_create(widget->window);
	   cairo_set_source_rgb(mouseState.cr, 0, 0, 0);
	   cairo_set_line_width(mouseState.cr, 2);
	   mouseState.x = event->x;
	   mouseState.y = event->y; 
	 }
	   break;
      case GDK_BUTTON_RELEASE: {
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
      
      if (!mouseState.isdragging ||
	  (mouseState.x == event->x && mouseState.y == event->y))
	return TRUE;
      
      brush(mouseState.cr, mouseState.x, mouseState.y, event->x, event->y);
      mouseState.x = event->x;		mouseState.y = event->y;
      
      
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
