#ifndef J_CAMERA_NOTIFICATIONS_H
#define J_CAMERA_NOTIFICATIONS_H


#include "J_Camera_Socket_Interface_Fwd_Decl.h"
#include <J_UI/J_UI_Fwd_Decl.h> // For Circles and Boxes Declaration
#include <J_UI/J_Notification_Handler.h>
//
#include <J_PXC_Face.h>
//
#include <J_PXC_Gesture.h>
//
#include <J_Image.h>

namespace jomike{

typedef std::array<
			std::array<J_UI_Circle_Weak_t, NUM_NODES_PER_GEO_SIDE>
				, NUM_GEO_SIDES>
	GeoNode_Multi_Array_t;

class GeoNodes_Handler{
public:
	GeoNodes_Handler(GeoNode_Multi_Array_t, J_Camera_Stream_Weak_t, J_Context_Weak_t);
	void operator()(PXC_GeoNode*, PXC_GeoNode*, bool);
private:
	bool M_mirror_status;
	j_float M_width;
	Pen_Pos_FL_t get_screen_coords_from_relative
		(J_Camera_Stream_Shared_t i_pane, PXC_GeoNode i_geo_node);

	Pen_Pos_FL_t depth_pane_to_image_pane_pixel_pos(Pen_Pos_FL_t);
	Pen_Pos_FL_t get_color_stream_coordinates(J_Camera_Stream_Shared_t stream_obj
		, const PXC_GeoNode& geo_node);
	ex_array<ex_array<Pen_Pos_FL_t>> 
		get_geo_node_color_poses(const ex_array<ex_array<PXC_GeoNode>>& i_geonodes)const;
	void to_screen_coordinates(Pen_Pos_FL_t* new_center);
	J_Camera_Stream_Weak_t M_stream_pane;
	J_Context_Weak_t M_context;
	GeoNode_Multi_Array_t M_geocircles;
};




class Face_Data_Handler{
public:
	Face_Data_Handler(std::array<J_UI_Box_Weak_t, MAX_FACES>
		, ex_array<ex_array<J_UI_Circle_Shared_t>>i_circles
		, J_Camera_Stream_Weak_t, J_Context_Shared_t);
	void operator()(Face_Data_Cont_t, const Landmark_Data_Cont_t&, bool i_mirror_status);
private:
	std::array<J_UI_Box_Weak_t, MAX_FACES> M_face_boxes;
#ifdef VS_2013
	ex_array<ex_array<J_UI_Circle_Weak_t>> M_landmark_circles
		= ex_array<ex_array<J_UI_Circle_Weak_t>>(MAX_FACES);
	bool M_mirror_status = false;
	int M_width = 0;
#else
	ex_array<ex_array<J_UI_Circle_Weak_t>> M_landmark_circles;
	bool M_mirror_status;
	int M_width;
	void default_initialization();
#endif // VS_2013
	J_Camera_Stream_Weak_t M_image_pane;
	J_Context_Weak_t M_context;

};

class Color_Stream_Notification : public J_Notification_Handler{
public:
	Color_Stream_Notification(J_Camera_Stream_Weak_t);
	void operator()(j_ubyte* buffer, Image_Format, int width, int height);
private:
	J_Camera_Stream_Weak_t M_stream_pane;
};

class Depth_Stream_Notification : public J_Notification_Handler{
public:
	Depth_Stream_Notification(J_Camera_Stream_Weak_t);
	void operator()(j_ubyte* i_buffer, int i_width, int i_height);
private:
	J_Camera_Stream_Weak_t M_stream_pane;
};

}

#endif //J_CAMERA_NOTIFICATIONS_H

