#include "J_Socket_Server.h"
//
#include <cpprest/json.h>
//
#include <J_Error.h>
//
#include <iostream>
//

using std::wcerr;
using std::cerr;
using std::bind;
using std::cout; using std::endl; using std::wcout;
using std::for_each;
namespace json = web::json;
using std::wstring; 
using std::pair; using std::make_pair;
namespace opcode = websocketpp::frame::opcode;
using std::string;


namespace jomike{

const j_dbl K_SYNC_TIME = 1.0 /25.0;

json_val get_json_data(PXC_Gesture_Data i_gesture){
	json_val j_val;
	j_val[L"side"] = json_val::string(get_side(i_gesture.body));
	j_val[L"group"] = json_val::string(L"arm");
	j_val[L"body"] = json::value::string(get_body_name(i_gesture.body));
	j_val[L"gesture"] = json::value::string(get_gesture_name(i_gesture.label));
	auto position_vector = json::value::element_vector();
	position_vector.push_back(make_pair(json::value::number(1), json::value::number(1)));
	position_vector.push_back(make_pair(json::value::number(1), json::value::number(1)));
	j_val[L"position"] = json::value::array(position_vector);
	j_val[L"active"] = json::value::boolean(i_gesture.active);
	return j_val;
}

string get_json_data_string(PXC_Gesture_Data i_gesture){
	auto json_string =  get_json_data(i_gesture).to_string();
	return string(json_string.begin(), json_string.end());
}

J_Socket_Server::J_Socket_Server(uint16_t i_port_number) :M_port_number(i_port_number){
#ifndef VS_2013
	default_initialization();
#endif //!VS_2013

	M_server.init_asio();

	// Register handler callbacks
	M_server.set_open_handler(boost::bind(&J_Socket_Server::on_open, this, ::_1));
	M_server.set_close_handler(boost::bind(&J_Socket_Server::on_close, this, ::_1));
	M_server.set_message_handler(boost::bind(&J_Socket_Server::on_message, this, ::_1, ::_2));
}

#ifndef VS_2013
void J_Socket_Server::default_initialization(){
	M_launched_flag = false;
}
#endif //!VS_2013

void J_Socket_Server::queue_json_data(const json_val& i_json_val){
	auto val_string = i_json_val.to_string();

	M_pending_messages.emplace_back(string(val_string.begin(), val_string.end()));

	cerr << "\nJson Val: " << M_pending_messages.back();

}

void J_Socket_Server::queue_string(const string& i_data){

	M_pending_messages.emplace_back(i_data);
#if SOCKET_PRINTING
	cerr << "\nQueued String: " << M_pending_messages.back();
#endif

}

J_Socket_Server::~J_Socket_Server(){
	for (auto& connection : M_connections){
		M_server.close(connection, websocketpp::close::status::going_away
			, "J_Socket_Server Ending");
	}

}

void J_Socket_Server::send_data(){
	if (M_pending_messages.empty()){
		return;
	}

	for (auto& message : M_pending_messages){
		send_message(message);
	}
	M_pending_messages.clear();
}

void J_Socket_Server::send_message(const string& irk_message){
	for (auto& connection : M_connections){
		M_server.send(connection, irk_message, opcode::text);
	}
}

void J_Socket_Server::on_open(M_Connection_Cont_t::value_type i_connection){
	static int s_connection_count = 0;
	//J_LOCK(M_connection_cont_mutex);
	assert(!M_connections.count(i_connection));
	auto insert_results = M_connections.insert(i_connection);
	cerr << "\nConnection Number: " << ++s_connection_count;
	assert(insert_results.second);
	(void)insert_results;

}

void J_Socket_Server::on_message(M_Connection_Cont_t::value_type i_connection, Server_t::message_ptr i_msg){
	auto con = M_server.get_con_from_hdl(i_connection);

	cout << "\nMessage from connection: " << con << "\nRecieved message : " << i_msg->get_payload()
		<< endl;
}

void J_Socket_Server::on_close(M_Connection_Cont_t::value_type i_connection){
	//J_LOCK(M_connection_cont_mutex);
	auto iter_pos = M_connections.find(i_connection);

	assert(iter_pos != M_connections.end());

	M_connections.erase(iter_pos);

}

void J_Socket_Server::launch_server(){
	if (M_launched_flag){
		cerr << "\nWarning server already launched";
		return;
	}
	
	M_server.listen(M_port_number);
#ifndef VS_2013
	VLDDisable();
#endif
	M_server.start_accept();
#ifndef VS_2013
	VLDEnable();
#endif
	M_launched_flag = true;
}

void J_Socket_Server::poll_events(){
	assert(M_launched_flag);
	M_server.poll();
}

void J_Socket_Server::close_server(){
	M_server.stop_listening();
	M_server.stop();

}

void J_Socket_Server::run_server(){
	M_server.listen(M_port_number);
	M_server.start_accept();
	try{
		M_server.run();
	}
	catch (J_Error& error){
		error.print();
		cout << "\nEnding Server";
	}
	

}

void J_Socket_Server::terminate(M_Connection_Cont_t::value_type){
	throw J_Error("End Server");
}

void J_Socket_Server::terminate_message(M_Connection_Cont_t::value_type, Server_t::message_ptr){
	throw J_Error("End Server");
}

Gesture_Socket_Notification::Gesture_Socket_Notification(J_Socket_Server_Shared_t i_socket){
	M_socket = i_socket;
}

void Gesture_Socket_Notification::operator()(PXC_Gesture_Data i_data){
	if(i_data.label != M_cur_gestures[i_data.body]){
		M_cur_gestures[i_data.body] = i_data.label;
		M_socket->queue_string(get_json_data_string(i_data));
		return;
	}

}

}

