#include <gtk/gtk.h>
#include "mouse_handler.h"
#include "widgets_drawer.h"

int main( int argc,
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  GtkWidget *toolbar;

  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (gtk_main_quit), "WM destroy");
  gtk_window_set_title (GTK_WINDOW(window), "X Painter");
  gtk_widget_set_usize (GTK_WIDGET(window), 300, 200);
  
  main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_border_width (GTK_CONTAINER (main_vbox), 1);
  gtk_container_add (GTK_CONTAINER (window), main_vbox);
  gtk_widget_show (main_vbox);
  
  get_main_menu (window, &menubar);
  get_toolbar(window, &toolbar);

  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), toolbar, FALSE, TRUE, 0);
  
  
  //gtkdrawable + cairo
  GtkWidget *canvas = gtk_drawing_area_new();  
  gtk_widget_set_size_request (canvas, 500, 600);
  
  //gtk_widget_add_events(canvas, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(canvas, GDK_ALL_EVENTS_MASK);
  g_signal_connect(canvas, "button-press-event", G_CALLBACK(handle_mouse_click), NULL);
  g_signal_connect(canvas, "button-release-event", G_CALLBACK(handle_mouse_release), NULL);
  g_signal_connect(canvas, "motion-notify-event",G_CALLBACK(handle_mouse_drag), NULL);

  gtk_box_pack_start (GTK_BOX (main_vbox), canvas, FALSE, TRUE, 0);
  
  //ends gtkdrawable + cairo example
  
  gtk_widget_show (menubar);
  
  gtk_widget_show_all (window);
  gtk_main ();
  
  return(0);
}
/* example-end */
