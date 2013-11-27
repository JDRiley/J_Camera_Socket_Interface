#ifndef J_UI_MODEL_H
#define J_UI_MODEL_H

#include <J_Error.h>
#include "J_UI_Fwd_Decl.h"
#include <J_Fwd_Decl.h>
#include <ex_array.h>
#include "J_Image/J_Image.h"
#include <J_Utile.h>
#include <J_UI/J_UI_String.h>
#include <RB_Tree.h>
//Containers
#include <map>

//Utilities
#include <memory>



namespace jomike{

class J_UI_Model{
public:
	void J_UI_Model::update();

	//View Management
	virtual void attach_view(J_View_Shared_t);
	//enum Font_Indices{MATH_SYMBOLS, INPUT, ANSWER, ERROR, LOG, FONT_END};
	virtual void add_text_box(J_Text_Box_Shared_t);
	virtual void add_box(J_UI_Box_Shared_t);
	virtual void add_image_pane(J_Image_Pane_Shared_t);
	virtual void add_ui_object(J_UI_Object_Shared_t);
	virtual void add_ui_circle(J_UI_Circle_Shared_t);
	void notify_erase_chars(j_uint text_box_id, j_size_t pos, j_size_t size);
	void notify_char_delete(j_uint text_box_id, j_size_t pos);
	void notify_char_add(j_uint text_id, j_size_t pos, J_UI_Char i_char);
	void notify_text_string(j_uint text_box_id, const J_UI_Multi_String&);
	void notify_text_cursor(j_uint obj_id, j_size_t cursor_pos);
	void notify_text_cursor_color(j_uint obj_id, J_Color_RGBA<j_float>);
	void notify_outline_visibility_status(j_uint obj_id, bool status);
	void notify_fill_visibility_status(j_uint obj_id, bool status);

	void notify_box_coordinates(j_uint i_obj_id, const J_Rectangle& i_rectangle);
	void notify_fill_color(j_uint obj_id, const J_UI_Color& i_color);
	void notify_outline_color(j_uint obj_id, const J_Color_RGBA<j_float>);
	void notify_outline_thickness(j_uint obj_id, j_float);
	
	void notify_center(j_uint obj_id, j_float x_pos, j_float y_pos);
	void notify_radius(j_uint obj_id, j_float radius);
	void notify_circle_shape_data(j_uint obj_id, const J_Circle&);

	//Image
	void notify_new_image_buffer(j_uint, const j_ubyte*);
	void notify_new_image_buffer_mono(j_uint, const j_ubyte*);
	void notify_image_clear(j_uint);
	void notify_image_width(j_uint, int);
	void notify_image_height(j_uint, int);
	void notify_image_input_format(j_uint, Image_Format);
	void notify_middle_line_color(j_uint, J_UI_Color);
	bool is_text_box_present(j_uint i_text_box_id);
	void notify_clickable_status(j_uint obj_id, bool);
	J_Text_Box_Shared_t get_text_box(j_uint id);
	J_UI_Object_Shared_t get_ui_object(j_uint id);
	J_UI_Circle_Shared_t get_ui_circle(j_uint id);
	J_UI_Box_Shared_t get_ui_box(j_uint id);
	void broadcast_current_state()const;
	//J_Font_Face get_font_face(int index)const;
	virtual ~J_UI_Model();
	virtual void clear_data();

	virtual void remove_view(J_View_Shared_t);
protected:
	J_UI_Model();
private:
	static J_UI_Model& get_instance();
	friend class Instance_Pointer<J_UI_Model>;

	typedef std::map<j_uint, J_UI_Object_Shared_t> UI_Obj_Cont_t;
	UI_Obj_Cont_t M_ui_objects;
	typedef std::map<j_uint, J_Text_Box_Shared_t> Text_Box_Cont_t;
	Text_Box_Cont_t M_text_boxes;

	typedef std::map<j_uint, J_UI_Box_Shared_t> J_UI_Box_Cont_t;
	J_UI_Box_Cont_t M_ui_boxes;

	typedef std::map<j_uint, J_UI_Circle_Shared_t> J_UI_Circle_Cont_t;
	J_UI_Circle_Cont_t M_ui_circles;

	typedef RB_Tree<J_View_Shared_t> J_View_Cont_t;
	J_View_Cont_t M_views;

	typedef std::map<j_uint, J_UI_Box_Shared_t> J_Image_Pane_Cont_t;
	J_Image_Pane_Cont_t M_image_panes;



};


}



#endif