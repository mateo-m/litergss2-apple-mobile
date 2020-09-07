#include <iostream>
#include "rbAdapter.h"
#include "InputMappingTable.h"

VALUE rb_cInputMappingTable = Qnil;

void rb_InputMappingTable_Free(void* data) {
	delete reinterpret_cast<InputMappingTableElement*>(data);
}

VALUE rb_InputMappingTable_Alloc(VALUE klass) {
	return Data_Wrap_Struct(klass, NULL, rb_InputMappingTable_Free, new InputMappingTableElement());
}

VALUE rb_InputMappingTable_initialize(VALUE self, VALUE array) {
	return self;
}

VALUE rb_InputMappingTable_get(VALUE self, VALUE index) {
	auto& table = rb::Get<InputMappingTableElement>(self);
	VirtualKeyIndex vkey = NUM2ULONG(index);
	if (table.data == nullptr || vkey >= table.data->size()) {
		return Qnil;
	}
	return LONG2NUM((*table.data)[vkey]);
}

VALUE rb_InputMappingTable_set(int argc, VALUE* argv, VALUE self) {
	auto& table = rb::Get<InputMappingTableElement>(self);
	VALUE rindex, v;
	rb_scan_args(argc, argv, "2", &rindex, &v);
	VirtualKeyIndex vkey = NUM2ULONG(rindex);
	PhysicalKeyIndex pkey = NUM2LONG(v);
	if (table.data == nullptr || vkey >= table.data->size()) {
		return self;
	}
	(*table.data)[vkey] = pkey;
	return self;
}

VALUE rb_InputMappingTable_size(VALUE self) {
	auto& table = rb::Get<InputMappingTableElement>(self);
	return ULONG2NUM(table.data == nullptr ? 0 : table.data->size());
}

VALUE rb_InputMappingTable_last(VALUE self) {
	auto& table = rb::Get<InputMappingTableElement>(self);
	return LONG2NUM(table.data == nullptr ? -1 : table.data->back());
}

VALUE rb_InputMappingTable_clear(VALUE self) {
	auto& table = rb::Get<InputMappingTableElement>(self);
	if (table.data != nullptr) {
		table.data->clear();
	}
	return self;
}

VALUE rb_InputMappingTable_clone(VALUE self) {
  auto& table = rb::Get<InputMappingTableElement>(self);
  VALUE result = rb_ary_new();
  if (table.data == nullptr) {
    return result;
  }

  for (const auto& pkey : *table.data) {
    rb_ary_push(result, LONG2NUM(pkey));
  }

  return result;
}

VALUE rb_InputMappingTable_concat(int argc, VALUE *argv, VALUE self) {
  auto& table = rb::Get<InputMappingTableElement>(self);
  if (table.data == nullptr) {
    return self;
  }

  for (int i = 0; i < argc; i++) {
    const long maxLength = RARRAY_LEN(argv[i]);
    for (long j = 0; j < maxLength; j++) {
      table.data->push_back(NUM2LONG(rb_ary_entry(argv[i], j)));
    }
  }

  return self;
}

VALUE rb_InputMappingTable_each(int argc, VALUE *argv, VALUE self) {
  rb_need_block();

  auto& table = rb::Get<InputMappingTableElement>(self);
  if (table.data == nullptr) {
    return self;
  }

  for (const auto& pkey : *table.data) {
    rb_yield(LONG2NUM(pkey));
  }
  return self;
}

VALUE rb_InputMappingTable_find(VALUE self) {
  rb_need_block();

  auto& table = rb::Get<InputMappingTableElement>(self);
  if (table.data == nullptr) {
    std::cout << "NULL" << std::endl;
    return Qnil;
  }

  std::cout << "Not null, " << table.data->size() << " length" << std::endl;
  for (const auto& pkey : *table.data) {
    std::cout << pkey << std::endl;
    VALUE findRes = rb_yield(LONG2NUM(pkey));
    if (findRes == Qtrue) {
      return LONG2NUM(pkey);
    }
  }
  return Qnil;
}

VALUE rb_InputMappingTable_push(VALUE self, VALUE object) {
  auto& table = rb::Get<InputMappingTableElement>(self);
  if (table.data == nullptr) {
    return self;
  }
  PhysicalKeyIndex pkey = NUM2ULONG(object);
  table.data->push_back(std::move(pkey));
  return self;
}


void Init_InputMappingTable() {
	rb_cInputMappingTable = rb_define_class("InputMappingTable", rb_cObject);
	rb_define_alloc_func(rb_cInputMappingTable, rb_InputMappingTable_Alloc);
	rb_define_method(rb_cInputMappingTable, "initialize", _rbf rb_InputMappingTable_initialize, -1);
	rb_define_method(rb_cInputMappingTable, "[]", _rbf rb_InputMappingTable_get, 1);
	rb_define_method(rb_cInputMappingTable, "[]=", _rbf rb_InputMappingTable_set, -1);
	rb_define_method(rb_cInputMappingTable, "size", _rbf rb_InputMappingTable_size, 0);
	rb_define_method(rb_cInputMappingTable, "last", _rbf rb_InputMappingTable_last, 0);
	rb_define_method(rb_cInputMappingTable, "clear", _rbf rb_InputMappingTable_clear, 0);
	rb_define_method(rb_cInputMappingTable, "each", _rbf rb_InputMappingTable_each, -1);
  rb_define_method(rb_cInputMappingTable, "concat", _rbf rb_InputMappingTable_concat, -1);
  rb_define_method(rb_cInputMappingTable, "find", _rbf rb_InputMappingTable_find, 0);
  rb_define_method(rb_cInputMappingTable, "clone", _rbf rb_InputMappingTable_clone, 0);
  rb_define_method(rb_cInputMappingTable, "<<", _rbf rb_InputMappingTable_push, 1);
}
