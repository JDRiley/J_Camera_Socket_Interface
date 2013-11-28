#ifndef J_PXC_FACE_MODULE_H
#define J_PXC_FACE_MODULE_H


#include "J_PXC_Fwd_Decl.h"
//
#include "pxcbase.h"
#include "j_pxc_face.h"
#include "service/pxcsessionservice.h"
#include "pxcaccelerator.h"
#include "pxcsession.h"


J_POINTER_DECL(PXCScheduler::SyncPoint, PXC_SyncPoint)

namespace jomike{

namespace J_PXC_Face{

}




class J_Face_Module :public PXCBase{
public:
	PXC_CUID_OVERWRITE(PXC_UID('J', 'F', 'P', 'M'));

	virtual pxcStatus PXCAPI ProcessImageAsync(PXC_Image*, PXC_SyncPoint*) = 0;


private:
};

class J_Face_Module_Impl :public PXCBaseImpl<J_Face_Module> {
public:
	J_Face_Module_Impl(PXC_Session i_session);
	pxcStatus PXCAPI ProcessImageAsync(PXC_Image*, PXC_SyncPoint*)override;

	static pxcStatus PXCAPI CreateInstance(PXCSession*, PXCScheduler*, PXCAccelerator*
		, PXCSessionService::DLLExportTable*, pxcUID cuid, PXCBase **instance);
private:
	PXC_Face_Module M_face_module;
};

extern PXCSessionService::DLLExportTable __declspec(dllexport) g_j_pxc_face_dll_table;




}

#endif //J_PXC_FACE_MODULE_H

