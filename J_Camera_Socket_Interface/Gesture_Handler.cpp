#include "Gesture_Handler.h"
//
#include <J_UI/J_Text_Box.h>
//
#include <iostream>

using std::ostringstream; using std::endl;
using std::wstring; using std::string;
using std::cerr;
namespace jomike{


Gesture_Handler::Gesture_Handler(J_Text_Box_Shared_t i_primary, J_Text_Box_Shared_t i_secondary
	, J_Font_Face i_font_face){
	M_font_face = i_font_face;
	M_primary_gesture_box = i_primary;
	M_secondary_gesture_box = i_secondary;
}

void Gesture_Handler::operator()(PXC_Gesture_Data i_data){
	ostringstream o_str;
	wstring gesture_string = get_gesture_name(i_data.label);
	o_str << string(gesture_string.begin(), gesture_string.end()) << '!';

	o_str << " Active value" << (i_data.active ? " true" : " false")
		<< " Confidence: " << i_data.confidence;

#ifdef GESTURE_PRINTING
	cerr << '\n' << o_str.str();
#endif

	assert(!M_primary_gesture_box.expired());
	assert(!M_secondary_gesture_box.expired());

	J_Text_Box_Shared_t text_box_ptr;

	switch(i_data.body){
	case PXC_GeoNode::LABEL_BODY_HAND_PRIMARY:
		text_box_ptr = M_primary_gesture_box.lock();
		break;
	case PXC_GeoNode::LABEL_BODY_HAND_SECONDARY:
		text_box_ptr = M_secondary_gesture_box.lock();
		break;
	default:
		cerr << "\nUnknown Body Part Retrieved";
		break;
	}
	if(!text_box_ptr){
		return;
	}
	if(!text_box_ptr->get_string().empty())
	text_box_ptr->set_string(
		J_UI_String(o_str.str(), M_font_face, J_Color::Black));
}



}



