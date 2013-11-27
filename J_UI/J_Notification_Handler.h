#ifndef J_NOTIFICATION_HANDLER_H
#define J_NOTIFICATION_HANDLER_H

#include <J_Fwd_Decl.h>


namespace jomike{

class J_Notification_Handler{
public:
	J_Notification_Handler();
	j_uint get_ID();
private:
	const j_uint M_ID;
};

}

#endif //J_NOTIFICATION_HANDLER_H

