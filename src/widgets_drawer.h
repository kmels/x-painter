#ifndef WIDGETS_DRAWER

#define WIDGETS_DRAWER

void get_main_menu(GtkWidget *window, GtkWidget **menubar);
void get_toolbar(GtkWidget *window, GtkWidget **toolbar);
gboolean redraw_canvas(GtkWidget *widget, gpointer userdata);
gboolean update_coordinates_label(GtkWidget *widget, void *e, gpointer *t);
#endif
