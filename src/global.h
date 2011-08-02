#ifndef GLOBAL
#define GLOBAL

#include "types/xpainter_toolitem_id.h"
XPainterToolItemType current_tool;
int brush_width;

cairo_surface_t *surfaces_history[100];
cairo_surface_t *current_surface;
int current_surface_index;
int surfaces_history_size;

void assign_current_tool(GtkWidget *widget, gpointer data);
#endif
