#include "J_Utile.h"

//IO Facilities
#include <iostream>

using std::ios; using std::streamsize;

namespace jomike{

True_Class j_true;

bool true_test(bool i_val){ return i_val; }

class Stream_Settings_Preserver::Stream_Settings_Impl{
public:
	Stream_Settings_Impl(ios& ir_stream):M_stream(ir_stream){
		old_precision = ir_stream.precision();
		old_flags = ir_stream.flags();
	}
	~Stream_Settings_Impl(){
		M_stream.precision(old_precision);
		M_stream.flags(old_flags);
	}
private:
	ios& M_stream;
	streamsize old_precision;
	ios::fmtflags old_flags;
	

	
};

Stream_Settings_Preserver::Stream_Settings_Preserver(ios& ir_stream)
	:M_impl(new Stream_Settings_Impl(ir_stream)){}

Stream_Settings_Preserver::~Stream_Settings_Preserver(){}




}

