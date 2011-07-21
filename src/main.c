#include <gtk/gtk.h>
#include "widgets_drawer.h"

int main( int argc,
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy", 
                      GTK_SIGNAL_FUNC (gtk_main_quit), 
                      "WM destroy");
  gtk_window_set_title (GTK_WINDOW(window), "Item Factory");
  gtk_widget_set_usize (GTK_WIDGET(window), 300, 200);
  
  main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_border_width (GTK_CONTAINER (main_vbox), 1);
  gtk_container_add (GTK_CONTAINER (window), main_vbox);
  gtk_widget_show (main_vbox);
  
  get_main_menu (window, &menubar);
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
  gtk_widget_show (menubar);
  
  gtk_widget_show (window);
  gtk_main ();
  
  return(0);
}
/* example-end */
