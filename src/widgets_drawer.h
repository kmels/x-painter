#ifndef WIDGETS_DRAWER

#define WIDGETS_DRAWER

void get_main_menu(GtkWidget *window, GtkWidget **menubar);
void get_toolbar(GtkWidget *window, GtkWidget **toolbar);
gboolean redraw_canvas(GtkWidget *widget, gpointer userdata);
gboolean update_coordinates_label(GtkWidget *widget, void *e, gpointer *t);
void add_line_width_widget_to(GtkContainer *box);
void adjust_color(GtkColorButton *widget,gpointer data);
void add_color_widgets_to(GtkContainer *box);
void add_alpha_ranges_to(GtkContainer *box);
void add_fill_patterns_widgets_to(GtkContainer *box);
void set_current_fill_pattern_on_widget();
#endif
