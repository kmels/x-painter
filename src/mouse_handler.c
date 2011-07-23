#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include "types/xpainter_toolitem_id.h"
#include "tools.h"

int isdragging = 0;

gboolean handle_mouse_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data){  
  if (event->button == 1) {
    isdragging = 1;
  }

  return TRUE;
}

gboolean handle_mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data){  
  if (event->button == 1) {
    isdragging = 0;
  }

  return TRUE;
}

gboolean handle_mouse_drag(GtkWidget *widget, GdkEventButton *event, gpointer user_data){  
  switch (current_tool){
  case XPainter_BRUSH_TOOL: {
    if (isdragging)
      brush(widget, event->x, event->y);
    break;
  }
  default: 
    //printf("OTRO\n");
    break;
  }  

  return TRUE;
}
