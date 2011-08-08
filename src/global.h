#ifndef GLOBAL
#define GLOBAL

#include "types/xpainter_toolitem_id.h"

/* GUI */  
GtkWidget *window;
GtkWidget *canvas;

/* Types */
struct history{
  cairo_surface_t *surfaces[100];
  int current_index;
  int count;
};

typedef struct{
  guint16 red;
  guint16 green;
  guint16 blue;
  guint16 alpha;
} XPainterColor;

/* Functionality */
XPainterToolItemType current_tool;
int line_width;
XPainterColor color1, color2;

cairo_surface_t *current_surface;
GdkPixbuf *current_surface_pixbuf;
guchar *pixbuf_pixels;

/* Tools */
gboolean selection_is_on;
double selection_x1, selection_y1, selection_x2, selection_y2;
double move_x1, move_y1;
gboolean figure_is_filled;

/* Dirty variables */
gboolean canvas_drawn;

extern struct history canvas_history;

void assign_current_tool(GtkWidget *widget, gpointer data);
void undo(GtkWidget *widget, gpointer data);
void redo(GtkWidget *widget, gpointer data);

void paint_current_surface_on_canvas(cairo_t *canvas_context);
void save_current_surface(cairo_surface_t *surface);
void save_current_surface_in_history();
void set_new_canvas(GtkWidget *widget, gpointer data);
void open_file(GtkWidget *widget, gpointer data);
void save_file(GtkWidget *widget, gpointer data);
#endif
