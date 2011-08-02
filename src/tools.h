#ifndef TOOLS
#define TOOLS

inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2);
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2);

void undo_current_drawing(cairo_t *canvas_context);
void undo(cairo_t *canvas_context);

void save_current_surface(cairo_surface_t *surface);
void save_current_surface_in_history();
#endif

