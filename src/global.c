#include <gtk/gtk.h>
#include "global.h"

void assign_current_tool(GtkWidget *widget, gpointer data){  
  current_tool = (XPainterToolItemType) data;
}
