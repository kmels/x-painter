#ifndef TOOLS
#define TOOLS

inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2);

void undo(cairo_t *canvas_context);
void save_surface_in_history(cairo_surface_t *surface);
#endif

