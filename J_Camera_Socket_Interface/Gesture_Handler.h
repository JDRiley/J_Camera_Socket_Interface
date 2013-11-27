#ifndef GESTURE_HANDLER_H
#define GESTURE_HANDLER_H

#include "J_Camera_Socket_Interface_Fwd_Decl.h"
//
#include <J_ui/J_UI_Fwd_Decl.h>
//
#include <J_PXC_Gesture.h>
//
#include <J_Image.h>
namespace jomike{

class Gesture_Handler{
public:
	Gesture_Handler(J_Text_Box_Shared_t i_primary, J_Text_Box_Shared_t i_secondary
		, J_Font_Face);
	void operator()(PXC_Gesture_Data);
private:
	J_Text_Box_Weak_t M_primary_gesture_box, M_secondary_gesture_box;
	J_Font_Face M_font_face;
};



}

#endif //GESTURE_HANDLER_H

