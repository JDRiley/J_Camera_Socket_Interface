#include "J_Camera_Socket_Controller.h"
//
#include "J_Camera_Socket_Model.h"
//
#include <J_UI/J_View.h>
//
#include "J_Camera_Stream.h"
//
#include <J_UI/J_Font_Manager.h>
//
#include <J_Socket_Server.h>
//
#include <J_Camera_Manager.h>
//
#include <J_UI/J_Text_Box.h>
//
#include <GLFW/glfw3.h>
//
#include <J_OpenGl.h>
//
#include <algorithm>
//
#include <iostream>
//
#include <iomanip>
//
#include <J_String.h>
//
#include "J_Camera_Notifications.h"
//
#include "Gesture_Handler.h"


using std::bind; using std::for_each; using std::mem_fn; using std::ostringstream;
using namespace std::placeholders; using std::cerr; using std::wcerr;
using std::cout; using std::wcout; using std::endl; using std::string;
using std::wstring; using std::dynamic_pointer_cast; using std::array;
using std::min; using std::setw; using std::ios; using std::fixed;
namespace jomike{
static Instance_Pointer<J_Camera_Socket_Controller> s_controller;
static Instance_Pointer<J_Camera_Socket_Model> s_camera_model;
static Instance_Pointer<J_Camera_Manager> s_camera_manager;
static Instance_Pointer<Contexts_Handler> s_contexts_handler;
static Instance_Pointer<J_Font_Manager> s_font_manager;
static Instance_Pointer<Contexts_Handler> s_contexts;



static void camera_interface_window_close_callback(j_window_t i_window){
	s_controller->remove_view(i_window);
}
static void camera_interface_mouse_button_callback(j_window_t i_window, int i_mouse_button, int i_action, int i_modifiers){

	s_controller->mouse_button_cmd(i_window, i_mouse_button, i_action, i_modifiers);
}

static void set_gesture_box_settings(J_Text_Box_Shared_t);

static void set_menu_box_settings(J_Text_Box_Shared_t);

void set_device_command(J_UI_Object_Shared_t i_obj, int, int, Pen_Pos_FL_t){
	s_controller->set_stream(i_obj->get_ID());
}

static bool toggle_text_box(J_Text_Box_Shared_t i_text_box, J_UI_Color i_true_background_color
	, J_UI_Color i_true_outline_color, J_UI_Color i_true_text_color
	, J_UI_Color i_false_background_color, J_UI_Color i_false_outline_color
	, J_UI_Color i_false_text_color){
	i_text_box->toggle();
	bool toggle_status = i_text_box->toggle_status();
	if(toggle_status){
		i_text_box->set_fill_color(i_true_background_color);
		i_text_box->set_outline_color(i_true_outline_color);
		auto mirror_string = *(i_text_box->get_string().get_string_holding_index(0));

		i_text_box->change_color_at_pos(0, i_true_text_color);
	} else{
		i_text_box->set_fill_color(i_false_background_color);
		i_text_box->set_outline_color(i_false_outline_color);
		i_text_box->change_color_at_pos(0, i_false_text_color);
	}
	i_text_box->broadcast_current_state();
	return toggle_status;
}




J_UI_Controller& J_UI_Controller::get_instance(){
	return *s_controller;
}

J_UI_Model& J_UI_Model::get_instance(){
	return *s_camera_model;
}


static void reverse_stream_image(J_UI_Object_Shared_t i_obj, int, int, Pen_Pos_FL_t);

void set_gesture_box_settings(J_Text_Box_Shared_t i_gesture_box){
	i_gesture_box->set_colors(J_Color::Clear, J_Color::Yellow, J_Color::Black);
	i_gesture_box->set_outline_visibility_status(true);
	i_gesture_box->set_outline_thickness(2.0f);
	i_gesture_box->set_fill_visibility_status(true);
	i_gesture_box->broadcast_current_state();
}

void set_menu_box_settings(J_Text_Box_Shared_t i_text_box){
	i_text_box->set_outline_visibility_status(true);
	i_text_box->set_fill_visibility_status(true);
	i_text_box->set_colors(J_Color::Clear, J_Color::Cyan.alpha(0.2f), J_Color::Cyan);
	i_text_box->set_clickable_status(true);
	i_text_box->broadcast_current_state();
}

void cursor_pos_callback(j_window_t i_window, j_dbl, j_dbl);

/*cursor_pos_callback(j_window_t, j_dbl, j_dbl)*/
void cursor_pos_callback(j_window_t i_window, j_dbl i_x_pos, j_dbl i_y_pos){
	s_controller->cursor_pos_input_cmd(i_window, i_x_pos, i_y_pos);
}

static void reverse_stream_image(J_UI_Object_Shared_t i_obj, int, int, Pen_Pos_FL_t){
	auto text_box = dynamic_pointer_cast<J_Text_Box>(i_obj);
	

	assert(text_box);
	bool reverse_status 
		= toggle_text_box(text_box, J_BLUE, J_CYAN, J_YELLOW, J_BLACK, J_WHITE, J_CYAN);
	s_camera_manager->set_mirror_status(reverse_status);
}


J_Camera_Socket_Controller& J_Camera_Socket_Controller::get_instance(){
	static J_Camera_Socket_Controller controller;
	return controller;
	
}

J_Camera_Socket_Controller::J_Camera_Socket_Controller(){ 
#ifndef VS_2013
	default_initialization();
#endif //!VS_2013
	M_continue_flag = true; 
	//M_socket = J_Socket_Server_Shared_t(new J_Socket_Server(9002));
	M_manager_view 
		= J_View_Shared_t(new J_View(1500, 800, "J_Camera_Manager"));

	add_view(M_manager_view);
	M_manager_view->make_active_context();
	
	j_clear();
	j_swap_buffers(M_manager_view->get_context());

	M_log_font_face 
		= s_font_manager->get_font_face(M_manager_view->get_context(), "times", 14);
	M_notification_font_face 
		= s_font_manager->get_font_face(M_manager_view->get_context(), "times_italic", 14);

	J_Rectangle pane_rectangle(-0.4f, -0.6f, 1.2f, 1.2f);
	J_Camera_Stream_Shared_t color_stream(
		new J_Camera_Stream(pane_rectangle)
		);
	J_Camera_Stream_Shared_t depth_stream(
		new J_Camera_Stream(pane_rectangle, Image_Format::RGBA32_UBYTE)
		);

	M_depth_stream_pane_id = depth_stream->get_ID();
	M_color_stream_pane_id = color_stream->get_ID();


	add_stream_pane(color_stream);
	add_stream_pane(depth_stream);
	
	set_pane_settings(color_stream);
	set_pane_settings(depth_stream);
	depth_stream->set_outline_and_fill_visibility_status(false);
	


	auto gesture_textbox_primary
		= J_Text_Box_Shared_t(new J_Text_Box(J_Rectangle(-1.0f, -1.0f, 1.0f, 0.15f)));

	auto gesture_textbox_secondary
		= J_Text_Box_Shared_t(new J_Text_Box(J_Rectangle(0.0f, -1.0f, 1.0f, 0.15f)));

	auto gesture_data_box
		= J_Text_Box_Shared_t(new J_Text_Box(J_Rectangle(-1.0f, -0.85f, 0.5f, 0.30f)));
	
	auto cursor_pos_box
		= J_Text_Box_Shared_t(new J_Text_Box(J_Rectangle(-1.0f, -0.55f, 0.5f, 0.15f)));


	add_text_box_to_manager(gesture_textbox_primary);
	add_text_box_to_manager(gesture_textbox_secondary);
	add_text_box_to_manager(gesture_data_box);
	add_text_box_to_manager(cursor_pos_box);

	set_gesture_box_settings(gesture_textbox_primary);
	set_gesture_box_settings(gesture_textbox_secondary);
	set_menu_box_settings(gesture_data_box);
	set_menu_box_settings(cursor_pos_box);

	M_gesture_handler
		= Gesture_Handler_Shared_t(new Gesture_Handler(gesture_textbox_primary
		, gesture_textbox_secondary, M_log_font_face));

	M_gesture_fps = 0.0f;


		


	M_gesture_fps_text_id = gesture_data_box->get_ID();
	M_cursor_pos_text_box_id = cursor_pos_box->get_ID();

	init_circles_above_gesture_notification();
	auto geo_node_array = init_geo_circles();
	auto face_boxes = init_face_boxes();
	assert(face_boxes.size() == safe_uns_cast(MAX_FACES));
	array<J_UI_Box_Weak_t, MAX_FACES> weak_face_boxes;
	for(size_t i = 0; i < MAX_FACES; i++){
		weak_face_boxes[i] = face_boxes[i];
	}
	auto landmark_circles_array = init_landmark_circles();
	

	//s_camera_manager->add_depth_stream_notification_callback(
	//	Depth_Stream_Notification(depth_stream)
	//	);
	M_face_data_handler
		= Face_Data_Handler_Shared_t(new Face_Data_Handler(weak_face_boxes, landmark_circles_array
		, color_stream, M_manager_view->get_context()));

	M_geonodes_handler = 
		GeoNodes_Handler_Shared_t( 
			new GeoNodes_Handler(geo_node_array, color_stream,
				M_manager_view->get_context())
				);

	M_depth_stream_handler = Depth_Stream_Notification_Shared_t(
		new Depth_Stream_Notification(depth_stream)
		);

	M_color_stream_handler 
		= Color_Stream_Notification_Shared_t( 
			new Color_Stream_Notification(color_stream)
		);

	j_focus_context(M_manager_view->get_context());
	M_manager_view->clear_window();
	M_manager_view->draw();
	init_device_text_boxes();
	s_camera_manager->get_instance();
	init_toggles();
	start_background_ramp();
	
}

#ifndef VS_2013
void J_Camera_Socket_Controller::default_initialization(){
	M_background_ramping_status = false;
	M_initial_background_color = (J_RED*0.12f + J_BLUE*0.02f).alpha(1.0f);
	M_background_color = J_UI_Color(0.0f, 0.01f, 0.1f, 1.0f);
}
#endif //!VS_2013

void J_Camera_Socket_Controller::init_toggles(){
	J_Rectangle rectangle(0.75f, 0.8f, 0.23f, 0.15f);
	J_UI_String mirror_text("  Mirror", M_log_font_face, J_GREEN);
	J_Text_Box_Shared_t mirror_text_box(new J_Text_Box(rectangle, mirror_text));
	add_text_box(mirror_text_box);
	mirror_text_box->set_clickable_status(true);
	mirror_text_box->set_mouse_button_press_callback(reverse_stream_image);
	mirror_text_box->set_outline_and_fill_visibility_status(true);
	mirror_text_box->mouse_button_press(0, 0, Pen_Pos_FL_t());

}

ex_array<ex_array<J_UI_Circle_Shared_t>> J_Camera_Socket_Controller::init_landmark_circles(){
	J_Circle base_circle(0.0f, 0.0f, 0.013f);
	ex_array<ex_array<J_UI_Circle_Shared_t>> landmark_circles(MAX_FACES);
	for(int i = 0; i < MAX_FACES; i++){
		for(int b = 0; b < NUM_LANDMARKS; b++){
			J_UI_Circle_Shared_t new_circle(new J_UI_Circle(base_circle));
			landmark_circles[i].push_back(new_circle);
			add_circle(new_circle);
			new_circle->set_fill_color((J_RED*0.8f).alpha(0.8f)+J_CYAN.alpha(0.6f));
			new_circle->set_fill_visibility_status(false);
			new_circle->set_outline_color(J_BLACK);
			new_circle->set_outline_visibility_status(false);
			new_circle->set_outline_thickness(1.0f);

		}
	}
	return landmark_circles;
}

void J_Camera_Socket_Controller::init_circles_above_gesture_notification(){
	J_Circle circle_shape;
	circle_shape.set_radius(0.03f);
	j_float radius = circle_shape.radius();

	auto gesture_text_box = s_camera_model->get_text_box(M_gesture_fps_text_id);
	circle_shape.set_x(gesture_text_box->x2() + 1.05f*radius);
	circle_shape.set_y(gesture_text_box->y2() + 1.05f*radius*s_contexts->ratio());

	int circles_added = 0;
	while ((circle_shape.x1() + radius) < 1.0f){
		J_UI_Circle_Shared_t new_circle(
			new J_UI_Circle(circle_shape));
		add_circle(new_circle);
		new_circle->set_fill_color(J_Color::Cyan.alpha(0.2f));
		new_circle->set_fill_visibility_status(true);
		new_circle->set_outline_color(J_Color::Cyan);
		new_circle->set_outline_visibility_status(true);
		new_circle->set_outline_thickness(1.0f);
		circle_shape.set_x(circle_shape.x1() + 2.10f*radius);
		++circles_added;
	}
	cerr << "\n**** " << circles_added << " Circles Added!";
}

void J_Camera_Socket_Controller::set_pane_settings(J_Camera_Stream_Shared_t i_pane){
	i_pane->set_outline_color(M_initial_background_color);
	i_pane->set_outline_visibility_status(true);
	j_float ratio = s_contexts->ratio();
	j_float width = 1.2f;
	j_float aspect_ratio = 1280.0f / 720.0f;
	i_pane->set_width(width);
	i_pane->set_height(width*ratio / aspect_ratio);
	i_pane->set_outline_thickness(3.0f);
}

GeoNode_Multi_Array_t J_Camera_Socket_Controller::init_geo_circles(){
	J_Circle base_circle(0.0f, 0.0f, 0.013f);
	GeoNode_Multi_Array_t geo_node_array;
	for (int i = 0; i < NUM_GEO_SIDES; i++){
		for (int b = 0; b < NUM_NODES_PER_GEO_SIDE; b++){
			J_UI_Circle_Shared_t new_circle(new J_UI_Circle(base_circle));
			add_circle(new_circle);
			new_circle->set_fill_color(J_BLACK.alpha(0.3f));
			new_circle->set_fill_visibility_status(false);
			new_circle->set_outline_color(J_WHITE);
			new_circle->set_outline_visibility_status(false);
			new_circle->set_outline_thickness(1.0f);

			geo_node_array[i][b] = new_circle;
		}
	}
	return geo_node_array;
}

array<J_UI_Box_Shared_t, MAX_FACES> J_Camera_Socket_Controller::init_face_boxes(){
	J_Rectangle rectangle(0.0f, 0.0f, 1.0f, 1.0f);
	array<J_UI_Box_Shared_t, MAX_FACES> face_boxes;
	for (int i = 0; i < MAX_FACES; i++){
		J_UI_Box_Shared_t new_ui_box(new J_UI_Box(rectangle));
		face_boxes[i] = new_ui_box;
		add_box(new_ui_box);
		new_ui_box->set_fill_visibility_status(false);
		new_ui_box->set_outline_visibility_status(false);
		new_ui_box->set_fill_color(J_BLACK.alpha(0.2f));
		new_ui_box->set_outline_color(J_BLUE);
		M_face_id_boxes[i] = new_ui_box->get_ID();
	}

	return face_boxes;
}

void J_Camera_Socket_Controller::start_background_ramp(){
	M_background_ramping_status = true;
}

void J_Camera_Socket_Controller::background_ramp(){
	static const j_dbl sk_ramp_down_time = 1.0;
	static const j_dbl sk_ramp_up_time = 1.5;
	static const j_dbl sk_ramp_comptete_time = 2.5;
	static j_dbl s_background_ramp_start_time = 0.0;
	static bool s_was_ramping = false;

	if (!M_background_ramping_status){
		return;
	}


	if (!s_was_ramping && M_background_ramping_status){
		s_was_ramping = true;
		s_background_ramp_start_time = j_get_time();
	}

	j_dbl time = j_get_time() - s_background_ramp_start_time;

	if (time > sk_ramp_comptete_time){
		s_was_ramping = false;
		M_background_ramping_status = false;
		j_clear_color(M_background_color);
	}
	else if (time > sk_ramp_down_time){
		j_float ratio = static_cast<j_float>((time - sk_ramp_down_time)
			/ sk_ramp_up_time);
		j_clear_color((M_background_color*ratio).alpha(1.0f));
	}
	else{
		j_clear_color((M_initial_background_color
			*static_cast<j_float>(1.0f - time / sk_ramp_down_time)).alpha(1.0f));
	}
}

void J_Camera_Socket_Controller::update_fps_box(){
	static string fps_string("Fps: ");
	static int original_string_size = safe_int_cast(fps_string.size());
	j_dbl fps = s_camera_manager->get_gesture_fps();
	if (fps != M_gesture_fps){
		M_gesture_fps = fps;
		fps_string.resize(original_string_size);
		fps_string += number_to_string(M_gesture_fps);

		s_camera_model->get_text_box(M_gesture_fps_text_id)
			->set_string(J_UI_String(fps_string, M_log_font_face, J_Color::White));
	}
}

void J_Camera_Socket_Controller::execute(){


	M_socket->launch_server();

	glfwSetCursorPosCallback(M_manager_view->get_window(), cursor_pos_callback);
	while (M_continue_flag){
		poll_events();
		M_socket->poll_events();
		s_camera_manager->poll_events();
		s_camera_model->update();
		update_fps_box();
		background_ramp();

		M_socket->send_data();

		//Draw the views
		draw_views();


		if(!has_views()){
			end_execute();
		}
	}
}

J_Camera_Socket_Controller::~J_Camera_Socket_Controller(){
	clear_all();
}

void J_Camera_Socket_Controller::end_execute(){
	M_continue_flag = false;
	M_socket->close_server();
	s_camera_manager->notify_end();
	M_manager_view.reset();
	M_cur_clicked_view.reset();
//	clear_all();
}

void J_Camera_Socket_Controller::poll_events()const{
	glfwPollEvents();
}

void J_Camera_Socket_Controller::clear_all(){
	J_UI_Controller::clear_all();
	M_manager_view.reset();

	s_contexts_handler->destroy();

	delete M_log_font_face;
	delete M_notification_font_face;


	M_log_font_face = M_notification_font_face = nullptr;
	


}

void J_Camera_Socket_Controller::set_stream(j_uint i_device_text_box_id){
	s_camera_manager->set_stream(M_devices_by_text_box_id[i_device_text_box_id]);
	
	auto gesture_processor = s_camera_manager->get_gesture_processor();
	gesture_processor->add_gesture_notification_callback(*M_gesture_handler);
	gesture_processor->add_gesture_notification_callback(
							Gesture_Socket_Notification(M_socket)
						);


	gesture_processor->add_geonode_notification_callback(*M_geonodes_handler);

	auto face_processor = s_camera_manager->get_face_processor();

	face_processor->add_face_notification_callback(*M_face_data_handler);

	//auto depth_processor = s_camera_manager->get_depth_stream_processor();
	//depth_processor->add_depth_stream_notification_callback(*M_depth_stream_handler);
	//depth_processor->set_depth_stream_alpha(0.5f);

	auto color_processor = s_camera_manager->get_color_stream_processor();
	color_processor->add_color_stream_notification_callback(*M_color_stream_handler);

	auto color_stream_pane = s_camera_model->get_camera_stream(M_color_stream_pane_id);
	auto depth_stream_pane = s_camera_model->get_camera_stream(M_depth_stream_pane_id);
	j_float ratio = s_contexts->ratio();
	j_float aspect_ratio = s_camera_manager->get_aspect_ratio();
#if _DEBUG
	j_float width = 1.0f;
#else
	j_float width = 1.3f;
#endif
	j_float height = width*ratio / aspect_ratio;
	auto& depth_stream = s_camera_manager->get_depth_stream();
	color_stream_pane->set_width(width);
	color_stream_pane->set_height(height);
	depth_stream_pane->set_image_width(depth_stream.width());
	depth_stream_pane->set_image_height(depth_stream.height());
	depth_stream_pane->set_x(color_stream_pane->x1());
	depth_stream_pane->set_y(color_stream_pane->y2());
	depth_stream_pane->set_width(width);
	depth_stream_pane->set_height(height);
	depth_stream_pane->set_outline_visibility_status(true);
	depth_stream_pane->set_outline_thickness(1.0f);
	depth_stream_pane->set_outline_color(J_WHITE.alpha(0.4f));
	/*
	j_float x_ratio
		= s_camera_manager->depth_color_stream_fov_x_ratio();

	j_float y_ratio
	 = s_camera_manager->depth_color_stream_fov_y_ratio();

	x_ratio = pow(x_ratio, 1.18f);
	y_ratio = pow(y_ratio, 3.0f);

	j_float depth_width = width*x_ratio;
	j_float depth_height = height*y_ratio;

	depth_stream_pane->set_width(depth_width);
	depth_stream_pane->set_height(depth_height);


	depth_stream_pane->set_x(depth_stream_pane->x1() - (depth_width -width)/2.0f);
	depth_stream_pane->set_y(depth_stream_pane->y2() - (depth_height - height) / 2.0f);
	*/
}

void J_Camera_Socket_Controller::notify_text_box_press(J_View_Shared_t i_view, j_uint i_text_obj_id
	, j_size_t i_cursor_index){

		assert(s_camera_model->is_text_box_present(i_text_obj_id));
		J_Text_Box_Shared_t text_box = s_camera_model->get_text_box(i_text_obj_id);
		text_box->set_cursor_pos(i_cursor_index);
		//text_box->set_left_click_on();
		text_box->mouse_button_press(J_LEFT_MOUSE_BUTTON, J_PRESS, M_cursor_pos_fl);
		M_active_ui_objs[i_view] = text_box;

}

void J_Camera_Socket_Controller::notify_text_box_release(j_uint i_text_obj_id, j_size_t i_cursor_index){

	assert(s_camera_model->is_text_box_present(i_text_obj_id));
	auto text_box = s_camera_model->get_text_box(i_text_obj_id);

	text_box->set_cursor_pos(i_cursor_index);
	text_box->set_left_click_off();

}


void J_Camera_Socket_Controller::add_circle(J_UI_Circle_Shared_t i_circle_ptr){
	assert(M_manager_view);
	M_manager_view->add_display_circle(i_circle_ptr->get_ID());
	s_camera_model->add_ui_circle(i_circle_ptr);
	i_circle_ptr->broadcast_current_state();


}

void J_Camera_Socket_Controller::add_box(J_UI_Box_Shared_t i_box_ptr){
	assert(M_manager_view);
	M_manager_view->add_display_box(i_box_ptr->get_ID());
	s_camera_model->add_box(i_box_ptr);
	i_box_ptr->broadcast_current_state();
}

void J_Camera_Socket_Controller::add_text_box(J_Text_Box_Shared_t i_text_box){
	assert(M_manager_view);
	M_manager_view->add_text_display(i_text_box->get_ID());
	s_camera_model->add_text_box(i_text_box);
	i_text_box->broadcast_current_state();
}



void J_Camera_Socket_Controller::init_device_text_boxes(){
	auto device_names = s_camera_manager->get_device_names();

	for (int i = 0; i < device_names.size(); i++){

		j_float y_height = to_y_screen(M_manager_view->get_window(), 50);
		J_Text_Box_Shared_t new_text_box
			= J_Text_Box_Shared_t(new J_Text_Box(J_Rectangle(-0.9f, 0.8f
			- i*y_height
			, to_x_screen(M_manager_view->get_window(), 300), y_height)
			, J_UI_String(device_names[i].c_str(), M_log_font_face, J_Color::White)));


		M_manager_view->add_text_display(new_text_box->get_ID());
		s_camera_model->add_text_box(new_text_box);

		set_menu_box_settings(new_text_box);
		new_text_box->broadcast_current_state();

		assert(!M_devices_by_text_box_id.count(new_text_box->get_ID()));

		M_devices_by_text_box_id[new_text_box->get_ID()] = device_names[i];

		new_text_box->set_mouse_button_press_callback(set_device_command);

		

		wcerr << '\n' << device_names[i] << endl;
		if (!s_camera_manager->gesture_detection_status()){
			M_manager_view->mouse_button_press(M_manager_view, J_LEFT_MOUSE_BUTTON, 0
				, Pen_Pos_FL_t(new_text_box->x1() + new_text_box->width() / 2
				, new_text_box->y1() - new_text_box->height() / 2));
		}
	}

}

void J_Camera_Socket_Controller::cursor_pos_input_cmd(j_window_t i_window, j_dbl i_x_pos, j_dbl i_y_pos){
	M_cursor_pos.first = static_cast<int>(i_x_pos);
	M_cursor_pos.second = static_cast<int>(i_y_pos);
	M_cursor_pos_fl = screen_coords(i_window, M_cursor_pos);

	ostringstream o_str;
	auto width_manip = setw(8);
	o_str.precision(3);
	o_str << fixed;
	o_str << '\t' << M_cursor_pos_fl.first << " : " << M_cursor_pos_fl.second;
	auto cursor_pos_box = s_camera_model->get_text_box(M_cursor_pos_text_box_id);
	cursor_pos_box->set_string(J_UI_String(o_str.str(), M_log_font_face, J_WHITE));
}


void J_Camera_Socket_Controller::derived_init(int argc, char** argv){
	//initialize_text_data();
	glfwSetCursorPosCallback(M_manager_view->get_window(), cursor_pos_callback);
	glfwSetMouseButtonCallback(M_manager_view->get_window()
		, camera_interface_mouse_button_callback);
	glfwSetWindowCloseCallback(M_manager_view->get_window()
		, camera_interface_window_close_callback);
	short socket_number = 0;
	if(argc > 1){
		socket_number = safe_cast<short>(atoi(argv[1]));
	}
	socket_number = socket_number ? socket_number : 9002;
	M_socket = J_Socket_Server_Shared_t(new J_Socket_Server(socket_number));
}

void J_Camera_Socket_Controller::remove_view(j_window_t i_window){
	J_UI_Controller::remove_view(i_window);
	M_manager_view->clear();
	M_manager_view.reset();
}

void J_Camera_Socket_Controller::mouse_button_cmd(j_window_t i_window, int i_mouse_key
	, int i_action, int i_modifiers){
	assert(M_manager_view->get_window() == i_window);
	if(J_PRESS != i_action){
		return;
	}

	M_manager_view->mouse_button_press(M_manager_view, i_mouse_key
		, i_modifiers, M_cursor_pos_fl);

}

void J_Camera_Socket_Controller::add_stream_pane(J_Camera_Stream_Shared_t i_stream){
	M_manager_view->add_image_pane(i_stream->get_ID());
	s_camera_model->add_camera_stream(i_stream);
}

void J_Camera_Socket_Controller::add_text_box_to_manager(J_Text_Box_Shared_t i_text_box){
	M_manager_view->add_text_display(i_text_box->get_ID());
	s_camera_model->add_text_box(i_text_box);
}

J_Font_Face J_Camera_Socket_Controller::log_font_face()const{
	return M_log_font_face;
}



}

