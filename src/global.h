#ifndef GLOBAL
#define GLOBAL

#include "types/xpainter_toolitem_id.h"

/* GUI */
GtkWidget *canvas;

/* Functionality */
XPainterToolItemType current_tool;
int line_width;
GdkColor color1, color2;

cairo_surface_t *current_surface;

/* Dirty variables */
gboolean canvas_drawn;

/* Types */
struct history{
  cairo_surface_t *surfaces[100];
  int current_index;
  int count;
};

extern struct history canvas_history;

void assign_current_tool(GtkWidget *widget, gpointer data);
void undo(GtkWidget *widget, gpointer data);
void redo(GtkWidget *widget, gpointer data);

void paint_current_surface_on_canvas(cairo_t *canvas_context);
void save_current_surface(cairo_surface_t *surface);
void save_current_surface_in_history();
#endif
