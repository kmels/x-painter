#ifndef MOUSE_HANDLER
#define MOUSE_HANDLER

enum {
  MOUSE_CLICK,
  MOUSE_DRAG  
} MouseEventType;

gboolean handle_mouse(GtkWidget *widget, void *e, gpointer *t);

#endif
