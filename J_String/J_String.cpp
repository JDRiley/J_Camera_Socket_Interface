#include "J_String.h"
#include <ex_array.h>
#include <J_UI/J_UI_String.h>
#include <j_type.h>
//Algorithm
#include <algorithm>

using std::find_first_of; using std::for_each;

//Containers
using std::string;

//IO Facilities

#include <fstream>


using std::ifstream;

//Utilities
#include <iterator>

using std::istreambuf_iterator;

namespace jomike{
string file_to_str(const string& irk_filename){

	
	ifstream file(irk_filename);
	assert(file);
	return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}







j_dbl read_double_and_advance(J_UI_String::const_iterator* i_string_pos, int i_max_len){
	auto string_pos = *i_string_pos;
	auto end_pos = *i_string_pos + i_max_len;
	assert(0);
	string new_string;
	while(string_pos != end_pos){
	
		if(string_pos->is_alpha_numeric() || ('.' == string_pos->charcode())){
			continue;
		}

	}
	char* end_char_pos;
	Dbl_t return_val = strtod(new_string.c_str(), &end_char_pos);
	*i_string_pos += safe_int_cast(end_char_pos - &new_string[0]);
	
	return return_val;
}


}