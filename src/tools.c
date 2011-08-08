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

GtkWidget *canvas;

gboolean selection_is_on;
double selection_x1, selection_y1, selection_x2, selection_y2;
GdkPixbuf *selection_pixbuf;
double move_x1, move_y1;
gboolean figure_is_filled;

int line_width;
XPainterColor color1, color2;
guint16 color1_alpha, color2_alpha;

cairo_surface_t *current_surface;
GdkPixbuf *current_surface_pixbuf;
guchar *pixbuf_pixels;

double paste_x1,paste_y1,paste_x2,paste_y2;

//a simple linked list
struct node_struct {
  double x;
  double y;
  struct node_struct *next_node;
};

GdkPixbuf *clipboard_pixbuf;

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

inline void put_pixel_pixbuf_color2(GdkPixbuf *pixbuf,unsigned char *pixels,double x, double y){  
  guchar *p = pixels + ((int)y) * gdk_pixbuf_get_rowstride (pixbuf) + ((int)x) * gdk_pixbuf_get_n_channels(pixbuf);

  p[0] = color2.red;
  p[1] = color2.green;
  p[2] = color2.blue;
  p[3] = color2.alpha;
}

/* paints a single pixel in coordinate x,y*/
inline void put_pixel_width_1(cairo_t *cr,double x, double y){
  cairo_move_to(cr,x,y);
  cairo_rectangle(cr,x,y,1,1);
  cairo_fill(cr);
}

XPainterColor get_pixel_pixbuf(double x,double y,GdkPixbuf *pixbuf,unsigned char *pixels){
  XPainterColor color;
  guchar *p;
  p = pixels + ((int)y) * gdk_pixbuf_get_rowstride (pixbuf) + ((int)x) * gdk_pixbuf_get_n_channels(pixbuf);
  
  color.red = p[0];
  color.green = p[1];
  color.blue = p[2];
  color.alpha = p[3];
  
  return color;
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
  //printf("c1: %d,%d,%d; c2: %d,%d,%d\n",c1.red,c1.green,c1.blue,c2.red,c2.green,c2.blue);
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
  
  cairo_set_source_rgb(mouseState->cr,(double)color1.red / 255, (double)color1.green / 255, (double)color1.blue / 255);
  //line from the last saved coordinate to the current position
  line(mouseState->cr,mouseState->coordinates[ncoordinates-1].x,mouseState->coordinates[ncoordinates-1].y,x,y);
  //line from the current position to the first coordinate
  line(mouseState->cr,x,y,mouseState->coordinates[0].x,mouseState->coordinates[0].y);

  mouseState->ispainting = FALSE;
  return TRUE;
}

void mark_selection(cairo_t *cr, double x1, double y1, double x2, double y2){
  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);

  static const double dashed1[] = {2.0, 1.0};
  static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);

  cairo_set_line_width(cr, 0.5);
  cairo_set_dash(cr, dashed1, len1, 0);

  cairo_move_to(cr, x1-1, y1-1);
  cairo_line_to(cr, x1-1, y2+1);
  cairo_move_to(cr, x1-1, y2+1);
  cairo_line_to(cr, x2+1, y2+1);
  cairo_move_to(cr, x2+1, y2+1);
  cairo_line_to(cr, x2+1, y1-1);
  cairo_move_to(cr, x2+1, y1-1);
  cairo_line_to(cr, x1-1, y1-1);
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
  selection_pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),selection_x1,selection_y1,0,0,selection_x2-selection_x1,selection_y2-selection_y1);
}

/* Tipically called after mouse release when using the MOVE tool */
inline void save_new_selection_after_moving(double x,double y){
  double diff_x = move_x1 - selection_x1;
  double diff_y = move_y1 - selection_y1;

  selection_x1 = x-diff_x;
  selection_y1 = y-diff_y;
  selection_x2 = x+selection_x2-move_x1;
  selection_y2 = y+selection_y2-move_y1; 
}

void move(mouseStateStruct *mouseState, double x, double y){  
  //simulate it's been cut
  cairo_set_source_rgb(mouseState->cr,1,1,1);
  cairo_rectangle(mouseState->cr,selection_x1,selection_y1,selection_x2-selection_x1,selection_y2-selection_y1);
  cairo_fill(mouseState->cr);

  //draw selection  
  double diff_x = move_x1 - selection_x1;
  double diff_y = move_y1 - selection_y1;
  
  mark_selection(mouseState->cr,x-diff_x,y-diff_y,x+selection_x2-move_x1,y+selection_y2-move_y1);
  
  gdk_cairo_set_source_pixbuf(mouseState->cr,selection_pixbuf,x-diff_x,y-diff_y);
  cairo_paint(mouseState->cr);
}

/* draws the move, but without the selection */
void move_finally(mouseStateStruct *mouseState, double x, double y){
  //simulate it's been cut
  cairo_set_source_rgb(mouseState->cr,1,1,1);
  cairo_rectangle(mouseState->cr,selection_x1,selection_y1,selection_x2-selection_x1,selection_y2-selection_y1);
  cairo_fill(mouseState->cr);

  //draw selection  
  double diff_x = move_x1 - selection_x1;
  double diff_y = move_y1 - selection_y1;
  
  gdk_cairo_set_source_pixbuf(mouseState->cr,selection_pixbuf,x-diff_x,y-diff_y);
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
  show_error_message("No hay nada seleccionado para mover");
  
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

inline gboolean is_within_rectangle_bounds(double x,double y, double rect_x1,double rect_y1, double rect_x2, double rect_y2){   
  return (is_within_bounds(x,y)) && (x > rect_x1) && (x < rect_x2) && (y > rect_y1) && (y < rect_y2);
}

void fill_rectangle(cairo_t *cr, double x1, double y1, double x2, double y2){
  double xi = x1;
  double yi = y1;  
  double xf = x2;
  double yf = y2;
  
  if (x2 < x1){
    xi = x2;
    xf = x1;
  }
  if (y2 < y1){
    yi = y2;
    yi = y1;
  }

  GdkPixbuf *rectangle_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,canvas->allocation.width,canvas->allocation.height);

  unsigned char *pixels = gdk_pixbuf_get_pixels(rectangle_pixbuf);  
  double mid_x = round(xi+(xf-xi)/2); //we need to get rid of decimals!
  double mid_y = round(yi+(yf-yi)/2);

  typedef struct node_struct node;
  
  // Creating the list/stack
  node *list = (node*) malloc(sizeof(struct node_struct));
  (*list).x = mid_x;
  (*list).y = mid_y;
  (*list).next_node = NULL;
  
  // The algorithm 
  while (list != NULL) {
    node *pointer_to_free = list;
    node current_node = *list;
    list = current_node.next_node;
    
    put_pixel_pixbuf_color2(rectangle_pixbuf,pixels,current_node.x,current_node.y);
    //check neighbours
    node *new_node;    
    // Up
    if (is_within_rectangle_bounds(current_node.x,current_node.y-1,x1,y1,x2,y2) && !colors_are_equal(get_pixel_pixbuf(current_node.x,current_node.y-1,rectangle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y-1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    // Down
    if (is_within_rectangle_bounds(current_node.x,current_node.y+1,x1,y1,x2,y2) && !colors_are_equal(get_pixel_pixbuf(current_node.x,current_node.y+1,rectangle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y+1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    //Left
    if (is_within_rectangle_bounds(current_node.x-1,current_node.y,x1,y1,x2,y2) && !colors_are_equal(get_pixel_pixbuf(current_node.x-1,current_node.y,rectangle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x-1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }

    //Right
    if (is_within_rectangle_bounds(current_node.x+1,current_node.y,x1,y1,x2,y2) && !colors_are_equal(get_pixel_pixbuf(current_node.x+1,current_node.y,rectangle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x+1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    free(pointer_to_free);
  }
  
  //GError          *error = NULL;
  //gboolean t = gdk_pixbuf_save (rectangle_pixbuf, "rectangle.png", "png", &error,NULL);
  gdk_cairo_set_source_pixbuf(cr,rectangle_pixbuf,0,0);  
  cairo_paint(cr);
}

inline gboolean is_within_circle_bounds(double x,double y, double cen_x,double cen_y, double radius){    
  return (is_within_bounds(x,y)) && (pow(x-cen_x,2) + pow(y-cen_y,2) < (radius*radius));
}

/* Fills circle */
void fill_circle(cairo_t *cr, double cen_x, double cen_y, double current_x, double current_y){
  double radius = sqrt(pow((current_x-cen_x),2) + pow((current_y-cen_y),2));

  GdkPixbuf *circle_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,canvas->allocation.width,canvas->allocation.height);

  unsigned char *pixels = gdk_pixbuf_get_pixels(circle_pixbuf);  
  double mid_x = cen_x; 
  double mid_y = cen_y;

  typedef struct node_struct node;
  
  // Creating the list/stack
  node *list = (node*) malloc(sizeof(struct node_struct));
  (*list).x = mid_x;
  (*list).y = mid_y;
  (*list).next_node = NULL;
  
  // The algorithm 
  while (list != NULL) {
    node *pointer_to_free = list;
    node current_node = *list;
    list = current_node.next_node;
    
    put_pixel_pixbuf_color2(circle_pixbuf,pixels,current_node.x,current_node.y);
    //check neighbours
    node *new_node;    
    // Up
    if (is_within_circle_bounds(current_node.x,current_node.y-1,cen_x,cen_y,radius) && !colors_are_equal(get_pixel_pixbuf(current_node.x,current_node.y-1,circle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y-1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    // Down
    if (is_within_circle_bounds(current_node.x,current_node.y+1,cen_x,cen_y,radius) && !colors_are_equal(get_pixel_pixbuf(current_node.x,current_node.y+1,circle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x;
      (*new_node).y = current_node.y+1;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    //Left
    if (is_within_circle_bounds(current_node.x-1,current_node.y,cen_x,cen_y,radius) && !colors_are_equal(get_pixel_pixbuf(current_node.x-1,current_node.y,circle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x-1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }

    //Right
    if (is_within_circle_bounds(current_node.x+1,current_node.y,cen_x,cen_y,radius) && !colors_are_equal(get_pixel_pixbuf(current_node.x+1,current_node.y,circle_pixbuf,pixels),color2)) {
      new_node = (node*) malloc(sizeof(struct node_struct));
      (*new_node).x = current_node.x+1;
      (*new_node).y = current_node.y;
      (*new_node).next_node = list;
      list = new_node;
    }
    
    free(pointer_to_free);
  }
  
  gdk_cairo_set_source_pixbuf(cr,circle_pixbuf,0,0);  
  cairo_paint(cr);
}

inline void save_paste_positions(){
  paste_x1 = selection_x1;
  paste_y1 = selection_y1;
}

/* Cut selection */
void cut(GtkWidget *widget, gpointer data){//widget and data are useless
  if (!selection_is_on)
    show_error_message("No hay nada seleccionado para cortar");
  
  //get area
  clipboard_pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),selection_x1,selection_y1,0,0,selection_x2-selection_x1,selection_y2-selection_y1);  
  cairo_t *cr = gdk_cairo_create(canvas->window);      
  //erase selection
  paint_current_surface_on_canvas(cr);
  
  //draw white rectangle in selection area
  cairo_set_source_rgb(cr,1,1,1);
  cairo_rectangle(cr,selection_x1,selection_y1,selection_x2-selection_x1,selection_y2-selection_y1);
  cairo_fill(cr);
  
  save_paste_positions();
  save_current_surface(cairo_get_target(cr));
  save_current_surface_in_history();
}

void paste(GtkWidget *widget, gpointer data){
  cairo_t *cr = gdk_cairo_create(canvas->window);  
  //save_current_surface(cairo_get_target(cr));
  
  // GError          *error = NULL;
  //gdk_pixbuf_save (current_surface_pixbuf, "current.png", "png", &error,NULL);

  double clipboard_width = (double) gdk_pixbuf_get_width(clipboard_pixbuf);
  double clipboard_height = (double) gdk_pixbuf_get_height(clipboard_pixbuf);  

  selection_x1 = paste_x1;
  selection_y1 = paste_y1;
  selection_x2 = paste_x1+clipboard_width;
  selection_y2 = paste_y1+clipboard_height;
  
  gdk_cairo_set_source_pixbuf(cr,clipboard_pixbuf,paste_x1,paste_y1);
  cairo_paint(cr);
  current_tool = XPainter_MOVE_TOOL;
  
  mark_selection(cr,selection_x1,selection_y1,selection_x2,selection_y2);
  save_selection(selection_x1,selection_y1,selection_x2,selection_y2);
}

/* Copy selection */
void copy(GtkWidget *widget, gpointer data){//widget and data are useless
  if (!selection_is_on)
    show_error_message("No hay nada seleccionado para copiar");
  
  save_paste_positions();
  clipboard_pixbuf = gdk_pixbuf_get_from_drawable(NULL,GDK_DRAWABLE(canvas->window),gdk_colormap_get_system(),selection_x1,selection_y1,0,0,selection_x2-selection_x1,selection_y2-selection_y1);
}

gboolean should_save_surface_at_click(int tool){
  if (tool==XPainter_SELECT_TOOL || tool == XPainter_MOVE_TOOL)
    return FALSE;
  
  return TRUE;
}
