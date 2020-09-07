#ifndef InputMappingTable_H
#define InputMappingTable_H

#include "RubyValue.h"
#include "BaseInputMapping.h"

struct InputMappingTableElement {
	std::vector<PhysicalKeyIndex>* data = nullptr;
};

void Init_InputMappingTable();

extern VALUE rb_cInputMappingTable;

#endif