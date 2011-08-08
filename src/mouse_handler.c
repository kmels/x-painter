#include <gtk/gtk.h>
#include "global.h"
#include "mouse_handler.h"
#include "types/xpainter_toolitem_id.h"
#include "tools.h"
#include <string.h> /* memset */

mouseStateStruct mouseState;

void save_coordinates(double x,double y){
  //printf("saving coordinates[%d] = %f,%f\n",mouseState.coordinates_size,x,y);
  mouseState.coordinates[mouseState.coordinates_size].x = x;
  mouseState.coordinates[mouseState.coordinates_size].y = y;
  mouseState.coordinates_size++;
}

gboolean handle_mouse(GtkWidget *widget, void *e, gpointer *t){  
  gint type = GPOINTER_TO_INT(t);
    
  switch(type){
  case MOUSE_CLICK:{	
    GdkEventButton *event = (GdkEventButton*) e;
    switch(event->type) {
    case GDK_BUTTON_PRESS: {      
      mouseState.ispainting = TRUE;
      mouseState.cr = gdk_cairo_create(widget->window);
      gboolean remove_selection = TRUE; //should we remove selection after clicking with the selected tool?
      
      if (selection_is_on)
	paint_current_surface_on_canvas(mouseState.cr); //delete possible selection
      
      if (event->button==3) { //right click 
	//might be finishing polygon
	finish_polygon(&mouseState,event->x,event->y);
      }else if (event->button==1){ //left click	
	save_current_surface(cairo_get_target(mouseState.cr));
	cairo_set_source_rgb(mouseState.cr,(double)color1.red / 255, (double)color1.green / 255, (double)color1.blue / 255);
	save_coordinates(event->x,event->y);
      }
      
      switch(current_tool){
      case XPainter_MOVE_TOOL: remove_selection = !click_is_within_selection(event->x,event->y); break;
      case XPainter_ERASER_TOOL: single_eraser(mouseState.cr, event->x, event->y); break;
      case XPainter_SPRAY_TOOL: spray(event->x, event->y,&mouseState); break;
      default: break;
      }
      
      if (remove_selection)
	selection_is_on = FALSE;
    }
      break;
    case GDK_BUTTON_RELEASE: {
      mouseState.ispainting = FALSE;
      mouseState.should_save_change = TRUE;
      
      switch(current_tool){
      case XPainter_SELECT_TOOL:{
	mouseState.should_save_change = FALSE;
	mark_selection(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);
	save_selection(mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);
      }break;
      case XPainter_MOVE_TOOL:{
	selection_is_on = FALSE;
      }break;	
      case XPainter_LINE_TOOL: line(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y); break;
      case XPainter_RECTANGLE_TOOL: {
	rectangle(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y);
	if (figure_is_filled){
	  save_current_surface(cairo_get_target(mouseState.cr));
	  fill_rectangle(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y);
	}
      }break;
      case XPainter_POLYGON_TOOL: {
	//we might just finished drawing a segment of the polygon, or finished drawing
	if (event->button!=3) //we are drawing a segment (not finishing yet)
	  mouseState.ispainting = TRUE;
	
	line(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x,event->y);

	if (mouseState.coordinates[mouseState.coordinates_size-1].x!=event->x || mouseState.coordinates[mouseState.coordinates_size-1].y!=event->y)
	  save_coordinates(event->x,event->y);
      }break;
      case XPainter_FLOOD_TOOL: flood_fill(mouseState.cr,mouseState.coordinates[0].x, mouseState.coordinates[0].y); break;	
      default : break;
      }
      
      if (mouseState.should_save_change){
	save_current_surface(cairo_get_target(mouseState.cr));
	save_current_surface_in_history();
      }
      
      if (!mouseState.ispainting){
	cairo_destroy(mouseState.cr);
	memset(&mouseState, 0, sizeof(mouseState));	  
      }
    }
      break;
    default:
      break;
    }
  } //end case MOUSE_CLICK
    break;
  case MOUSE_DRAG:{
    GdkEventMotion *event = (GdkEventMotion*) e;
    static	GdkWindow *gdkWindow;
    
    if (!gdkWindow){	// initialize
      gdkWindow = gdk_get_default_root_window();
      memset(&mouseState, 0, sizeof(mouseState));
      return TRUE;
    }
      
    mouseState.save_dragging = TRUE;

    if (!mouseState.ispainting) //are we doing painting?
      return TRUE;
      
    switch(current_tool){
    case XPainter_SELECT_TOOL: {
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);
      mark_selection(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);
    }break;
    case XPainter_MOVE_TOOL: {
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);
      move(&mouseState,event->x,event->y);
    } break;
    case XPainter_BRUSH_TOOL: brush(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x, event->y); break; 
    case XPainter_ERASER_TOOL: dragged_eraser(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x, event->y); break; 
    case XPainter_SPRAY_TOOL: spray(event->x, event->y,&mouseState); break; 
    case XPainter_LINE_TOOL: {
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);
      line(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);
    } break;
    case XPainter_POLYGON_TOOL:{
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);	
      line(mouseState.cr, mouseState.coordinates[mouseState.coordinates_size-1].x, mouseState.coordinates[mouseState.coordinates_size-1].y, event->x,event->y);
    } break;
    case XPainter_CIRCLE_TOOL:{
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);	
      circle(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y);
    } break;
    case XPainter_ELLIPSE_TOOL:{
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);	
      ellipse(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x,event->y);
    } break;
    case XPainter_RECTANGLE_TOOL: {	
      mouseState.save_dragging = FALSE;
      paint_current_surface_on_canvas(mouseState.cr);	
      rectangle(mouseState.cr, mouseState.coordinates[0].x, mouseState.coordinates[0].y, event->x, event->y);      
    } break;
    default: break;
    }
      
    if (mouseState.save_dragging)
      save_coordinates(event->x,event->y);      

    // even though we don't use the resulting information from this call, 
    // calling it is an indication to the main_loop() 
    // that we are ready to receive the next mouse motion notify event
    gint x, y;
    GdkModifierType state;
    gdk_window_get_pointer(gdkWindow, &x, &y, &state);      
  }
    break;
  default:
    break;
  }

  return TRUE;
}
