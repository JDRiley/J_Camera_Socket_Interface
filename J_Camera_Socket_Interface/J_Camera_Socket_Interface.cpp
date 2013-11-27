#include "J_Camera_Socket_Controller.h"
//
#include <J_Utile.h>
//
#include <J_Error.h>

#if WIN32

#ifdef ASDF //RELEASE_BUILD
int wWinMain(HINSTANCE, HINSTANCE, PTSTR, int){
#else 
int main(int, char**){
#endif

#else



int main(int, char**){
#endif



	jtl::Instance_Pointer<jtl::J_Camera_Socket_Controller> controller;
	try{
		controller->init();
		controller->execute();
	}
	catch (jtl::J_Finished_Exception&){
		controller->clear_all();
		return 0;
	}
	catch(jtl::J_Error& error){
		error.print();
#ifdef WIN32
		MessageBoxA(0, error.message(), "Error", MB_OK);
#endif
		return 1;
	}
	
	return 0;
}

