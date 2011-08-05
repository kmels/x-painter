#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include <stdlib.h>
#include <math.h>

/* paints a single pixel in coordinate x,y*/
inline void put_pixel(cairo_t *cr,double x, double y){
  cairo_move_to(cr,x,y);
  //cairo_rectangle(cr,x,y,1,1);
  cairo_rectangle(cr,x,y,line_width,line_width);
  cairo_fill(cr);
}

/* Line, draws a line from x1,y1 to coordinate x2,y2 using Bresenham's algorithm*/
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2){
  //printf("line: %f,%f -> %f,%f\n",x1,y1,x2,y2);
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


void flood_fill(cairo_t *cr, double x, double y){
  
  cairo_surface_t *surface = cairo_get_target(cr);
  //char* image = cairo_image_surface_get_data(surface);
  
  printf("FORMAT: %d\n",cairo_image_surface_get_format(surface));
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
