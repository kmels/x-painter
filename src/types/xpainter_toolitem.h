#ifndef XPAINTER_TOOLITEM
#define XPAINTER_TOOLITEM

#include "xpainter_toolitem_id.h"

typedef struct{
  char *path_to_image;
  char *name;
  XPainterToolItemType type;  
} XPainterToolItem;
#endif
