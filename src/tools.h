#ifndef TOOLS
#define TOOLS

inline void put_pixel(cairo_t *cr,double x, double y);
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2);
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2);

void paint_current_surface_on_canvas(cairo_t *canvas_context);
void undo(cairo_t *canvas_context);

void save_current_surface(cairo_surface_t *surface);
void save_current_surface_in_history();
#endif

