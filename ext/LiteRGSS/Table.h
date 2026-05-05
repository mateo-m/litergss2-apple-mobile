#ifndef Table_H
#define Table_H

#include "RubyValue.h"
#include "rbAdapter.h"

extern VALUE rb_cTable;
void Init_Table();

struct rb_Table_Struct_Header {
	unsigned int dim = 0u;
	unsigned int xsize = 0u;
	unsigned int ysize = 0u;
	unsigned int zsize = 0u;
	unsigned int data_size = 0u;
};

struct rb_Table_Struct {
	rb_Table_Struct_Header header {};
	short* heap = nullptr;
};

template<>
void rb::Mark<rb_Table_Struct>(void* ptr);

template<>
void rb::Free<rb_Table_Struct>(void* data);

#endif
