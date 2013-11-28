#include "J_PXC_Face_Module.h"


namespace jomike{

static J_PXC_Error_Manager s_pxc_error;

pxcStatus PXCAPI J_Face_Module_Impl::CreateInstance(PXCSession* i_session
	, PXCScheduler*, PXCAccelerator*, PXCSessionService::DLLExportTable*
	, pxcUID , PXCBase **i_instance){
	*i_instance = new J_Face_Module_Impl(i_session);

	return PXC_STATUS_NO_ERROR;
}

J_Face_Module_Impl::J_Face_Module_Impl(PXC_Session i_session){
	s_pxc_error = i_session->CreateImpl<PXCFaceAnalysis>(&M_face_module); 
	
}

pxcStatus PXCAPI J_Face_Module_Impl::ProcessImageAsync(PXC_Image*, PXC_SyncPoint*){
	assert(!"Not Implemented");
}




}



