#include <gtk/gtk.h>
#include "global.h"

/* Brush, paints a single pixel in coordinate x,y*/
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2){
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);
	cairo_stroke(cr);
}
