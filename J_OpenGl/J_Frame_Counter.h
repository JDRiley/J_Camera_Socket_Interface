#ifndef J_FRAME_COUNTER_H
#define J_FRAME_COUNTER_H


#include "J_OpenGL_Fwd_Decl.h"

namespace jomike{

class J_Frame_Counter{
public:
	J_Frame_Counter(int frame_threshold = 60);
	void count_frame();
	j_dbl fps()const;
private:
	int M_frame_count;
	int M_frame_threshold;
	j_dbl M_time_since_last_count;
	j_dbl M_fps;

};


}

#endif