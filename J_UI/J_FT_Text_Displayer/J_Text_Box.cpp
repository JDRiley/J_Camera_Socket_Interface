#include "J_Text_Box.h"
#include "../J_UI_Model.h"
#include "../../J_Utile/J_Utile.h"
//Algorithms
#include <algorithm>
#include <functional>
using std::any_of; using std::bind; using std::equal_to; using std::mem_fn; using std::not1;
using std::remove_if; using std::transform;

using namespace std::placeholders;

//Containers


#include <iostream>
using std::cerr;

//Utilities
#include <cassert>

namespace jomike{

static Instance_Pointer<J_UI_Model> s_ui_model;

j_dbl DEFAULT_CURSOR_REFESH_TIME = 1.0/1.5;
J_Text_Box::J_Text_Box(const J_Rectangle& irk_rec_pos, const J_UI_Multi_String& irk_string)
		: J_UI_Box(irk_rec_pos), M_multi_string(irk_string), M_cursor_pos(static_cast<int>(M_multi_string.size()))
		, M_curser_on_switch(false)
		, M_last_cursor_switch(0.0), M_cursor_switch_time(DEFAULT_CURSOR_REFESH_TIME){

	M_cursor_color = J_Color::Clear;
	M_left_click_is_on = false;
	M_text_state = Text_Flags::SCROLL;		
}

bool J_Text_Box::insert_char(J_UI_Char i_char){
	M_multi_string.insert(M_cursor_pos, i_char);

	s_ui_model->notify_char_add(get_ID(), M_cursor_pos, i_char);
	
	move_cursor(1);
	set_cursor_on();

	return true;
}


/*void insert_string(const J_UI_String&)*/
void J_Text_Box::insert_string(const J_UI_String& irk_string){
	insert_string(get_cursor_pos(), irk_string);
}

/*void insert_string(int pos, const J_UI_String&)*/
void J_Text_Box::insert_string(j_size_t i_pos, const J_UI_String& irk_string){
	set_cursor_pos(i_pos);

	for_each(irk_string.begin(), irk_string.end(),
		bind(&J_Text_Box::insert_char, this, _1));
}


void J_Text_Box::set_string(const J_UI_String& irk_string){

	M_multi_string = J_UI_Multi_String(irk_string);

	s_ui_model->notify_text_string(get_ID(), irk_string);
	set_cursor_on();
	M_cursor_pos = static_cast<int>(irk_string.size());
}

void J_Text_Box::move_cursor(j_size_t i_amt){
	j_size_t new_cursor =  M_cursor_pos + i_amt;
	if(new_cursor < 0){
		M_cursor_pos = 0;
	}else if(new_cursor > M_multi_string.size()){
		M_cursor_pos = static_cast<int>(M_multi_string.size());
	}else{
		M_cursor_pos = new_cursor;
	}
	s_ui_model->notify_text_cursor(get_ID(), M_cursor_pos);
	set_cursor_on();
	
}

void J_Text_Box::set_cursor_pos(j_size_t i_cursor_pos){
	assert(between_inclusive(i_cursor_pos, J_SIZE_T_ZERO, M_multi_string.size()));
		
	M_cursor_pos = i_cursor_pos;
	s_ui_model->notify_text_cursor(get_ID(), M_cursor_pos);
	set_cursor_on();
}

void J_Text_Box::set_cursor_color(const J_Color_RGBA<j_float> i_color){
	M_cursor_color = i_color;
	set_cursor_on();
	s_ui_model->notify_text_cursor_color(get_ID(), i_color);
}

void J_Text_Box::update(){
	j_dbl time = j_get_time();
	if((time - M_last_cursor_switch) > M_cursor_switch_time){
		//cerr << '\n' << time << " Last Cursor: " << M_last_cursor_switch << " Switch Time: " << M_cursor_switch_time;
		J_Color_RGBA<j_float> color = !M_curser_on_switch ? M_cursor_color : J_Color::Clear;
		s_ui_model->notify_text_cursor_color(get_ID(), color);
		M_curser_on_switch = !M_curser_on_switch;
		M_last_cursor_switch = time;
		
	}
	
}

void J_Text_Box::backspace(){
	if(0 == M_cursor_pos){
		return;
	}

	M_multi_string.erase(--M_cursor_pos, 1);

	s_ui_model->notify_char_delete(get_ID(), M_cursor_pos);
	set_cursor_on();
}

void J_Text_Box::delete_char(){
	if(M_multi_string.size() == M_cursor_pos){
		return;
	}

	M_multi_string.erase(M_cursor_pos,1);

	s_ui_model->notify_char_delete(get_ID(), M_cursor_pos);
	set_cursor_on();
}

/*void erase_chars(int pos, int size)*/
void J_Text_Box::erase_chars(j_size_t i_pos, j_size_t i_size){
	set_cursor_pos(i_pos);

	M_multi_string.erase(i_pos, i_size);
	s_ui_model->notify_erase_chars(get_ID(), M_cursor_pos, i_size);
}

void J_Text_Box::set_cursor_on(){
	j_dbl time = j_get_time();
	M_curser_on_switch = true;
	M_last_cursor_switch = time- M_cursor_switch_time*0.5;
	s_ui_model->notify_text_cursor_color(get_ID(), M_cursor_color);
}


void J_Text_Box::set_left_click_on(){
	M_left_click_is_on = true;
	M_saved_outline_visibility_status = outline_visibility_status();
	set_outline_visibility_status(true);
}

void J_Text_Box::set_left_click_off(){
	M_left_click_is_on = false;
	set_outline_visibility_status(M_saved_outline_visibility_status);
}

void J_Text_Box::broadcast_current_state()const{
	J_UI_Box::broadcast_current_state();
	s_ui_model->notify_text_string(get_ID(), M_multi_string);
	s_ui_model->notify_clickable_status(get_ID(), clickable_status());
	s_ui_model->notify_text_cursor(get_ID(), M_cursor_pos);
	s_ui_model->notify_text_cursor_color(get_ID(), M_cursor_color);

}

void J_Text_Box::standard_text_box_input_parser(int i_key, int , int i_action, int ){
	
	if((J_PRESS != i_action) && (J_REPEAT != i_action)){
		return;
	}

	if(read_only_status()){
		return;
	}

	switch(i_key){
	case J_KEY_BACKSPACE:
		backspace();
		break;
	case J_KEY_TAB:
		insert_char('\t');
		break;
	case J_KEY_ENTER:
		insert_char('\n');
		break;
	case J_KEY_LEFT:
		move_cursor(-1);
		break;
	case J_KEY_RIGHT:
		move_cursor(1);
		break;
	case J_KEY_DELETE:
		delete_char();
		break;
	default:
		;
	}
}


void J_Text_Box::key_input_cmd(j_window_t , int i_key, int i_scancode, int i_action, int i_modifiers){
	
	if((i_action != J_REPEAT) && (i_action != J_PRESS )){
		return;
	}
	standard_text_box_input_parser(i_key, i_scancode, i_action, i_modifiers);
}

void J_Text_Box::char_input_cmd(j_window_t , j_ulint i_charcode){
	if(read_only_status()){
		return;
	}
	insert_char(i_charcode);
}

/*void set_read_only_status(bool)*/
void J_Text_Box::set_read_only_status(bool i_status){
	set_clickable_status(!i_status);
	M_text_state = i_status ? (M_text_state & ~WRITE) : (M_text_state | WRITE);
}

void J_Text_Box::set_colors(J_Color_RGBA<j_float> i_cursor_color
			, J_Color_RGBA<j_float> i_fill_color, J_Color_RGBA<j_float> i_outline_color){

	set_cursor_color(i_cursor_color);
	set_fill_color(i_fill_color);
	set_outline_color(i_outline_color);
}

/*bool read_only_status()*/
bool J_Text_Box::read_only_status()const{
	return !(M_text_state & WRITE);
}

const J_UI_Multi_String& J_Text_Box::get_string()const{
	return M_multi_string;
}

void J_Text_Box::change_color_at_pos(int i_pos, J_UI_Color i_color){
	M_multi_string.get_string_holding_index(i_pos)->set_color(i_color);
	s_ui_model->notify_text_string(get_ID(), M_multi_string);
}

}