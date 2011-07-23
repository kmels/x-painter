#include <gtk/gtk.h>
#include <strings.h>

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
static char* toolbar_item_icons[] = {
  "../icons/icon_select.png",
  "../icons/icon_undo.png",
  "../icons/icon_save.png",
  "../icons/icon_load.png",
  "../icons/icon_line",
  "../icons/icon_circle.png",
  "../icons/icon_ellipse.png",
  "../icons/icon_rectangle.png",
  "../icons/icon_polygon",
  "../icons/icon_flood.png",
  "../icons/icon_text.png",
  "../icons/icon_eraser.png",
  "../icons/icon_spray.png",
  "../icons/icon_brush.png",
  "../icons/icon_pen.png"
};


void get_toolbar(GtkWidget *window, GtkWidget **toolbar){
  GtkToolItem *new_tool_item;
  GtkWidget *new_tool_item_icon;

  *toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(*toolbar), GTK_TOOLBAR_ICONS);
  /* SELECT */
  
  int i;
  int nicons = sizeof toolbar_item_icons / sizeof (*toolbar_item_icons);

  for (i = 0; i < nicons; i++){
    new_tool_item_icon = gtk_image_new_from_file(toolbar_item_icons[i]);
    new_tool_item = gtk_tool_button_new(new_tool_item_icon,NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), new_tool_item, -1);
  }  
}
