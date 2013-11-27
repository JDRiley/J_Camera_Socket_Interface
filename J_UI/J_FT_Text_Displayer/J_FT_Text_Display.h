#ifndef J_FT_TEXT_DISPLAY_H
#define J_FT_TEXT_DISPLAY_H
#include <J_Error.h>
#include <J_Image.h>
#include "../J_Display_Object/J_Display_Object.h"
#include "../J_UI_String/J_UI_String.h"
//Containers
#include <array>
#include <ex_array.h>
#include <utility>

#include <mutex>
namespace jomike{

class J_FT_Text_Display : public J_Display_Box{
public:
	J_FT_Text_Display(j_uint);
	
	void add_font_face(J_Font_Face);
	void add_char(J_UI_Char i_char);
	void insert_char(j_size_t i_pos, J_UI_Char i_char);
	void insert_chars(j_size_t i_pos, const J_UI_String&);
	void set_string(const J_UI_Multi_String& irk_source);
	void delete_char(j_size_t i_pos);
	void erase_chars(j_size_t i_start, j_size_t i_end);
	void add_string(const J_UI_String&);
	void add_multi_string(const J_UI_Multi_String&);
	void draw()const override;
	void alert_resize(int,int)override;
	void pop_back();
	void set_cursor_pos(j_size_t i_size);
	void set_cursor_color(J_Color_RGBA<j_float>);
	void mouse_button_press(J_View_Shared_t, int i_button, int modifiers, Pen_Pos_FL_t)override;
	void mouse_button_release(J_View_Shared_t, int i_button, int modifiers, Pen_Pos_FL_t)override;
	static J_Font_Face create_font_face(FT_Face, int point_size, int render_mode);
	j_size_t get_cursor_index(Pen_Pos_FL_t)const;
	static void delete_font_face(J_Font_Face);
	~J_FT_Text_Display();
private:
	//Render Settings
	J_Text_Shader_Program* M_shader;
	j_uint M_program_id;
	

	void draw_string(J_UI_Multi_String::const_iterator)const;

	J_UI_Multi_String M_string;
	ex_array<j_uint> M_vao_ids;
	ex_array<j_uint> M_vao_buffer_ids;
	ex_array<Pen_Pos_FL_t> M_pen_poses;
	int M_new_line_size;
	j_size_t M_cursor_pos;
	j_uint M_cursor_vao_id;
	j_uint M_cursor_buffer_id;
	
	void init_cursor_vao();
	
	void draw_to_buffer(j_size_t index)const;
	void draw_cursor()const;
	Pen_Pos_FL_t default_pen_pos()const;

	Pen_Pos_FL_t J_FT_Text_Display::calculate_pen_advance(
		Pen_Pos_FL_t i_cur_pen, int i_advance)const;
	void recalculate_text_positions();
	void clear();
	void clear_from(j_size_t pos);

	void delete_vaos();
	void delete_buffers();
	void delete_vaos_from(j_size_t pos);
	void delete_buffers_from(j_size_t pos);
	void add_new_vao_and_vbo(int);
	Pen_Pos_FL_t new_line_pen_pos(Pen_Pos_FL_t i_cur_pen)const;
	j_uint Test_cursor_buffer;
};





}

#endif