#include <gtk/gtk.h>

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
  if (isdragging) {
    cairo_t * cr;
    cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width (cr, 0.5);

    cairo_rectangle(cr,event->x,event->y,1,1);
    
    
    cairo_stroke(cr);
    cairo_destroy(cr);    
  }

  return TRUE;
}
