#include <gtk/gtk.h>
#include "global.h"

/* Brush, paints a single pixel in coordinate x,y*/
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2){
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);
	cairo_stroke(cr);
}

void undo(cairo_t *cr){
  if (current_surface_index - 1 >= 0){
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,surfaces_history[--current_surface_index],0,0);
    cairo_paint(cr);
  }
}

/* Saves a surface in history */
void save_surface_in_history(cairo_surface_t *surface){  
  cairo_surface_t *surface_to_save = cairo_surface_create_similar(surface, CAIRO_CONTENT_COLOR, 1200, 1200);
  cairo_t *new_cr = cairo_create(surface_to_save);
  
  cairo_set_source_surface(new_cr,surface,0,0);
  cairo_paint(new_cr);

  surfaces_history[++current_surface_index] = surface_to_save;
  surfaces_history_size++;
}

