#include <gtk/gtk.h>
#include "global.h"

void assign_current_tool(GtkWidget *widget, gpointer data){  
  current_tool = (XPainterToolItemType) data;
  gtk_widget_grab_focus(widget);
}

void undo(GtkWidget *widget, gpointer data){  
  cairo_t *cr = gdk_cairo_create(canvas->window);

  if (canvas_history.current_index - 1 >= 0){  
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,canvas_history.surfaces[--canvas_history.current_index],0,0);
    cairo_paint(cr);
  }
}

void redo(GtkWidget *widget, gpointer data){  
  cairo_t *cr = gdk_cairo_create(canvas->window);

  if (canvas_history.current_index + 1 <= canvas_history.count){
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,canvas_history.surfaces[++canvas_history.current_index],0,0);
    cairo_paint(cr);
  }
}

/* Saves a surface, tipically called before drawing anything new, so we can simulate drawing on it*/
void save_current_surface(cairo_surface_t *surface){
  cairo_surface_t *surface_to_save = cairo_surface_create_similar(surface, CAIRO_CONTENT_COLOR, canvas->allocation.width,canvas->allocation.height);
  cairo_t *new_cr = cairo_create(surface_to_save);
  cairo_set_source_surface(new_cr,surface,0,0);
  cairo_paint(new_cr);
  current_surface = surface_to_save;

  //save pixbuf
  current_surface_pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),0,0,0,0,canvas->allocation.width,canvas->allocation.height);
  //save pixels
  pixbuf_pixels = gdk_pixbuf_get_pixels (current_surface_pixbuf);
}

void paint_current_surface_on_canvas(cairo_t *cr){
  cairo_save (cr);
  cairo_set_source_surface(cr,current_surface,0,0);
  cairo_paint (cr);
  cairo_restore (cr);
}

/* Saves a surface in history */
void save_current_surface_in_history(){
  canvas_history.surfaces[++canvas_history.current_index] = current_surface;
  canvas_history.count = canvas_history.current_index;
}
