#include "J_Camera_Notifications.h"
//
#include "J_Camera_Socket_Controller.h"
//
#include <J_UI/J_Text_Box.h>
//
#include "J_Camera_Stream.h"
//
#include <J_Camera_Manager.h>
//
#include <J_OpenGl.h>
//
#include <iostream>
//
#include <functional>
//
#include "pxcprojection.h"

using std::ostringstream; using std::cerr; using std::endl;
using std::cout; using std::wstring; using std::string; using std::transform;
using std::array; using std::min; using std::all_of;
using std::bind; using std::equal_to; using namespace std::placeholders;
namespace jomike{

static Instance_Pointer<J_Camera_Manager> s_camera_manager;
static Instance_Pointer<Contexts_Handler> s_contexts;


Face_Data_Handler
::Face_Data_Handler(array<J_UI_Box_Weak_t, MAX_FACES> i_boxes
, ex_array<ex_array<J_UI_Circle_Shared_t>> i_circles
, J_Camera_Stream_Weak_t i_stream_pane, J_Context_Shared_t i_context){
#ifndef VS_2013
	default_initialization();
#endif //!VS_2013
	M_face_boxes = i_boxes;
	M_context = i_context;
	M_image_pane = i_stream_pane;
	for(int i = 0; i < MAX_FACES; i++){
		for(int b = 0; b < NUM_LANDMARKS; b++){
			M_landmark_circles[i].push_back(i_circles[i][b]);
		}
	}
}

#ifndef VS_2013
void Face_Data_Handler::default_initialization(){
	M_landmark_circles
		= ex_array<ex_array<J_UI_Circle_Weak_t>>(MAX_FACES);
	M_mirror_status = false;
	M_width = 0;
}

#endif //!VS_2013

void Face_Data_Handler::operator()(Face_Data_Cont_t i_face_data
	, const Landmark_Data_Cont_t& irk_landmark_data, bool i_mirror_status){
	if(M_context.expired()){
		return;
	}
	M_mirror_status = i_mirror_status;

	assert(!M_image_pane.expired());
	assert(!M_context.expired());
	J_Camera_Stream_Shared_t stream_pane = M_image_pane.lock();
	M_width = stream_pane->image_width();
	auto context = M_context.lock();
	auto window = s_contexts->get_window(context);
	j_float ratio = stream_pane->width()
		/ to_x_screen(window, stream_pane->image_width());


	for(int i = 0; i < MAX_FACES; i++){
		assert(!M_face_boxes[i].expired());
		auto face_box = M_face_boxes[i].lock();

		if(!i_face_data[i].confidence){
			face_box->set_outline_and_fill_visibility_status(false);
			M_landmark_circles[i].apply([](J_UI_Circle_Weak_t y_circle){
				y_circle.lock()->set_outline_and_fill_visibility_status(false); });
			continue;
		}
		face_box->set_outline_and_fill_visibility_status(true);
		auto rectangle = i_face_data[i].rectangle;


		j_float width = ratio*to_x_screen(window, rectangle.w);
		j_float height = ratio*to_y_screen(window, rectangle.h);
		face_box->set_width(width);
		face_box->set_height(height);

		j_float x_pos = stream_pane->x1()
			+ ratio*to_x_screen(window, rectangle.x);
		x_pos = M_mirror_status ? stream_pane->x2() - (x_pos - stream_pane->x1()) - width
			: x_pos;

		face_box->set_x(x_pos);
		face_box->set_y(stream_pane->y1() - ratio*to_y_screen(window, rectangle.y) - height);

		auto& landmark_circles_data = irk_landmark_data[i];
		auto& landmark_circles = M_landmark_circles[i];
		for(int b = 0; b < NUM_LANDMARKS; b++){
			assert(!landmark_circles[b].expired());
			auto landmark_circle = landmark_circles[b].lock();
			
			if(b >= landmark_circles_data.size()){
				landmark_circle->set_outline_and_fill_visibility_status(false);
				continue;
			}
			auto landmark_circle_data = landmark_circles_data[b];
			const j_float minimum_radius = 0.003f;

			landmark_circle->set_outline_and_fill_visibility_status(true);
			j_float radius = min(minimum_radius, 2 * minimum_radius
				- minimum_radius*landmark_circle_data.position.z / 1000);
			landmark_circle->set_radius(radius);
			int x_pos = static_cast<int>(landmark_circle_data.position.x);
			x_pos = M_mirror_status ? M_width - x_pos : x_pos;
			int y_pos = static_cast<int>(landmark_circle_data.position.y);

			landmark_circle->set_x(stream_pane->x1() + ratio*to_x_screen(window, x_pos));
			landmark_circle->set_y(stream_pane->y1() - ratio*to_y_screen(window, y_pos));
			
		}
		
	}

}



void GeoNodes_Handler::operator()(PXC_GeoNode* i_primary_nodes
	, PXC_GeoNode* i_secondary_nodes, bool i_mirror_status){
	


	ex_array<ex_array<PXC_GeoNode>> geonodes;
#ifdef VS_2013
	geonodes.emplace_back(i_primary_nodes, i_primary_nodes + NUM_NODES_PER_GEO_SIDE);
	geonodes.emplace_back(i_secondary_nodes, i_secondary_nodes + NUM_NODES_PER_GEO_SIDE);
#else
	geonodes.push_back(
		ex_array<PXC_GeoNode>(i_primary_nodes, i_primary_nodes + NUM_NODES_PER_GEO_SIDE)
		);
	geonodes.push_back(
			ex_array<PXC_GeoNode>(i_secondary_nodes, i_secondary_nodes + NUM_NODES_PER_GEO_SIDE)
		);
#endif // VS_2013
	if(M_stream_pane.expired()){
		return;
	}
	auto  stream_obj = M_stream_pane.lock();
	int image_width = stream_obj->image_width();
	M_width = static_cast<j_float>(image_width);
	M_mirror_status = i_mirror_status;

	assert(!M_context.expired());
	auto window = s_contexts->get_window(M_context.lock());
	auto node_poses = get_geo_node_color_poses(geonodes);
	j_float pane_x_pos = stream_obj->x1();
	j_float pane_y_pos = stream_obj->y1();


	


	j_float ratio = stream_obj->width() / to_x_screen(window, image_width);

	for(int i = 0; i < NUM_GEO_SIDES; i++){
		for(int b = 0; b < NUM_NODES_PER_GEO_SIDE; b++){
			PXC_GeoNode& geo_node = geonodes[i][b];
			Pen_Pos_FL_t new_center = node_poses[i][b];
			assert(!M_geocircles[i][b].expired());
			auto ui_circle = M_geocircles[i][b].lock();
			assert(ui_circle);
			if(!geo_node.body){
				ui_circle->set_outline_visibility_status(false);
				ui_circle->set_fill_visibility_status(false);
				continue;
			}



			ui_circle->set_center(pane_x_pos + ratio*to_x_screen(window, static_cast<int>(new_center.first))
				, pane_y_pos - ratio*to_y_screen(window, static_cast<int>(new_center.second)));
			ui_circle->set_outline_visibility_status(true);
			ui_circle->set_fill_visibility_status(true);

			//ui_circle->set_radius(geo_node.radiusImage);
		}
	}

}



ex_array<ex_array<Pen_Pos_FL_t>> GeoNodes_Handler
	::get_geo_node_color_poses(const ex_array<ex_array<PXC_GeoNode>>& i_geonodes)const{
	ex_array<ex_array<PXCPointF32>>
		color_poses(NUM_GEO_SIDES, ex_array<PXCPointF32>(NUM_NODES_PER_GEO_SIDE));

	assert(NUM_GEO_SIDES == i_geonodes.size());
	assert(all_of(i_geonodes.begin(), i_geonodes.end()
		, bind(equal_to<j_size_t>(), bind(&ex_array<PXC_GeoNode>::size, _1), NUM_NODES_PER_GEO_SIDE)));

	ex_array<ex_array<PXCPoint3DF32>>
		depth_poses(NUM_GEO_SIDES, ex_array<PXCPoint3DF32>(NUM_NODES_PER_GEO_SIDE));
	auto depth_stream_processor = s_camera_manager->get_depth_stream_depth_processor();

	for(int i = 0; i < NUM_GEO_SIDES; i++){
		for(int b = 0; b < NUM_NODES_PER_GEO_SIDE; b++){
			PXCPoint3DF32& depth_pos_r = depth_poses[i][b];
			depth_pos_r = i_geonodes[i][b].positionImage;
			depth_stream_processor->fill_depth_value(&depth_pos_r);
		}
	}

	for(int i = 0; i < NUM_GEO_SIDES; i++){
		s_camera_manager->get_depth_stream().device().projector()
			->MapDepthToColorCoordinates(NUM_NODES_PER_GEO_SIDE, depth_poses[i].data()
			, color_poses[i].data());
	}
	ex_array<ex_array<Pen_Pos_FL_t>>
		return_poses(NUM_GEO_SIDES, ex_array<Pen_Pos_FL_t>(NUM_NODES_PER_GEO_SIDE));

	for(int i = 0; i < NUM_GEO_SIDES; i++){
		transform(color_poses[i].begin(), color_poses[i].end(), return_poses[i].begin()
			, [&](PXCPointF32& i_pos){
				j_float x_pos = M_mirror_status ? M_width - i_pos.x : i_pos.x;
				return Pen_Pos_FL_t(x_pos, i_pos.y); });
	}
	
	return return_poses;
}


Pen_Pos_FL_t GeoNodes_Handler::get_screen_coords_from_relative
(J_Camera_Stream_Shared_t i_pane, PXC_GeoNode i_geo_node){
	//Pen_Pos_FL_t image_pane_pixel_pos;
		//= depth_pane_to_image_pane_pixel_pos(Pen_Pos_FL_t(i_geo_node.positionImage.x
		//, i_geo_node.positionImage.y));
	assert(!M_context.expired());
	J_Context_Shared_t context = M_context.lock();
	auto window = s_contexts->get_window(context);



	//image_pane_pixel_pos.first = image_pane_pixel_pos.first
	//	*(i_pane->width() / to_x_screen(window, static_cast<int>(s_camera_manager->image_width())));

	//image_pane_pixel_pos.second = image_pane_pixel_pos.second
	//	*(i_pane->height() / to_y_screen(window, static_cast<int>(s_camera_manager->image_height())));

	int image_pane_x = static_cast<int>(i_geo_node.positionImage.x);
	int image_pane_y = static_cast<int>(i_geo_node.positionImage.y);

	auto& depth_stream = s_camera_manager->get_depth_stream();

	Pen_Pos_FL_t screen_coordinates;
	j_float ratio = i_pane->width()
		/ to_x_screen(window, depth_stream.width());

	screen_coordinates.first = i_pane->x1() + ratio*to_x_screen(window, image_pane_x);
	screen_coordinates.second = i_pane->y1() - ratio*to_y_screen(window, image_pane_y);

	return screen_coordinates;

}

Pen_Pos_FL_t GeoNodes_Handler::depth_pane_to_image_pane_pixel_pos(Pen_Pos_FL_t i_pen_pos){
	assert(!M_stream_pane.expired());
	auto stream_pane = M_stream_pane.lock();
	auto& depth_stream = s_camera_manager->get_depth_stream();
	auto& color_stream = s_camera_manager->get_color_stream();
	j_float depth_width = static_cast<j_float>(depth_stream.width());
	j_float depth_height = static_cast<j_float>(depth_stream.height());
	j_float image_width = static_cast<j_float>(color_stream.width());
	j_float image_height = static_cast<j_float>(color_stream.height());


	j_float ratio = min(image_width / depth_width, image_height / depth_height);

	bool image_wider = (abs(image_width - (ratio*depth_width))
		>= abs(image_height - (ratio*depth_height)));

	bool ratio_is_four_three = s_camera_manager->get_aspect_ratio() == (4.0f / 3.0f);
	assert(ratio_is_four_three);


	j_float x_ratio_offset, y_ratio_offset;

	x_ratio_offset= y_ratio_offset = ratio_is_four_three
		? 0.0f : 1.6f;

	j_float x_offset = ratio_is_four_three ? 0.0f : -10.0f;

	i_pen_pos.first += x_offset;


	if(image_wider){
		i_pen_pos.second *= (ratio + y_ratio_offset);
		i_pen_pos.second -= ratio_is_four_three ? 0.0f : 180.0f;
		i_pen_pos.first *= (ratio + x_ratio_offset);
		i_pen_pos.first -= ratio_is_four_three ? 0.0f : 3.0f;

	} else{
		assert(0);
		i_pen_pos.first *= ratio;
		j_float padding = (image_height - depth_height*ratio) / 2.0f;
		i_pen_pos.second = padding + i_pen_pos.second*ratio;
	}
	return i_pen_pos;
}

GeoNodes_Handler::GeoNodes_Handler(GeoNode_Multi_Array_t i_array
	, J_Camera_Stream_Weak_t i_stream_pane, J_Context_Weak_t i_context){
	M_geocircles = i_array;
	M_context = i_context;
	M_stream_pane = i_stream_pane;
}




Color_Stream_Notification::Color_Stream_Notification(J_Camera_Stream_Weak_t i_pane){
	M_stream_pane = i_pane;
}

void Color_Stream_Notification::operator()(j_ubyte* i_buffer
	, Image_Format i_format, int i_width, int i_height){
	assert(!M_stream_pane.expired());
	J_Camera_Stream_Shared_t stream_pane = M_stream_pane.lock();

	assert(i_width >= 0);
	assert(i_height >= 0);
	stream_pane->set_image_width(i_width);
	stream_pane->set_image_height(i_height);
	stream_pane->set_buffer(i_buffer);
	stream_pane->set_input_format(i_format);
}


Depth_Stream_Notification::Depth_Stream_Notification(J_Camera_Stream_Weak_t i_pane){
	M_stream_pane = i_pane;
}

void Depth_Stream_Notification::operator()(j_ubyte* i_buffer, int i_width, int i_height){
	assert(!M_stream_pane.expired());
	J_Camera_Stream_Shared_t stream_pane = M_stream_pane.lock();

	assert(i_width >= 0);
	assert(i_height >= 0);
	stream_pane->set_input_format(Image_Format::RGBA32_UBYTE);
	stream_pane->set_image_width(i_width);
	stream_pane->set_image_height(i_height);
	stream_pane->set_buffer(i_buffer);
}



}



