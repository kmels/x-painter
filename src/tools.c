#include <gtk/gtk.h>
#include "global.h"
//#include <cstdlib>
#include <stdlib.h>

/* paints a single pixel in coordinate x,y*/
inline void put_pixel(cairo_t *cr,double x, double y){
  cairo_move_to(cr,x,y);
  cairo_rectangle(cr,x,y,1,1);
  cairo_fill(cr);
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
/* Saves a surface, tipically called before drawing anything new, so we can simulate drawing on it*/
void save_current_surface(cairo_surface_t *surface){
  cairo_surface_t *surface_to_save = cairo_surface_create_similar(surface, CAIRO_CONTENT_COLOR, 1200, 1200);
  cairo_t *new_cr = cairo_create(surface_to_save);   
  cairo_set_source_surface(new_cr,surface,0,0);
  cairo_paint(new_cr);
  current_surface = surface_to_save;
}

void paint_current_surface_on_canvas(cairo_t *cr){
  cairo_save (cr);
  cairo_set_source_surface(cr,current_surface,0,0);
  cairo_paint (cr);
  cairo_restore (cr);
}

void undo(cairo_t *cr){
  printf("Undoing to surface: #%d\n",current_surface_index-1);

  if (current_surface_index - 1 >= 0){  
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,surfaces_history[--current_surface_index],0,0);
    cairo_paint(cr);
  }else{
    printf("DUH\n");
  }
}

/* Saves a surface in history */
void save_current_surface_in_history(){
  printf("Saving surface: #%d\n",current_surface_index+1);

  surfaces_history[++current_surface_index] = current_surface;
  surfaces_history_size++; 
}
