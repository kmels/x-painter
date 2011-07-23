#ifndef GLOBAL
#define GLOBAL

#include "types/xpainter_toolitem_id.h"
XPainterToolItemType current_tool;
int brush_width;

void assign_current_tool(GtkWidget *widget, gpointer data);
#endif

