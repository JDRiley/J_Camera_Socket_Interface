#include "J_UI_Model.h"
#include "../J_Error/J_Error.h"
#include "J_FT_Text_Displayer/J_FT_Text_Display.h"
#include "J_FT_Text_Displayer/J_Text_Box.h"
#include "J_View.h"
//
#include <J_Utile.h>
//
#include "J_Image_Pane.h"


//Algorithm
#include <algorithm>
#include <functional>

using std::bind; using std::for_each; using std::mem_fn; using std::transform;
using namespace std::placeholders;

//Utilities
#include <cassert>


//
#include <thread>

//
#include <future>
using std::future;
using std::async;

#ifdef VS_2013
using std::launch;
#else

namespace launch = std::launch;
#endif // VS_2013



#include <iostream>
using std::cerr; using std::endl;

const int MATH_FONT_FACE_SIZE = 36;
const int INPUT_FONT_FACE_SIZE = 18;
const int LOG_FONT_FACE_SIZE = 20;
const int ERROR_FONT_FACE_SIZE = 20;
namespace jomike{

static Instance_Pointer<J_UI_Model> s_ui_data;

static FT_Face create_face(FT_Library, const char* pathname, int level);

J_UI_Model::J_UI_Model(){
	
}



//J_Context_Shared_t share_context(J_Context_Shared_t);

void J_UI_Model::update(){
	for_each(make_pair_iter(M_ui_objects.begin())
		,make_pair_iter(M_ui_objects.end())
		, mem_fn(&J_UI_Object::update));
	
}

void J_UI_Model::attach_view(J_View_Shared_t i_view_ptr){
	bool inserted = M_views.insert(i_view_ptr).second;

	if(!inserted){
		throw J_Argument_Error("View Already A Member of J_UI_Model");
	}
}

void J_UI_Model::add_ui_circle(J_UI_Circle_Shared_t i_circle_ptr){
	add_ui_object(i_circle_ptr);

	assert(!M_ui_circles.count(i_circle_ptr->get_ID()));
	M_ui_circles[i_circle_ptr->get_ID()] = i_circle_ptr;
}


void J_UI_Model::add_text_box(J_Text_Box_Shared_t i_text_box_ptr){
	add_box(i_text_box_ptr);

	assert(!M_text_boxes.count(i_text_box_ptr->get_ID()));
	M_text_boxes[i_text_box_ptr->get_ID()] = i_text_box_ptr;
}

void J_UI_Model::add_image_pane(J_Image_Pane_Shared_t i_image_pane_ptr){
	add_box(i_image_pane_ptr);

	assert(!M_image_panes.count(i_image_pane_ptr->get_ID()));
	M_image_panes[i_image_pane_ptr->get_ID()] = i_image_pane_ptr;

}


void J_UI_Model::add_ui_object(J_UI_Object_Shared_t i_obj_ptr){
	assert(!M_ui_objects.count(i_obj_ptr->get_ID()));
	M_ui_objects[i_obj_ptr->get_ID()] = i_obj_ptr;
	i_obj_ptr->broadcast_current_state();
}

void J_UI_Model::add_box(J_UI_Box_Shared_t i_display_box_ptr){
	add_ui_object(i_display_box_ptr);

	assert(!M_ui_boxes.count(i_display_box_ptr->get_ID()));
	M_ui_boxes[i_display_box_ptr->get_ID()] = i_display_box_ptr;
}



J_Text_Box_Shared_t J_UI_Model::get_text_box(j_uint i_id){
	if(!M_text_boxes.count(i_id)){
		assert(0);
	}

	return M_text_boxes[i_id];
}

J_UI_Box_Shared_t J_UI_Model::get_ui_box(j_uint i_id){
	if (!M_ui_boxes.count(i_id)){
		assert(0);
	}

	return M_ui_boxes[i_id];
}


J_UI_Object_Shared_t J_UI_Model::get_ui_object(j_uint i_id){
	if(!M_ui_objects.count(i_id)){
		assert(0);
	}

	return M_ui_objects[i_id];
}

J_UI_Circle_Shared_t J_UI_Model::get_ui_circle(j_uint i_id){
	if (!M_ui_circles.count(i_id)){
		assert(0);
	}
	return M_ui_circles[i_id];
}


void J_UI_Model::notify_text_string(j_uint i_text_box_id, const J_UI_Multi_String& irk_string){
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_text_string, _1, i_text_box_id, irk_string));
}

void J_UI_Model::notify_text_cursor(j_uint i_text_box_id, j_size_t i_cursor_pos){
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_text_cursor, _1, i_text_box_id, i_cursor_pos));
}

void J_UI_Model::notify_text_cursor_color(j_uint i_text_box_id, J_Color_RGBA<j_float> i_color){
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_text_cursor_color, _1, i_text_box_id, i_color));

}


void J_UI_Model::notify_char_delete(j_uint i_text_box_id, j_size_t i_pos){
	assert(M_text_boxes.count(i_text_box_id));

	for_each(M_views.begin(), M_views.end(), bind(&J_View::delete_char, _1, i_text_box_id, i_pos));
}

void J_UI_Model::notify_erase_chars(j_uint i_text_box_id, j_size_t i_pos, j_size_t i_size){
	assert(M_text_boxes.count(i_text_box_id));

	for_each(M_views.begin(), M_views.end(), bind(&J_View::erase_chars, _1, i_text_box_id, i_pos, i_size));
}

void J_UI_Model::notify_char_add(j_uint i_text_box_id, j_size_t i_pos, J_UI_Char i_char){
	assert(M_text_boxes.count(i_text_box_id));

	for_each(M_views.begin(), M_views.end(), bind(&J_View::insert_char, _1, i_text_box_id, i_pos, i_char));
}

void J_UI_Model::notify_fill_visibility_status(j_uint i_disp_obj_id, bool i_status){
	assert(M_ui_objects.count(i_disp_obj_id));
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_fill_visibility, _1, i_disp_obj_id, i_status));
}

void J_UI_Model::notify_outline_visibility_status(j_uint i_disp_obj_id, bool i_status){
	assert(M_ui_objects.count(i_disp_obj_id));
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_outline_visibility, _1, i_disp_obj_id, i_status));
}

void J_UI_Model::notify_box_coordinates(j_uint i_obj_id, const J_Rectangle& i_rectangle)
{
	M_views.apply(bind(&J_View::update_box_coordinates,_1, i_obj_id, i_rectangle));
}
void J_UI_Model::notify_fill_color(j_uint i_obj_id, const J_UI_Color& i_color)
{
	M_views.apply(bind(&J_View::update_fill_color, _1, i_obj_id, i_color));
}
void J_UI_Model::notify_outline_color(j_uint i_obj_id
										  , const J_Color_RGBA<j_float> i_color){
	M_views.apply(bind(&J_View::update_outline_color, _1, i_obj_id, i_color));
}

void J_UI_Model::notify_circle_shape_data(j_uint i_obj_id, const J_Circle& irk_circle){
	M_views.apply(bind(&J_View::update_circle_shape_data, _1, i_obj_id, irk_circle));
}

void J_UI_Model::notify_center(j_uint i_obj_id, j_float i_x_pos, j_float i_y_pos){
	M_views.apply(bind(&J_View::update_center, _1, i_obj_id, i_x_pos, i_y_pos));
}

void J_UI_Model::notify_radius(j_uint i_obj_id, j_float i_radius){
	M_views.apply(bind(&J_View::update_radius, _1, i_obj_id, i_radius));
}

void J_UI_Model::notify_image_clear(j_uint i_obj_id){
	M_views.apply(bind(&J_View::update_image_clear, _1, i_obj_id));
}

void J_UI_Model::notify_new_image_buffer(j_uint i_obj_id, const j_ubyte* i_buffer){
	M_views.apply(bind(&J_View::update_image_buffer, _1, i_obj_id, i_buffer));
}

void J_UI_Model::notify_new_image_buffer_mono(j_uint i_obj_id, const j_ubyte* i_buffer){
	M_views.apply(bind(&J_View::update_image_buffer_mono, _1, i_obj_id, i_buffer));
}

void J_UI_Model::notify_image_height(j_uint i_obj_id, int i_height){
	M_views.apply(bind(&J_View::update_image_height, _1, i_obj_id, i_height));
}

void J_UI_Model::notify_image_width(j_uint i_obj_id, int i_width){
	M_views.apply(bind(&J_View::update_image_width, _1, i_obj_id, i_width));
}


void J_UI_Model::notify_outline_thickness(j_uint i_obj_id, j_float i_thickness){
	M_views.apply(bind(&J_View::update_outline_thickness, _1, i_obj_id, i_thickness));
}

void J_UI_Model::notify_image_input_format(j_uint i_obj_id, Image_Format i_format){
	M_views.apply(bind(&J_View::update_image_buffer_format, _1, i_obj_id, i_format));
}


void J_UI_Model::notify_middle_line_color(j_uint i_obj_id, J_UI_Color i_color){
	M_views.apply(bind(&J_View::update_middle_line_color, _1, i_obj_id, i_color));
}


bool J_UI_Model::is_text_box_present(j_uint i_text_box_id){
	return (0 != M_text_boxes.count(i_text_box_id));
}


void J_UI_Model::notify_clickable_status(j_uint i_obj_id, bool i_status){
	assert(M_ui_objects.count(i_obj_id));
	for_each(M_views.begin(), M_views.end(), bind(&J_View::set_clickable_status, _1, i_obj_id, i_status));
}

void J_UI_Model::broadcast_current_state()const{
	for(auto obj_pair : M_ui_objects){
		obj_pair.second->broadcast_current_state();	
	}
}

void J_UI_Model::remove_view(J_View_Shared_t i_view){M_views.erase(i_view);}

J_UI_Model::~J_UI_Model(){}

void J_UI_Model::clear_data(){
	M_ui_objects.clear();
	M_ui_boxes.clear();
	M_text_boxes.clear();
	M_image_panes.clear();
	M_views.clear();

}



}
