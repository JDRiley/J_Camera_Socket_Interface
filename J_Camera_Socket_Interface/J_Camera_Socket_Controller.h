#ifndef J_CAMERA_SOCKET_CONTROLLER_H
#define J_CAMERA_SOCKET_CONTROLLER_H

//
#include "J_Camera_Socket_Interface_Fwd_Decl.h"
//
#include <J_Image.h>
//
#include <J_OpenGL_Fwd_Decl.h>
//
#include <J_UI/J_UI_Controller.h>
//
#include <J_Web_Socket_Fwd_Decl.h>
//
#include <ex_array.h>
//
#include <J_PXC_Fwd_Decl.h>

//Containers
#include <map>

namespace jomike{
	
	typedef std::array<
		std::array<J_UI_Circle_Weak_t, NUM_NODES_PER_GEO_SIDE>
		, NUM_GEO_SIDES>
		GeoNode_Multi_Array_t;

class J_Camera_Socket_Controller : public J_UI_Controller{
public:
	void execute();
	void remove_view(j_window_t)override;
	J_Context_Shared_t get_current_context();
	//Accessors
	J_View_Shared_t get_view(j_window_t);
	J_View_Shared_t get_view_and_set_active(j_window_t);
	void cursor_pos_input_cmd(j_window_t, j_dbl, j_dbl);
	void resize_cmd(j_window_t, int width, int height);
	void window_close_cmd(j_window_t);
	void char_input_cmd(j_window_t, j_ulint);
	void key_input_cmd(j_window_t, int charcode, int, int, int);
	void integral_input_cmd(j_window_t, int key, int scancode, int action, int modifier);
	void function_input_cmd(j_window_t, int charcode, int, int, int);
	void text_key_input_cmd(j_window_t, int charcode, int, int, int);
	void mouse_button_cmd(j_window_t, int mouse_key, int action, int modifiers);

	void notify_text_box_release(j_uint text_obj_id, j_size_t cursor_index)override;
	void notify_text_box_press(J_View_Shared_t, j_uint text_obj_id, j_size_t cursor_index)override;
	void set_stream(j_uint);
	void camera_execute(J_View_Shared_t);
	void clear_all()override;
	~J_Camera_Socket_Controller();
	J_Font_Face log_font_face()const;
private:
	void derived_init(int argc, char** argv)override;
	void poll_events()const;
	void wait_events()const;
protected:
private:
#ifdef VS_2013
	bool M_background_ramping_status = false;
	const J_UI_Color M_initial_background_color = (J_RED*0.12f + J_BLUE*0.02f).alpha(1.0f);
	const J_UI_Color M_background_color = J_UI_Color(0.0f, 0.01f, 0.1f, 1.0f);
#else
	bool M_background_ramping_status;
	J_UI_Color M_initial_background_color;
	J_UI_Color M_background_color;
	void default_initialization();
#endif // VS_2013



	static J_Camera_Socket_Controller& get_instance();
	J_Camera_Socket_Controller();
	friend class Instance_Pointer<J_Camera_Socket_Controller>;
	void init_toggles();
	J_Font_Face M_log_font_face;
	J_Font_Face M_notification_font_face;
	void add_box(J_UI_Box_Shared_t);
	void add_circle(J_UI_Circle_Shared_t);
	
	J_View_Shared_t M_manager_view;
	J_Socket_Server_Shared_t M_socket;
	bool M_continue_flag;
	void end_execute();
	Pen_Pos_t M_cursor_pos;
	Pen_Pos_FL_t M_cursor_pos_fl;
	j_uint M_cursor_pos_text_box_id;
	j_uint M_mode_text_box_id;
	J_View_Shared_t M_cur_clicked_view;
	typedef std::map<J_View_Shared_t, J_UI_Object_Weak_t> Active_UI_Obj_Cont_t;
	Active_UI_Obj_Cont_t M_active_ui_objs;

	j_context_t M_current_context;
	ex_array<ex_array<J_UI_Circle_Shared_t>> init_landmark_circles();
	std::map<j_uint, std::wstring> M_devices_by_text_box_id;
	void init_device_text_boxes();
	void update_fps_box();
	typedef
		void(J_Camera_Socket_Controller::*Text_Key_Cmd_fp_t)(j_window_t, int charcode, int, int, int);

	Gesture_Handler_Shared_t M_gesture_handler;
	Face_Data_Handler_Shared_t M_face_data_handler;
	GeoNodes_Handler_Shared_t M_geonodes_handler;

	void add_text_box(J_Text_Box_Shared_t);
	j_uint M_face_id_boxes[MAX_FACES];
	j_uint M_gesture_primary_text_id;
	j_uint M_gesture_secondary_text_id;
	j_uint M_gesture_fps_text_id;
	j_uint M_color_stream_pane_id;
	j_uint M_depth_stream_pane_id;
	void start_background_ramp();
	void background_ramp();

	j_dbl M_gesture_fps;
	Depth_Stream_Notification_Shared_t M_depth_stream_handler;
	Color_Stream_Notification_Shared_t M_color_stream_handler;
	void init_circles_above_gesture_notification();


	GeoNode_Multi_Array_t init_geo_circles();
	std::array<J_UI_Box_Shared_t, MAX_FACES> init_face_boxes();
	void set_pane_settings(J_Camera_Stream_Shared_t i_pane);
	void add_stream_pane(J_Camera_Stream_Shared_t color_stream);
	void add_text_box_to_manager(J_Text_Box_Shared_t i_text_box);
	
};

}

#endif