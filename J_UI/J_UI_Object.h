#ifndef J_UI_OBJECT_H
#define J_UI_OBJECT_H

#include "J_UI_Fwd_Decl.h"
//
#include "J_Image/J_Image.h"
//Container
#include <ex_array.h>

#include "J_Shape.h"
namespace jomike{

j_dbl j_get_time();

typedef void (*Mouse_Press_Func_t)
		(J_UI_Object_Shared_t, int, int, Pen_Pos_FL_t);



class J_UI_Outline_Fill_Management : public J_Outline_Fill_Management{
public:
	J_UI_Outline_Fill_Management(j_uint);
	void set_outline_visibility_status(bool)override;
	void set_fill_visibility_status(bool)override;

	void set_outline_thickness(j_float)override;
	
	virtual void set_outline_color(const J_Color_RGBA<j_float>&);
	virtual void set_fill_color(const J_UI_Color&);

	
	J_UI_Color fill_color()const;
	J_UI_Color outline_color()const;
	void broadcast_outline_fill_state()const;
	j_uint get_ID()const;
private:
	const j_uint M_ID;
	J_UI_Color M_fill_color;
	J_UI_Color M_outline_color;

};


class J_UI_Object : public J_UI_Outline_Fill_Management
	, public virtual J_Shape, public std::enable_shared_from_this<J_UI_Object>{
public:
	virtual void update(){}
	j_uint get_ID()const{return M_ID;}
#if VS_2013
	J_UI_Object& operator=(const J_UI_Object&) = delete;
#endif
	virtual void set_left_click_on();
	virtual void set_left_click_off();
	bool clickable_status()const;
	virtual void broadcast_current_state()const;
	virtual void key_input_cmd(j_window_t, int charcode, int scancode, int action, int modifier);
	virtual void char_input_cmd(j_window_t, j_ulint /*charcode*/){}
	virtual void mouse_button_press(int,int, Pen_Pos_FL_t);
	virtual void mouse_button_release(int,int, Pen_Pos_FL_t);
	virtual void set_mouse_button_press_callback(Mouse_Press_Func_t);
	virtual void set_mouse_button_release_callback(Mouse_Press_Func_t);
	void set_clickable_status(bool clickable);
	void toggle_n(int num_toggles);
	virtual ~J_UI_Object(){}
	virtual void toggle();
	bool toggle_status()const;
protected:
	J_UI_Object();
	
private:
	const j_uint M_ID;
	J_UI_Object_Shared_t M_parent;
#ifdef VS_2013
	bool M_toggled_status = false;
	bool M_left_click_on_flag = false;
	Mouse_Press_Func_t M_mouse_press_callback = nullptr;
	Mouse_Press_Func_t M_mouse_release_callback = nullptr;
	bool M_clickable_flag = false;
#else
	J_UI_Object& operator=(const J_UI_Object&);
	bool M_toggled_status;
	bool M_left_click_on_flag;
	Mouse_Press_Func_t M_mouse_press_callback;
	Mouse_Press_Func_t M_mouse_release_callback;
	bool M_clickable_flag;
	void default_initialization();
#endif

};



class J_UI_Box : public J_UI_Object, public J_Rectangle{
public:
	J_UI_Box(const J_Rectangle&);
	void broadcast_current_state()const;

	void set_y(j_float)override;
	void set_x(j_float)override;
	void set_width(j_float)override;
	void set_height(j_float)override;
private:

};

class J_UI_Circle : public J_UI_Object, public J_Circle{
public:
	J_UI_Circle(const J_Circle&);
	void broadcast_current_state()const;


	void set_x(j_float)override;
	void set_y(j_float)override;
	void set_radius(j_float)override;
	void set_center(j_float, j_float)override;

private:
	
};


}




#endif