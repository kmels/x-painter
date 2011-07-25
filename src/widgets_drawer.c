#include <gtk/gtk.h>
#include <strings.h>
#include "types/xpainter_toolitem.h"
#include "global.h"
//#include "types/xpainter_toolitem_id.h"

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
  { "../icons/icon_undo.gif", "Deshacer", XPainter_UNDO_TOOL},
  { "../icons/icon_redo.gif", "Rehacer", XPainter_SAVE_TOOL},
  { "../icons/icon_line.gif", "Línea", XPainter_LINE_TOOL },
  { "../icons/icon_circle.gif", "Círculo", XPainter_CIRCLE_TOOL },
  { "../icons/icon_ellipse.png", "Elipse", XPainter_ELLIPSE_TOOL },
  { "../icons/icon_rectangle.gif", "Rectangulo", XPainter_RECTANGLE_TOOL },
  { "../icons/icon_polygon.gif", "Polígono", XPainter_POLYGON_TOOL },
  { "../icons/icon_flood.png", "Rellenar", XPainter_FLOOD_TOOL },
  { "../icons/icon_text.png", "Texto", XPainter_TEXT_TOOL },
  { "../icons/icon_eraser.png", "Borrador", XPainter_ERASER_TOOL },
  { "../icons/icon_spray.png", "Spray", XPainter_SPRAY_TOOL },
  { "../icons/icon_brush.png", "Brush", XPainter_BRUSH_TOOL},
  { "../icons/icon_pen.png", "Lapicero", XPainter_PEN_TOOL},
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
    g_signal_connect(new_tool_item, "clicked", G_CALLBACK(assign_current_tool), (gpointer) toolbar_item_icons[i].type);   
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), new_tool_item, -1);
    gtk_widget_set_can_focus ((GtkWidget*) new_tool_item, TRUE);
  }
}
