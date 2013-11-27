#include "J_UI_Controller.h"
#include "J_Font_Manager.h"
//Algorithm
#include <algorithm>
#include <functional>
using std::bind; using std::for_each;
using namespace std::placeholders;
//
#include "J_View.h"
//
#include "J_UI_Model.h"


namespace jomike{

static Instance_Pointer<J_Font_Manager> s_font_manager;
static Instance_Pointer<J_UI_Model> s_ui_model;
void J_UI_Controller::init(){
	derived_init();
}

void J_UI_Controller::initialize_text_data(){
	s_font_manager->add_font("Fonts/times.ttf", "times");
	s_font_manager->add_font("Fonts/timesi.ttf", "times_italic");
	s_font_manager->add_font("Fonts/timesbd.ttf", "times_bold");
	s_font_manager->add_font("Fonts/timesbi.ttf", "times_bold_italic");
	s_font_manager->add_font("Fonts/machadomath.ttf", "math_symbols");
}

J_UI_Controller::~J_UI_Controller(){}

void J_UI_Controller::clear_all(){
	auto view_start = make_pair_iter(M_j_views.begin());
	auto view_end = make_pair_iter(M_j_views.end());

	for_each(view_start, view_end, bind(&J_UI_Model::remove_view, &*s_ui_model, _1));
	s_font_manager->clear();
	//for_each(view_start, view_end, mem_fn(&J_View::clear));
	M_j_views.clear();
}


void J_UI_Controller::add_view(J_View_Shared_t i_view){
	assert(!M_j_views.count(i_view->get_window()));

	M_j_views[i_view->get_window()] = i_view;
	s_ui_model->attach_view(i_view);
}

void J_UI_Controller::remove_view(j_window_t i_window){
	assert(M_j_views.count(i_window));
	s_ui_model->remove_view(M_j_views[i_window]);
	M_j_views.erase(i_window);
}

bool J_UI_Controller::has_views()const{
	return !M_j_views.empty();
}

void J_UI_Controller::draw_views()const{
	for (auto view : M_j_views){
		view.second->draw();
	}
}

J_UI_Controller::J_UI_Controller(){
	initialize_text_data();
}


}