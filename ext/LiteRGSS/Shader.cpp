#include "LiteRGSS.h"
#include "Shader.h"
#include "Texture_Bitmap.h"
#include "Tone.h"
#include "Color.h"

VALUE rb_cShader = Qnil;

VALUE rb_Shader_loadFromMemory(int argc, VALUE *argv, VALUE self) {
	VALUE arg1, arg2, arg3;
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_scan_args(argc, argv, "12", &arg1, &arg2, &arg3);
	rb_check_type(arg1, T_STRING);
	if (RTEST(arg2)) {
		if (rb_obj_is_kind_of(arg2, rb_cInteger) == Qtrue) {
			renderStates.data().loadShaderFromMemory(rb_string_value_cstr(&arg1), static_cast<sf::Shader::Type>(NUM2LONG(arg2)));
		} else {
			rb_check_type(arg2, T_STRING);
			if (argc == 2) {
				renderStates.data().loadShaderFromMemory(rb_string_value_cstr(&arg1), rb_string_value_cstr(&arg2));
			} else if (argc == 3) {
				rb_check_type(arg3, T_STRING);
				renderStates.data().loadShaderFromMemory(rb_string_value_cstr(&arg1), rb_string_value_cstr(&arg2), rb_string_value_cstr(&arg3));
			}
		}
	} else {
		renderStates.data().loadShaderFromMemory(rb_string_value_cstr(&arg1), sf::Shader::Type::Fragment);
	}
	return self;
}

VALUE rb_Shader_setFloatUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_check_type(name, T_STRING);
	if (rb_obj_is_kind_of(uniform, rb_cArray) == Qtrue) {
		long arylen = RARRAY_LEN(uniform);
		if (arylen == 4) {
			sf::Glsl::Vec4 vect4(NUM2DBL(rb_ary_entry(uniform, 0)), NUM2DBL(rb_ary_entry(uniform, 1)), NUM2DBL(rb_ary_entry(uniform, 2)), NUM2DBL(rb_ary_entry(uniform, 3)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect4);
		} else if (arylen == 3) {
			sf::Glsl::Vec3 vect3(NUM2DBL(rb_ary_entry(uniform, 0)), NUM2DBL(rb_ary_entry(uniform, 1)), NUM2DBL(rb_ary_entry(uniform, 2)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect3);
		} else if(arylen == 2) {
			sf::Glsl::Vec2 vect2(NUM2DBL(rb_ary_entry(uniform, 0)), NUM2DBL(rb_ary_entry(uniform, 1)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect2);
		}
	} else if (rb_obj_is_kind_of(uniform, rb_cColor) == Qtrue) {
		auto* color = rb::GetPtr<ColorElement>(uniform);
		if (color != nullptr) {
			auto& colorValue = color->getValue();
			sf::Glsl::Vec4 vect4(colorValue.r / 255.0, colorValue.g / 255.0, colorValue.b / 255.0, colorValue.a / 255.0);
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect4);
		}
	} else if (rb_obj_is_kind_of(uniform, rb_cTone) == Qtrue) {
		auto* tone = rb::GetPtr<ToneElement>(uniform);
		if (tone != nullptr) {
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), tone->getValue());
		}
	} else {
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), static_cast<float>(NUM2DBL(uniform)));
	}
	return self;
}

VALUE rb_Shader_setIntUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_check_type(name, T_STRING);
	if (rb_obj_is_kind_of(uniform, rb_cArray) == Qtrue) {
		long arylen = RARRAY_LEN(uniform);
		if (arylen == 4) {
			sf::Glsl::Ivec4 vect4(rb_num2long(rb_ary_entry(uniform, 0)), rb_num2long(rb_ary_entry(uniform, 1)), rb_num2long(rb_ary_entry(uniform, 2)), rb_num2long(rb_ary_entry(uniform, 3)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect4);
		} else if (arylen == 3) {
			sf::Glsl::Ivec3 vect3(rb_num2long(rb_ary_entry(uniform, 0)), rb_num2long(rb_ary_entry(uniform, 1)), rb_num2long(rb_ary_entry(uniform, 2)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect3);
		} else if(arylen == 2) {
			sf::Glsl::Ivec2 vect2(rb_num2long(rb_ary_entry(uniform, 0)), rb_num2long(rb_ary_entry(uniform, 1)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect2);
		}
	} else {
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), static_cast<int>(rb_num2long(uniform)));
	}
	return self;
}


VALUE rb_Shader_setBoolUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_check_type(name, T_STRING);
	if (rb_obj_is_kind_of(uniform, rb_cArray) == Qtrue) {
		long arylen = RARRAY_LEN(uniform);
		if (arylen == 4) {
			sf::Glsl::Bvec4 vect4(RTEST(rb_ary_entry(uniform, 0)), RTEST(rb_ary_entry(uniform, 1)), RTEST(rb_ary_entry(uniform, 2)), RTEST(rb_ary_entry(uniform, 3)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect4);
		} else if (arylen == 3) {
			sf::Glsl::Bvec3 vect3(RTEST(rb_ary_entry(uniform, 0)), RTEST(rb_ary_entry(uniform, 1)), RTEST(rb_ary_entry(uniform, 2)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect3);
		} else if(arylen == 2) {
			sf::Glsl::Bvec2 vect2(RTEST(rb_ary_entry(uniform, 0)), RTEST(rb_ary_entry(uniform, 1)));
			renderStates.data().setShaderUniform(rb_string_value_cstr(&name), vect2);
		}
	} else {
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), RTEST(uniform));
	}
	return self;
}

VALUE rb_Shader_setTextureUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_check_type(name, T_STRING);
	if (rb_obj_is_kind_of(uniform, rb_cBitmap) == Qtrue) {
		auto& bmp = rb::Get<TextureElement>(uniform);
		sf::Texture& texture = bmp->raw();
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), texture);
	} else {
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), sf::Shader::CurrentTexture);
	}
	return self;
}

VALUE rb_Shader_setMatrixUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	unsigned long i;
	rb_check_type(name, T_STRING);
	rb_check_type(uniform, T_ARRAY);
	long arylen = RARRAY_LEN(uniform);
	float floats[16];
	if (arylen == 9) {
		for (i = 0; i < 9; i++) {
			floats[i] = NUM2DBL(rb_ary_entry(uniform, i));
		}
		sf::Glsl::Mat3 matrix3(floats);
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), matrix3);
	} else if(arylen == 16) {
		for (i = 0; i < 16; i++) {
			floats[i] = NUM2DBL(rb_ary_entry(uniform, i));
		}
		sf::Glsl::Mat4 matrix4(floats);
		renderStates.data().setShaderUniform(rb_string_value_cstr(&name), matrix4);
	}

	return self;
}

VALUE rb_Shader_setFloatArrayUniform(VALUE self, VALUE name, VALUE uniform) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	rb_check_type(name, T_STRING);
	rb_check_type(uniform, T_ARRAY);
	unsigned int len = RARRAY_LEN(uniform);
	auto floats = std::vector<float>(len);
	for (unsigned int i = 0; i < len; i++) {
		VALUE val = rb_ary_entry(uniform, i);
		rb_check_type(val, T_FLOAT);
		floats[i] = static_cast<float>(NUM2DBL(val));
	}
	renderStates.data().setShaderUniformArray(rb_string_value_cstr(&name), &floats[0], len);
	return self;
}

VALUE rb_Shader_isAvailable(VALUE self)  {
	(void) self;
	return RenderStatesElement::areShadersEnabled() ? Qtrue : Qfalse;
}

VALUE rb_Shader_isGeometryAvailable(VALUE self) {
	(void) self;
	return RenderStatesElement::areGeometryShadersEnabled() ? Qtrue : Qfalse;
}

VALUE rb_Shader_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "Shaders cannot be cloned or duplicated.");
	return self;
}

void Init_Shader() {
	rb_cShader = rb_define_class_under(rb_mLiteRGSS, "Shader", rb_cBlendMode);
	rb_define_alloc_func(rb_cShader, rb::Alloc<RenderStatesElement>);
	rb_define_method(rb_cShader, "initialize", _rbf rb_Shader_loadFromMemory, -1);
	rb_define_method(rb_cShader, "load", _rbf rb_Shader_loadFromMemory, -1);
	rb_define_method(rb_cShader, "set_float_uniform", _rbf rb_Shader_setFloatUniform, 2);
	rb_define_method(rb_cShader, "set_int_uniform", _rbf rb_Shader_setIntUniform, 2);
	rb_define_method(rb_cShader, "set_bool_uniform", _rbf rb_Shader_setBoolUniform, 2);
	rb_define_method(rb_cShader, "set_texture_uniform", _rbf rb_Shader_setTextureUniform, 2);
	rb_define_method(rb_cShader, "set_matrix_uniform", _rbf rb_Shader_setMatrixUniform, 2);
	rb_define_method(rb_cShader, "set_float_array_uniform", _rbf rb_Shader_setFloatArrayUniform, 2);

	rb_define_method(rb_cShader, "clone", _rbf rb_Shader_Copy, 0);
	rb_define_method(rb_cShader, "dup", _rbf rb_Shader_Copy, 0);
	
	rb_define_singleton_method(rb_cShader, "is_geometry_available?", _rbf rb_Shader_isGeometryAvailable, 0);
	rb_define_singleton_method(rb_cShader, "available?", _rbf rb_Shader_isAvailable, 0);

	rb_define_const(rb_cShader, "Fragment", LONG2FIX(sf::Shader::Type::Fragment));
	rb_define_const(rb_cShader, "Vertex", LONG2FIX(sf::Shader::Type::Vertex));
	rb_define_const(rb_cShader, "Geometry", LONG2FIX(sf::Shader::Type::Geometry));
}
