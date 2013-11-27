#ifndef J_CAMERA_SOCKET_MODEL_H
#define J_CAMERA_SOCKET_MODEL_H

#include "J_Camera_Socket_Interface_Fwd_Decl.h"
#include <J_UI/J_UI_Model.h>

#include <map>

namespace jomike{

class J_Camera_Socket_Model : public J_UI_Model{
public:
	virtual void add_camera_stream(J_Camera_Stream_Shared_t);
	virtual J_Camera_Stream_Shared_t get_camera_stream(j_uint stream_obj_id);
	void clear_data()override;
private:
	friend class Instance_Pointer<J_Camera_Socket_Model>;
	static J_Camera_Socket_Model& get_instance();


	typedef std::map<j_uint, J_Camera_Stream_Shared_t> J_Camera_Stream_Cont_t;
	J_Camera_Stream_Cont_t M_camera_streams;
};



	
}



#endif