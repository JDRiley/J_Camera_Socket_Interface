#ifndef J_UTILE_H
#define J_UTILE_H

#include <J_Fwd_Decl.h>

//IO Facilities
#include <iosfwd>

namespace jomike{


class True_Class{
public:
	operator bool(){ return true; }
};

extern True_Class j_true;

template<typename T>
bool between_inclusive(const T& irk_cand, const T& irk_low, const T& irk_high){
	return ((irk_low <= irk_cand) && (irk_high >= irk_cand));
}

class Stream_Settings_Preserver{
public:
	Stream_Settings_Preserver(std::ios&);
	~Stream_Settings_Preserver();
private:
	class Stream_Settings_Impl;
	std::unique_ptr<Stream_Settings_Impl> M_impl;
};

bool true_test(bool i_val);

template<class St>
class Instance_Pointer{
public:
	St* operator->();
	St& operator*();
};

template<class St>
St* Instance_Pointer<St>::operator->(){
	return &St::get_instance();
}

template<class St>
St& Instance_Pointer<St>::operator*(){return St::get_instance();}


template<typename St>
St zero_set(){
	St new_st;
	memset(&new_st, 0, sizeof(St));
	return new_st;
}

template<typename St>
void set_to_zero(St* i_target){
	memset(i_target, 0, sizeof(St));
}


template<typename Ret_t, typename Int_t>
typename std::enable_if<std::is_integral<Int_t>::value
	&& std::is_integral<Ret_t>::value, Ret_t>::type normalized_val(Int_t i_val){
	j_dbl ratio = static_cast<j_dbl>(i_val)
		/std::numeric_limits<Int_t>::max();

	return static_cast<Ret_t>(std::numeric_limits<Ret_t>::max()*ratio);
}

}

#endif