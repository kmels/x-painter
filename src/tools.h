#ifndef TOOLS
#define TOOLS

inline void put_pixel(cairo_t *cr,double x, double y);
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2);
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2);
void spray(double x, double y,mouseStateStruct *mouseState);
void rectangle(cairo_t *cr, double x1, double y1, double x2, double y2);
void flood_fill(cairo_t *cr, double x1, double y1);
gboolean finish_polygon(mouseStateStruct *mouseState,double x,double y);
gboolean circle(cairo_t *cr, double x, double y, double xf, double yf);
void mark_selection(cairo_t *cr, double x1, double y1, double x2, double y2);
void save_selection(double x1, double y1, double x2, double y2);
void move(mouseStateStruct *mouseState, double x, double y);
gboolean click_is_within_selection(double x, double y);
inline void single_eraser(cairo_t *cr,double x, double y);
inline void dragged_eraser(cairo_t *cr,double x, double y,double x2, double y2);
void ellipse(cairo_t *cr, double cenx, double ceny, double current_x, double current_y);
void add_fill_widget_to(GtkContainer *box);
void add_vertical_separator_to(GtkContainer *box);
void add_coordinates_label_to(GtkContainer *box);
void fill_rectangle(cairo_t *cr, double x1, double y1, double x2, double y2);
void move_finally(mouseStateStruct *mouseState, double x, double y);
inline void save_new_selection_after_moving(double x,double y);
#endif
