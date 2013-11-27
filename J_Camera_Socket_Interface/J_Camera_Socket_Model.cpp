#include "J_Camera_Socket_Model.h"
#include <J_Camera_Stream.h>

namespace jomike{

J_Camera_Socket_Model& J_Camera_Socket_Model::get_instance(){
	static J_Camera_Socket_Model model;
	return model;
}



void J_Camera_Socket_Model::add_camera_stream(J_Camera_Stream_Shared_t i_camera_stream_ptr){
	add_image_pane(i_camera_stream_ptr);

	assert(!M_camera_streams.count(i_camera_stream_ptr->get_ID()));
	M_camera_streams[i_camera_stream_ptr->get_ID()] = i_camera_stream_ptr;

}

J_Camera_Stream_Shared_t J_Camera_Socket_Model::get_camera_stream(j_uint i_id){
	if (!M_camera_streams.count(i_id)){
		throw J_Argument_Error("No Camera Stream With That ID");
	}

	return M_camera_streams[i_id];
}

void J_Camera_Socket_Model::clear_data(){
	J_UI_Model::clear_data();
	M_camera_streams.clear();
}

}