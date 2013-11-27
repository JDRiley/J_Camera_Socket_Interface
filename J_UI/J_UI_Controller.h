#ifndef J_UI_CONTROLLER_H
#define J_UI_CONTROLLER_H

#include "J_UI_Fwd_Decl.h"
#include "../J_Utile/J_Fwd_Decl.h"

//
#include <map>
namespace jomike{

class J_UI_Controller{
public:
	J_UI_Controller();
	void init(int argc, char** argv);
	void initialize_text_data();
	static J_UI_Controller& get_instance();
	virtual void notify_text_box_release( j_uint text_obj_id, j_size_t cursor_index)=0;
	virtual void notify_text_box_press(J_View_Shared_t, j_uint text_obj_id, j_size_t cursor_index) =0;
	virtual ~J_UI_Controller();
	virtual void clear_all() = 0;
	virtual void add_view(J_View_Shared_t);
	virtual void remove_view(j_window_t);
	virtual bool has_views()const;
	virtual void draw_views()const;

private:
	virtual void derived_init(int argc, char** argv) = 0;
	typedef std::map<j_window_t, J_View_Shared_t> J_View_Cont_t;
	J_View_Cont_t M_j_views;
};


}




#endif