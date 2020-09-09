#ifndef InputMappingTable_H
#define InputMappingTable_H

#include "RubyValue.h"
#include "InputMapping.h"
#include "KeyboardInputMapping.h"

struct InputMappingTableElement {
	VirtualKeyIndex vIndex = std::numeric_limits<VirtualKeyIndex>::max();
	InputMapping<KeyboardInputMapping>* data = nullptr;
};

void Init_InputMappingTable();

extern VALUE rb_cInputMappingTable;

#endif