#ifndef L_TABLE32_HEADER
#define L_TABLE32_HEADER

#include <cstdint>

#include "RubyValue.h"
extern VALUE rb_cTable32;
void Init_Table32();

struct rb_Table32_Struct_Header {
	unsigned int dim = 0u;
	unsigned int xsize = 0u;
	unsigned int ysize = 0u;
	unsigned int zsize = 0u;
	unsigned int data_size = 0u;
};

struct rb_Table32_Struct {
	rb_Table32_Struct_Header header{};
	int32_t* heap = nullptr;
};

#endif
