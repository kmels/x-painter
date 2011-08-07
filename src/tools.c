#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
//#include <gdk-pixbuf/gdk-pixbuf-private.h>
//#include "gdk-pixbuf-xlib-private.h"
#include <X11/Xutil.h>
//#include <gdk-pixbuf/gdk-pixbuf.h>
//#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
//#include <gdk/gdkx.h>
//#include <gtk/gtk.h>

/* paints a single pixel in coordinate x,y*/
inline void put_pixel(cairo_t *cr,double x, double y){
  cairo_move_to(cr,x,y);
  //cairo_rectangle(cr,x,y,1,1);
  cairo_rectangle(cr,x,y,line_width,line_width);
  cairo_fill(cr);
}

inline void put_pixel_pixbuf(GdkPixbuf *pixbuf,double x, double y){
  guchar *p = pixbuf_pixels + ((int)y) * gdk_pixbuf_get_rowstride (current_surface_pixbuf) + ((int)x) * gdk_pixbuf_get_n_channels(current_surface_pixbuf);

  p[0] = color1.red;
  p[1] = color1.green;
  p[2] = color1.blue;
}

/* paints a single pixel in coordinate x,y*/
inline void put_pixel_width_1(cairo_t *cr,double x, double y){
  cairo_move_to(cr,x,y);
  cairo_rectangle(cr,x,y,1,1);
  cairo_fill(cr);
}

XPainterColor get_pixel(double x,double y){
  XPainterColor color;
  guchar *p;
  p = pixbuf_pixels + ((int)y) * gdk_pixbuf_get_rowstride (current_surface_pixbuf) + ((int)x) * gdk_pixbuf_get_n_channels(current_surface_pixbuf);
  
  color.red = p[0];
  color.green = p[1];
  color.blue = p[2];
  color.alpha = p[3];
  
  return color;
}

/* Line, draws a line from x1,y1 to coordinate x2,y2 using Bresenham's algorithm*/
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2){
  gboolean steep = FALSE;
  double tmp;
  if (abs(y2-y1) > abs(x2 - x1)){
    steep = TRUE;
    //swap x1, y1
    tmp = y1;
    y1 = x1;
    x1 = tmp;
    //swap x2,y2
    tmp = y2;
    y2 = x2;
    x2 = tmp;
  }
  
  if (x1 > x2){
    //swap x1, x2
    tmp = x2;
    x2 = x1;
    x1 = tmp;
    //swap y1,y2
    tmp = y2;
    y2 = y1;
    y1 = tmp;
  }

  int dx = x2 - x1;
  int dy = abs(y2 - y1);
   
  double discriminant = 2*dy - 2*dx;
  double next_y = y1;

  int ystep = 1;
  if (y1 > y2)
    ystep = -1;

  int x;
  for(x = x1; x <= x2 ; x ++){
    if (steep)
      put_pixel(cr,next_y,x);
    else
      put_pixel(cr,x,next_y);
    
    if (discriminant <= 0)
      discriminant = discriminant + 2*dy;
    else{
      next_y = next_y + ystep;
      discriminant = discriminant + 2*dy - 2*dx;
    }
  }
}

/* Brush, paints a single pixel in coordinate x,y*/
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2){
  line(cr,x1,y1,x2,y2);
}

void rectangle(cairo_t *cr, double x1, double y1, double x2, double y2){
  line(cr,x1,y1,x2,y1);
  line(cr,x2,y1,x2,y2);
  line(cr,x2,y2,x1,y2);
  line(cr,x1,y2,x1,y1);
}

/* */
void spray(double x, double y,mouseStateStruct *mouseState){
  int radius = 25*line_width;
  int radiusSquared = pow(radius,2);

  int pixel_i;
  double random_x;
  double random_y;
  int sign_x;
  int sign_y;

  srand (time(NULL));
  
  //while (mouseState->ispainting){
    for(pixel_i = 0; pixel_i < 100*line_width; pixel_i++){
      random_x = rand() % radius;
      random_y = rand() % radius;     
      
      //is (random_x,random_y) inside the circle?    
      if ((pow(random_x,2) + pow(random_y,2)) <= radiusSquared){ //it is
	if ((rand() % 1)<1)
	  sign_x = -1;
	else
	  sign_x = 1;
	
	if ((rand() %1)<1)
	  sign_y = -1;
	else
	  sign_y = 1;
      }
      
      put_pixel_width_1(mouseState->cr,x+random_x*sign_x,y+random_y*sign_y);
    }   
}

inline gboolean colors_are_equal(XPainterColor c1, XPainterColor c2){
  return (c1.red == c2.red) && (c1.green == c2.green) && (c1.blue == c2.blue);// && (c1.alpha == c2.alpha);
}

inline gboolean is_within_bounds(double x, double y){
  return (x >= 0) && (x <= canvas->allocation.width) && (y >= 0) && (y <= canvas->allocation.height);
}

void flood_fill(cairo_t *cr,double x, double y){ 
  // the destination color  
  XPainterColor replacement_color = color1;
  
  // the color we want to replace
  XPainterColor target_color = get_pixel(x,y);
  
  if (colors_are_equal(replacement_color,target_color))
    return;
  
  //a simple linked list
  struct node_struct {
    double x;
    double y;
    struct node_struct *next_node;
  };
  
  typedef struct node_struct node;
  
  // Creating the list/stack
  node *list = (node*) malloc(sizeof(struct node_struct));
  (*list).x = x;
  (*list).y = y;
  (*list).next_node = NULL;
  
  // The algorithm 
  while (list != NULL) {
    node *pointer_to_free = list;
    node current_node = *list;
    list = current_node.next_node;
    
    put_pixel_pixbuf(current_surface_pixbuf,current_node.x,current_node.y);

    //check neighbours
    node *new_node;    
    // Up
    if (is_within_bounds(current_node.x,current_node.y-1) && colors_are_equal(get_pixel(current_node.x,current_node.y-1),target_color)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y-1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    // Down
    if (is_within_bounds(current_node.x,current_node.y+1) && colors_are_equal(get_pixel(current_node.x,current_node.y+1),target_color)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y+1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    //Left
    if (is_within_bounds(current_node.x-1,current_node.y) && colors_are_equal(get_pixel(current_node.x-1,current_node.y),target_color)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x-1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }

    //Right
    if (is_within_bounds(current_node.x+1,current_node.y) && colors_are_equal(get_pixel(current_node.x+1,current_node.y),target_color)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x+1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }        
    
    free(pointer_to_free);
  }
  
  gdk_cairo_set_source_pixbuf(cr,current_surface_pixbuf,0,0);
  cairo_paint(cr);
}

/* returns true if x,y belongs to the line drawn from x1,y1 to x2,y2 */
gboolean coordinate_belongs_to_line(double x,double y,double x1,double y1,double x2,double y2){
//gboolean coordinate_belongs_to_line(x,y,x1,y1,x2,y2){
  double dy = y2 - y1;
  double dx = x2 - x1;
  return dy*(x-x1) - dx*y + dx*y1 == 0 ? TRUE : FALSE;
}

/* draws a line from the current coordinate of the mouse to the first one, in case the polygon tool is the current 
   returns TRUE iff it the polygon is finally drawn.
   x,y represent the current position
*/
gboolean finish_polygon(mouseStateStruct *mouseState,double x,double y){
  if (current_tool != XPainter_POLYGON_TOOL || mouseState->coordinates_size < 2) //if current tool != polygon or one only one line has been drawn.
    return FALSE;
  
  int ncoordinates = mouseState->coordinates_size;
  
  //line from the last saved coordinate to the current position
  line(mouseState->cr,mouseState->coordinates[ncoordinates-1].x,mouseState->coordinates[ncoordinates-1].y,x,y);
  //line from the current position to the first coordinate
  line(mouseState->cr,x,y,mouseState->coordinates[0].x,mouseState->coordinates[0].y);

  mouseState->ispainting = FALSE;
  return TRUE;
}

/* Draws a circle using the midpoint algorithm */
gboolean circle(cairo_t *cr, double x0, double y0, double xf, double yf){
  double radius = sqrt(pow((x0-xf),2) + pow((yf-y0),2));
  double f = 1 - radius;
  double ddF_x = 1;
  double ddF_y = -2 * radius;
  double x = 0;
  double y = radius;
 
  put_pixel(cr,x0, y0 + radius);
  put_pixel(cr,x0, y0 - radius);
  put_pixel(cr,x0 + radius, y0);
  put_pixel(cr,x0 - radius, y0);
 
  while(x < y){
    if(f >= 0){
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    put_pixel(cr,x0 + x, y0 + y);
    put_pixel(cr,x0 - x, y0 + y);
    put_pixel(cr,x0 + x, y0 - y);
    put_pixel(cr,x0 - x, y0 - y);
    put_pixel(cr,x0 + y, y0 + x);
    put_pixel(cr,x0 - y, y0 + x);
    put_pixel(cr,x0 + y, y0 - x);
    put_pixel(cr,x0 - y, y0 - x);
  }
  return TRUE;
}

void mark_selection(cairo_t *cr, double x1, double y1, double x2, double y2){
  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);

  static const double dashed1[] = {4.0, 1.0};
  static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);

  cairo_set_line_width(cr, 1.0);
  cairo_set_dash(cr, dashed1, len1, 0);

  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x1, y2);
  cairo_move_to(cr, x1, y2);
  cairo_line_to(cr, x2, y2);
  cairo_move_to(cr, x2, y2);
  cairo_line_to(cr, x2, y1);
  cairo_move_to(cr, x2, y1);
  cairo_line_to(cr, x1, y1);
  cairo_stroke(cr);
}

/* Ensures that selection_x1 < selection_x2 && selection_y1 < selection_y2 */
void save_selection(double x1, double y1, double x2, double y2){
  if (x1 < x2){
    selection_x1 = x1;
    selection_x2 = x2;
  }else{
    selection_x1 = x2;
    selection_x2 = x1;
  }

  if (y1 < y2){
    selection_y1 = y1;
    selection_y2 = y2;
  }else{
    selection_y1 = y2;
    selection_y2 = y1;
  }
  
  selection_is_on = TRUE;
}

void move(mouseStateStruct *mouseState, double x, double y){
  //copy data
  GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),selection_x1,selection_y1,0,0,selection_x2-selection_x1,selection_y2-selection_y1);
  
  //simulate it's been cut
  cairo_set_source_rgb(mouseState->cr,1,1,1);
  cairo_rectangle(mouseState->cr,selection_x1,selection_y1,selection_x2-selection_x1,selection_y2-selection_y1);
  cairo_fill(mouseState->cr);

  //draw selection  
  double diff_x = move_x1 - selection_x1;
  double diff_y = move_y1 - selection_y1;

  mark_selection(mouseState->cr,x-diff_x,y-diff_y,x+selection_x2-move_x1,y+selection_y2-move_y1);

  gdk_cairo_set_source_pixbuf(mouseState->cr,pixbuf,x-diff_x,y-diff_y);
  cairo_paint(mouseState->cr);  
}

/* Returns true iff given coordinate is within the posible selection */
gboolean click_is_within_selection(double x, double y){
  if (!selection_is_on)
    return FALSE;
  
  if ((x > selection_x1) && (x < selection_x2) && (y > selection_y1) && (y < selection_y2)){
    move_x1 = x;
    move_y1 = y;
    return TRUE;
  }
  
  return FALSE;
}

inline void single_eraser(cairo_t *cr,double x, double y){
  cairo_set_source_rgb(cr,1,1,1);  
  cairo_rectangle(cr,x,y,line_width,line_width);
  cairo_fill(cr);
}

inline void dragged_eraser(cairo_t *cr,double x, double y,double x2, double y2){
  cairo_set_source_rgb(cr,1,1,1);
  line(cr,x,y,x2,y2);
}


inline void put_ellipse_pixels(cairo_t *cr,double cx, double cy,double x,double y){
  put_pixel(cr,cx+x,cy+y);
  put_pixel(cr,cx-x,cy+y);
  put_pixel(cr,cx-x,cy-y);
  put_pixel(cr,cx+x,cy-y);  
}

/* John Kennedy */
void ellipse(cairo_t *cr, double cenx, double ceny, double current_x, double current_y){
  double a = abs(current_x - cenx);
  double b = abs(current_y - ceny);
  double Pk,x,y;

  Pk=b*b-b*a*a+0.25*a*a; x=0; y=b;
  
  put_ellipse_pixels(cr,cenx,ceny,x,y);
  while (2*x*b*b <= 2*y*a*a){
    if (Pk<0){
      x++;
      Pk=Pk+2*x*b*b+3*b*b;
    }
    else {
      x++; y--;
      Pk=Pk+2*x*b*b+3*b*b-2*y*a*a+2*a*a;
    }
    put_ellipse_pixels(cr,cenx,ceny,x,y);    
  }

  Pk=(x+0.5)*(x+0.5)*b*b+(y-1)*(y-1)*a*a-a*a*b*b;
  put_ellipse_pixels(cr,cenx,ceny,x,y);
  while (y>0){
    if (Pk>0){
      y--;
      Pk=Pk-2*y*a*a+3*a*a;
    }
    else{
      x++;
      y--;
      Pk=Pk-2*y*a*a+3*a*a+2*x*b*b+2*b*b;
    }
    put_ellipse_pixels(cr,cenx,ceny,x,y);
  }
}
