#include <gtk/gtk.h>
#include "global.h"
#include <string.h>

GtkWidget *window;
gboolean canvas_drawn;
char *filename;

void assign_current_tool(GtkWidget *widget, gpointer data){  
  current_tool = (XPainterToolItemType) data;
  gtk_widget_grab_focus(widget);
}

void undo(GtkWidget *widget, gpointer data){  
  cairo_t *cr = gdk_cairo_create(canvas->window);

  if (canvas_history.current_index - 1 >= 0){  
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,canvas_history.surfaces[--canvas_history.current_index],0,0);
    cairo_paint(cr);
  }
}

void redo(GtkWidget *widget, gpointer data){  
  cairo_t *cr = gdk_cairo_create(canvas->window);

  if (canvas_history.current_index + 1 <= canvas_history.count){
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,canvas_history.surfaces[++canvas_history.current_index],0,0);
    cairo_paint(cr);
  }
}

/* Saves a surface, tipically called before drawing anything new, so we can simulate drawing on it*/
void save_current_surface(cairo_surface_t *surface){
  //printf("Salvando current \n");
  cairo_surface_t *surface_to_save = cairo_surface_create_similar(surface, CAIRO_CONTENT_COLOR, canvas->allocation.width,canvas->allocation.height);
  cairo_t *new_cr = cairo_create(surface_to_save);
  cairo_set_source_surface(new_cr,surface,0,0);
  cairo_paint(new_cr);
  current_surface = surface_to_save;

  //save pixbuf
  current_surface_pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),0,0,0,0,canvas->allocation.width,canvas->allocation.height);
  //save pixels
  pixbuf_pixels = gdk_pixbuf_get_pixels (current_surface_pixbuf);  
}

void paint_current_surface_on_canvas(cairo_t *cr){
  cairo_save (cr);
  cairo_set_source_surface(cr,current_surface,0,0);
  cairo_paint (cr);
  cairo_restore (cr);
}

void show_error_message(char *message){
  GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
				   GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,
				   GTK_BUTTONS_CLOSE,
				   "%s",message);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

/* Saves a surface in history */
void save_current_surface_in_history(){
  //printf("Salvando historia \n");
  canvas_history.surfaces[++canvas_history.current_index] = current_surface;
  canvas_history.count = canvas_history.current_index;
}

/* Sets new canvas, and cleans history */
void set_new_canvas(GtkWidget *widget, gpointer data){
  cairo_t *cr = gdk_cairo_create(canvas->window);
  cairo_set_source_rgb(cr,1,1,1);
  cairo_paint(cr);
  save_current_surface(cairo_get_target(cr));
  canvas_history.current_index = -1;
  save_current_surface_in_history();
}

/* Returns true iff could load the file successfully*/
gboolean set_new_canvas_from_file(char *open_filename){
  size_t filename_length = strlen(open_filename);
  char *extension = strndup(open_filename+filename_length-3,filename_length);
  
  cairo_t *window_cr = gdk_cairo_create(canvas->window);

  GdkPixbuf *image_pixbuf = NULL;
  //we must create a new surface and work on it, since, if we want to save and do cairo_get_target(window_cr), we'd get the current window, but if it has something over it and we would  include it
  cairo_surface_t *new_surface = cairo_surface_create_similar(cairo_get_target(window_cr),CAIRO_CONTENT_COLOR_ALPHA,canvas->allocation.width,canvas->allocation.height);
  cairo_t *new_surface_cr = cairo_create(new_surface);
  
  if ((strcmp(extension,"bmp")==0) || (strcmp(extension,"png")==0) || (strcmp(extension,"jpg")==0) || (strcmp(extension,"gif")==0) || (strcmp(extension,"pdf")==0))
    image_pixbuf= gdk_pixbuf_new_from_file(open_filename,NULL);
  
  //set the surface, and clean history
  if (image_pixbuf!=NULL){
    //paint white rectangle behind
    cairo_set_source_rgb(new_surface_cr,1,1,1);
    cairo_paint(new_surface_cr);
    //set the image surface to the new canvas
    gdk_cairo_set_source_pixbuf(new_surface_cr,image_pixbuf,0,0);
    //cairo_set_source_surface(new_surface_cr,image_surface,0,0);
    cairo_paint(new_surface_cr);
    //set the new canvas
    cairo_set_source_surface(window_cr,cairo_get_target(new_surface_cr),0,0);
    cairo_paint(window_cr);
    //save new surface, 
    save_current_surface(cairo_get_target(new_surface_cr));
    canvas_history.current_index = -1;
    save_current_surface_in_history();
    return TRUE;
  }
  
  return FALSE;
}

void open_file(GtkWidget *widget, gpointer data){
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Abrir Archivo",
					GTK_WINDOW(window),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
    
  gboolean could_load = TRUE;
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){      
      char *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      could_load = set_new_canvas_from_file(filename);
  }
  
  gtk_widget_destroy (dialog);  
  
  if (!could_load)
    show_error_message("Ocurrio un error al abrir al archivo, extensiones soportadas: PNG, BMP, GIF, PDF y JPG");
}

void save_canvas_as(char* target_filename){
  size_t filename_length = strlen(target_filename);
  char *extension = strndup(target_filename+filename_length-3,filename_length);  
  
  GError *error = NULL;
  gboolean saved = gdk_pixbuf_save (current_surface_pixbuf, target_filename, extension, &error,NULL);
  
  if (!saved)
    show_error_message("Ocurrio un error al guardar el archivo. Formatos soportados: BMP y PNG");
  filename = target_filename;
}

void save_pixbuf_as(GdkPixbuf *pixbuf, char* target_filename){
  size_t filename_length = strlen(target_filename);
  char *extension = strndup(target_filename+filename_length-3,filename_length);  
  
  GError *error = NULL;
  gboolean saved = gdk_pixbuf_save (pixbuf, target_filename, extension, &error,NULL);
  
  if (!saved)
    show_error_message("Ocurrio un error al guardar el archivo. Formatos soportados: BMP y PNG");
}

void save_file(GtkWidget *widget, gpointer data){    
  if (filename != NULL)
    save_canvas_as(filename);
  else
    save_file_as(NULL,NULL);
}

void save_file_as(GtkWidget *widget, gpointer data){
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Guardar archivo como",
					GTK_WINDOW(window),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
    char *new_filename;
    new_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    save_canvas_as(new_filename);
  }
  
  gtk_widget_destroy (dialog);
}

void save_current_selection(GtkWidget *widget, gpointer data){
  if (!selection_is_on){
    show_error_message("No hay nada seleccionado");
    return;
  }  

  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Guardar seleccion como",
					GTK_WINDOW(window),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
    char *new_filename;
    new_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    save_pixbuf_as(selection_pixbuf,new_filename);
  }
  
  gtk_widget_destroy (dialog);
  paint_current_surface_on_canvas(gdk_cairo_create(canvas->window));
}
