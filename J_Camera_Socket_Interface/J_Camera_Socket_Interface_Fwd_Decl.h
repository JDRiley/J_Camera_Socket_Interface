#ifndef J_CAMERA_SOCKET_Interface_Fwd_Decl_H
#define J_CAMERA_SOCKET_Interface_Fwd_Decl_H

#include <J_Fwd_Decl.h>

namespace web{
	namespace json{ class value; }
}

namespace jomike{
typedef web::json::value json_val;

J_FWD_DECL(Gesture_Handler)
J_FWD_DECL(Face_Data_Handler)
J_FWD_DECL(GeoNodes_Handler)
J_FWD_DECL(Depth_Stream_Notification)
J_FWD_DECL(Color_Stream_Notification)
}


#endif