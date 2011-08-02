#include <gtk/gtk.h>
#include "global.h"

/* Brush, paints a single pixel in coordinate x,y*/
inline void brush(cairo_t *cr, double x1, double y1, double x2, double y2){
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);	
  cairo_stroke(cr);
  //printf("pattern: %p\n",cairo_get_source(cr));
}

/* Line, draws a line from x1,y1 to coordinate x2,y2*/
inline void line(cairo_t *cr, double x1, double y1, double x2, double y2){
  //printf("%f,%f->%f,%f\n",x1,y1,x2,y2);
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);	
  cairo_stroke(cr);
}

/* Saves a surface, tipically called before drawing anything new, so we can simulate drawing on it*/
void save_current_surface(cairo_surface_t *surface){
  cairo_surface_t *surface_to_save = cairo_surface_create_similar(surface, CAIRO_CONTENT_COLOR, 1200, 1200);
  cairo_t *new_cr = cairo_create(surface_to_save);   
  cairo_set_source_surface(new_cr,surface,0,0);
  cairo_paint(new_cr);
  current_surface = surface_to_save;
}

void undo_current_drawing(cairo_t *cr){
  cairo_save (cr);
  cairo_set_source_surface(cr,current_surface,0,0);
  cairo_paint (cr);
  cairo_restore (cr);
}

void undo(cairo_t *cr){
  printf("Current surface #%d\n",current_surface_index-1);

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
  surfaces_history[++current_surface_index] = current_surface;
  surfaces_history_size++; 
}

