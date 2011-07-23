#include <gtk/gtk.h>
#include "global.h"

/* Brush, paints a single pixel in coordinate x,y*/
void brush(GtkWidget *widget, double x, double y){
  cairo_t * cr;
  cr = gdk_cairo_create(widget->window);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width (cr, 0.5);  
  cairo_rectangle(cr,x,y,1,1);    
  cairo_stroke(cr);
  cairo_destroy(cr);
}
