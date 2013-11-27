#ifndef J_TEXT_H
#define J_TEXT_H

#include "../J_Image/J_Image.h"
#include "../J_UI_Fwd_Decl.h"
#include "../J_UI_Object.h"
//Container
#include <ex_array.h>
#include <J_UI/J_UI_String.h>

//Utilities
#include <memory>
#include <utility>



namespace jomike{

J_FWD_DECL(Text_Section)



class J_Text_Box : public J_UI_Box{
	public:
		J_Text_Box(const J_Rectangle&
			, const J_UI_Multi_String& i_string = J_UI_Multi_String());


		void add_font_face(J_Font_Face);
		//String Maintenance
		void clear_string();
		bool insert_char(J_UI_Char i_char);

		void insert_string(const J_UI_String&);
		void insert_string(j_size_t pos, const J_UI_String&);

		void backspace();
		
		void delete_char();
		void erase_chars(j_size_t pos, j_size_t size);

		void set_cursor_pos(j_size_t i_cursor_pos);
		void set_cursor_color(const J_Color_RGBA<j_float>);
		void set_string(const J_UI_String& irk_string);



		void set_left_click_on();
		void set_left_click_off();

		void update()override;
		//Cursor Maintenance
		void move_cursor(j_size_t);

		//UI Functionallity
		void key_input_cmd(j_window_t, int charcode, int scancode, int action, int modifier)override;
		void char_input_cmd(j_window_t, j_ulint charcode)override;

		//Box Maintenance
		void set_left_bound();
		void set_right_bound();
		void set_top_bound();
		void set_bottom_bound();

		//Class Maintenance
		void add_flags(unsigned i_flags){M_text_state |= i_flags;}
		void set_flags(unsigned i_flags){M_text_state = i_flags;}
		unsigned get_flags()const{return M_text_state;}

		enum Text_Flags{BOTTOM_FIXED = 1, RIGHT_FIXED = 2
			, SCROLL = 4, DISPLAY_BOX = 8, WRITE = 16};


		j_size_t get_cursor_pos()const{return M_cursor_pos;}

		void set_read_only_status(bool);
		bool read_only_status()const;

		void set_colors(J_Color_RGBA<j_float> cursor_color
			, J_Color_RGBA<j_float> box_color, J_Color_RGBA<j_float> outline_color);


		void change_color_at_pos(int i_pos, J_UI_Color i_color);
		//String Accessor
		const J_UI_Multi_String& get_string()const;
		void broadcast_current_state()const override;

protected:
	
	void standard_text_box_input_parser(int i_key, int i_scancode
									, int i_action, int i_modifiers);
private:
	
		int M_text_state;

		J_UI_Multi_String M_multi_string;

		j_size_t M_cursor_pos;
		J_Color_RGBA<j_float> M_text_color;
		J_Color_RGBA<j_float> M_cursor_color;
		bool M_curser_on_switch;
		j_dbl M_last_cursor_switch;
		j_dbl M_cursor_switch_time;
		void set_cursor_on();


		bool M_left_click_is_on;

		bool M_saved_outline_visibility_status;

		J_Text_Box& operator=(const J_Text_Box&);
};


}
#endif