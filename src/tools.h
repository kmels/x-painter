#ifndef TOOLS
#define TOOLS

inline void put_pixel(cairo_t *cr,double x, double y);
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2);
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2);
void rectangle(cairo_t *cr, double x1, double y1, double x2, double y2);
void flood_fill(cairo_t *cr, double x1, double y1);
gboolean finish_polygon(mouseStateStruct *mouseState,double x,double y);
#endif

