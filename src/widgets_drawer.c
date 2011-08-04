#include <gtk/gtk.h>
#include <strings.h>
#include "types/xpainter_toolitem.h"
#include "global.h"
#include "mouse_handler.h"
#include "tools.h"
#include <string.h> /* memset */
//#define _GNU_SOURCE
#include <stdio.h>

/* This is the GtkItemFactoryEntry structure used to generate new menus.
   Item 1: The menu path. The letter after the underscore indicates an
           accelerator key once the menu is open.
   Item 2: The accelerator key for the entry
   Item 3: The callback function.
   Item 4: The callback action.  This changes the parameters with
           which the function is called.  The default is 0.
   Item 5: The item type, used to define what kind of an item it is.
           Here are the possible values:
*/

struct history canvas_history;
extern GtkLabel *coordinates_label;

static GtkItemFactoryEntry menu_items[] = {
  { "/Archivo", NULL, NULL, 0, "<Branch>" },
  { "/Archivo/Nuevo", "<control>N", NULL, 0, NULL},
  { "/Archivo/Abrir", "<control>O", NULL, 0, NULL },
  { "/Archivo/Guardar", "<control>S", NULL, 0, NULL },
  { "/Archivo/Guardar como", NULL, NULL, 0, NULL },
  { "/Archivo/separador", NULL, NULL, 0, "<Separator>" },
  { "/Archivo/Salir", "<control>Q", gtk_main_quit, 0, NULL },
  { "/Ayuda", NULL, NULL, 0, "<LastBranch>" },
  { "/Ayuda/Acerca de", NULL, NULL, 0, NULL },
};

void get_main_menu( GtkWidget  *window,
                    GtkWidget **menubar )
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;
  gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);

  accel_group = gtk_accel_group_new ();

  /* This function initializes the item factory.*/

  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

  /* This function generates the menu items. Pass the item factory,
     the number of items in the array, the array itself, and any
     callback data for the the menu items. */
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  if (menubar)
    /* Finally, return the actual menu bar created by the item factory. */ 
    *menubar = gtk_item_factory_get_widget (item_factory, "<main>");
}

//GTK_STOCK_COLOR_PICKER
//GTK_STOCK_CUT
//GTK_STOCK_ZOOM_IN
//GTK_STOCK_ZOOM_OUT
static XPainterToolItem toolbar_item_icons[] = {
  { "../icons/icon_select.png", "Seleccionar", XPainter_SELECT_TOOL },
  { "../icons/icon_move.png", "Mover", XPainter_MOVE_TOOL },
  { "../icons/icon_undo.png", "Deshacer", XPainter_UNDO_TOOL},
  { "../icons/icon_redo.png", "Rehacer", XPainter_REDO_TOOL},
  { "../icons/icon_line.png", "Línea", XPainter_LINE_TOOL },
  { "../icons/icon_circle.png", "Círculo", XPainter_CIRCLE_TOOL },
  { "../icons/icon_ellipse.png", "Elipse", XPainter_ELLIPSE_TOOL },
  { "../icons/icon_rectangle.png", "Rectangulo", XPainter_RECTANGLE_TOOL },
  { "../icons/icon_polygon.png", "Polígono", XPainter_POLYGON_TOOL },
  { "../icons/icon_flood.png", "Rellenar", XPainter_FLOOD_TOOL },
  { "../icons/icon_text.png", "Texto", XPainter_TEXT_TOOL },
  { "../icons/icon_eraser.png", "Borrador", XPainter_ERASER_TOOL },
  { "../icons/icon_spray.png", "Spray", XPainter_SPRAY_TOOL },
  { "../icons/icon_brush.png", "Brush", XPainter_BRUSH_TOOL},
  { "../icons/icon_zoom_ant.png", "Lapicero", XPainter_PEN_TOOL},
  { "../icons/icon_zoom.png", "Zoom", XPainter_ZOOM_TOOL}
};

void get_toolbar(GtkWidget *window, GtkWidget **toolbar){
  GtkToolItem *new_tool_item;
  GtkWidget *new_tool_item_icon;

  *toolbar = gtk_toolbar_new();
  //gtk_toolbar_set_style(GTK_TOOLBAR(*toolbar), GTK_TOOLBAR_BOTH);
  gtk_toolbar_set_style(GTK_TOOLBAR(*toolbar), GTK_TOOLBAR_ICONS);
  
  int i;
  int nicons = sizeof toolbar_item_icons / sizeof (*toolbar_item_icons);
  for (i = 0; i < nicons; i++){  
    new_tool_item_icon = gtk_image_new_from_file(toolbar_item_icons[i].path_to_image);    
    new_tool_item = gtk_tool_button_new(new_tool_item_icon,toolbar_item_icons[i].name);
    
    XPainterToolItemType tool_type = toolbar_item_icons[i].type;
    switch(tool_type){
    case XPainter_UNDO_TOOL:
      g_signal_connect(new_tool_item, "clicked", G_CALLBACK(undo), NULL);   
      break;
    case XPainter_REDO_TOOL:
      g_signal_connect(new_tool_item, "clicked", G_CALLBACK(redo), NULL);   
      break;
    default: 
      g_signal_connect(new_tool_item, "clicked", G_CALLBACK(assign_current_tool), (gpointer) tool_type);
      break;
    }
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), new_tool_item, -1);
    gtk_widget_set_can_focus ((GtkWidget*) new_tool_item, TRUE);
  }
}

gboolean redraw_canvas(GtkWidget *widget, gpointer userdata){
  //is the drawing area initialized?
  if (!canvas_drawn){
    canvas_drawn = TRUE;
    canvas_history.current_index = -1;
    return FALSE;
  }
  else{
    //it has been initialized, now we are either in a redrawing or a new canvas escenario
    cairo_t *cr = gdk_cairo_create(widget->window);
    
    //if (current_surface_index < 0){ //new canvas escenario, paint it white
    if (canvas_history.current_index < 0){ //new canvas escenario, paint it white      
      cairo_set_source_rgb(cr, 255, 255, 255);
      cairo_paint(cr);
      save_current_surface(cairo_get_target(cr));
      save_current_surface_in_history();
    }else{      
      //redraw
      paint_current_surface_on_canvas(cr);
    }
    return TRUE;
  }  
}

gboolean update_coordinates_label(GtkWidget *widget, void *e, gpointer *t){  
  GdkEventMotion *event = (GdkEventMotion*) e;
  char *s = NULL;
  asprintf (&s, "x,y = %f,%f", event->x,event->y);
  gtk_label_set_text(coordinates_label,s);    
  return FALSE; //propagate next events
}
