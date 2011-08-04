#include "types/coordinate.h"
#ifndef MOUSE_HANDLER
#define MOUSE_HANDLER

enum {
  MOUSE_CLICK,
  MOUSE_DRAG  
} MouseEventType;

typedef struct{
  gboolean ispainting;
  cairo_t *cr;
  Coordinate coordinates[5000];
  int coordinates_size;
  gboolean save_dragging;
  gboolean should_save_change;
} mouseStateStruct;

//extern struct mouseStateStruct mouseState;

gboolean handle_mouse(GtkWidget *widget, void *e, gpointer *t);

#endif
