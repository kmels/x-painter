#ifndef XPAINTER_TOOLITEM_ID
#define XPAINTER_TOOLITEM_ID

typedef enum{
  XPainter_SELECT_TOOL =  1,
  XPainter_UNDO_TOOL = 2,
  XPainter_SAVE_TOOL = 3,
  XPainter_LOAD_TOOL = 4,
  XPainter_LINE_TOOL,
  XPainter_CIRCLE_TOOL,
  XPainter_ELLIPSE_TOOL,
  XPainter_RECTANGLE_TOOL,
  XPainter_POLYGON_TOOL,
  XPainter_FLOOD_TOOL,
  XPainter_TEXT_TOOL,
  XPainter_ERASER_TOOL,
  XPainter_SPRAY_TOOL,
  XPainter_BRUSH_TOOL,
  XPainter_PEN_TOOL,
  XPainter_ZOOM_TOOL,
  XPainter_MOVE_TOOL
} XPainterToolItemType;
#endif
