/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cstdlib>

#include "foundation/executable/library.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	template< typename signature >
		signature address( const c8* name )
	{
		signature function = 0;
		OOE_IGNORE( function = library::find< signature >( name, library::all ).function );

		if ( function )
			return function;

		throw error::runtime( "opengl: " ) << "Unable to find \"" << name << '\"';
	}
}

namespace ooe
{
	void opengl::symbol( void )
	{
		// core
		GetString = address< GetString_t >( "glGetString" );

		IsEnabled = address< IsEnabled_t >( "glIsEnabled" );
		Enable = address< Enable_t >( "glEnable" );
		Disable = address< Disable_t >( "glDisable" );
		EnableClientState = address< EnableClientState_t >( "glEnableClientState" );
		DisableClientState = address< DisableClientState_t >( "glDisableClientState" );
		EnableVertexAttribArray =
			address< EnableVertexAttribArray_t >( "glEnableVertexAttribArray" );
		DisableVertexAttribArray =
			address< DisableVertexAttribArray_t >( "glDisableVertexAttribArray" );

		Hint = address< Hint_t >( "glHint" );
		GetIntegerv = address< GetIntegerv_t >( "glGetIntegerv" );

		Clear = address< Clear_t >( "glClear" );
		ClearColor = address< ClearColor_t >( "glClearColor" );
		ClearDepth = address< ClearDepth_t >( "glClearDepth" );

		Viewport = address< Viewport_t >( "glViewport" );
		LineWidth = address< LineWidth_t >( "glLineWidth" );

		MatrixMode = address< MatrixMode_t >( "glMatrixMode" );
		LoadIdentity = address< LoadIdentity_t >( "glLoadIdentity" );
		LoadMatrixf = address< LoadMatrixf_t >( "glLoadMatrixf" );
		MultMatrixf = address< MultMatrixf_t >( "glMultMatrixf" );

		BlendEquation = address< BlendEquation_t >( "glBlendEquation" );
		BlendFunc = address< BlendFunc_t >( "glBlendFunc" );
		AlphaFunc = address< AlphaFunc_t >( "glAlphaFunc" );
		DepthFunc = address< DepthFunc_t >( "glDepthFunc" );

		VertexPointer = address< VertexPointer_t >( "glVertexPointer" );
		NormalPointer = address< NormalPointer_t >( "glNormalPointer" );
		ColorPointer = address< ColorPointer_t >( "glColorPointer" );
		TexCoordPointer = address< TexCoordPointer_t >( "glTexCoordPointer" );
		VertexAttribPointer = address< VertexAttribPointer_t >( "glVertexAttribPointer" );

		DrawElements = address< DrawElements_t >( "glDrawElements" );
		DrawArrays = address< DrawArrays_t >( "glDrawArrays" );

		// language
		DeleteShader = address< DeleteShader_t >( "glDeleteShader" );
		DetachShader = address< DetachShader_t >( "glDetachShader" );
		CreateShader = address< CreateShader_t >( "glCreateShader" );
		ShaderSource = address< ShaderSource_t >( "glShaderSource" );
		CompileShader = address< CompileShader_t >( "glCompileShader" );
		CreateProgram = address< CreateProgram_t >( "glCreateProgram" );
		AttachShader = address< AttachShader_t >( "glAttachShader" );
		LinkProgram = address< LinkProgram_t >( "glLinkProgram" );
		UseProgram = address< UseProgram_t >( "glUseProgram" );
		DeleteProgram = address< DeleteProgram_t >( "glDeleteProgram" );
		ValidateProgram = address< ValidateProgram_t >( "glValidateProgram" );
		GetShaderiv = address< GetShaderiv_t >( "glGetShaderiv" );
		GetProgramiv = address< GetProgramiv_t >( "glGetProgramiv" );
		GetShaderInfoLog = address< GetShaderInfoLog_t >( "glGetShaderInfoLog" );
		GetProgramInfoLog = address< GetProgramInfoLog_t >( "glGetProgramInfoLog" );
		GetUniformLocation = address< GetUniformLocation_t >( "glGetUniformLocation" );
		Uniform1fv = address< Uniform1fv_t >( "glUniform1fv" );
		Uniform2fv = address< Uniform2fv_t >( "glUniform2fv" );
		Uniform3fv = address< Uniform3fv_t >( "glUniform3fv" );
		Uniform4fv = address< Uniform4fv_t >( "glUniform4fv" );
		Uniform1iv = address< Uniform1iv_t >( "glUniform1iv" );
		Uniform2iv = address< Uniform2iv_t >( "glUniform2iv" );
		Uniform3iv = address< Uniform3iv_t >( "glUniform3iv" );
		Uniform4iv = address< Uniform4iv_t >( "glUniform4iv" );
		UniformMatrix2fv = address< UniformMatrix2fv_t >( "glUniformMatrix2fv" );
		UniformMatrix3fv = address< UniformMatrix3fv_t >( "glUniformMatrix3fv" );
		UniformMatrix4fv = address< UniformMatrix4fv_t >( "glUniformMatrix4fv" );
		GetAttribLocation = address< GetAttribLocation_t >( "glGetAttribLocation" );

		// buffer
		GenBuffers = address< GenBuffers_t >( "glGenBuffers" );
		DeleteBuffers = address< DeleteBuffers_t >( "glDeleteBuffers" );
		BindBuffer = address< BindBuffer_t >( "glBindBuffer" );
		BufferData = address< BufferData_t >( "glBufferData" );
		BufferSubData = address< BufferSubData_t >( "glBufferSubData" );
		MapBuffer = address< MapBuffer_t >( "glMapBuffer" );
		UnmapBuffer = address< UnmapBuffer_t >( "glUnmapBuffer" );

		// texture
		GenTextures = address< GenTextures_t >( "glGenTextures" );
		DeleteTextures = address< DeleteTextures_t >( "glDeleteTextures" );
		BindTexture = address< BindTexture_t >( "glBindTexture" );
		TexImage2D = address< TexImage2D_t >( "glTexImage2D" );
		TexSubImage2D = address< TexSubImage2D_t >( "glTexSubImage2D" );
		CompressedTexImage2D = address< CompressedTexImage2D_t >( "glCompressedTexImage2D" );
		CompressedTexSubImage2D =
			address< CompressedTexSubImage2D_t >( "glCompressedTexSubImage2D" );
		TexParameteri = address< TexParameteri_t >( "glTexParameteri" );
		TexEnvi = address< TexEnvi_t >( "glTexEnvi" );
		PixelStorei = address< PixelStorei_t >( "glPixelStorei" );
		ActiveTexture = address< ActiveTexture_t >( "glActiveTexture" );
		ClientActiveTexture = address< ClientActiveTexture_t >( "glClientActiveTexture" );

		// light
		Lightfv = address< Lightfv_t >( "glLightfv" );
	}

//--- function pointers --------------------------------------------------------
/*** gl *******************************/
	opengl::GetString_t opengl::GetString;

	opengl::IsEnabled_t opengl::IsEnabled;
	opengl::Enable_t opengl::Enable;
	opengl::Disable_t opengl::Disable;
	opengl::EnableClientState_t opengl::EnableClientState;
	opengl::DisableClientState_t opengl::DisableClientState;
	opengl::EnableVertexAttribArray_t opengl::EnableVertexAttribArray;
	opengl::DisableVertexAttribArray_t opengl::DisableVertexAttribArray;

	opengl::Hint_t opengl::Hint;
	opengl::GetIntegerv_t opengl::GetIntegerv;

	opengl::Clear_t opengl::Clear;
	opengl::ClearColor_t opengl::ClearColor;
	opengl::ClearDepth_t opengl::ClearDepth;

	opengl::Viewport_t opengl::Viewport;
	opengl::LineWidth_t opengl::LineWidth;

	opengl::MatrixMode_t opengl::MatrixMode;
	opengl::LoadIdentity_t opengl::LoadIdentity;
	opengl::LoadMatrixf_t opengl::LoadMatrixf;
	opengl::MultMatrixf_t opengl::MultMatrixf;

	opengl::BlendEquation_t opengl::BlendEquation;
	opengl::BlendFunc_t opengl::BlendFunc;
	opengl::AlphaFunc_t opengl::AlphaFunc;
	opengl::DepthFunc_t opengl::DepthFunc;

	opengl::VertexPointer_t opengl::VertexPointer;
	opengl::NormalPointer_t opengl::NormalPointer;
	opengl::ColorPointer_t opengl::ColorPointer;
	opengl::TexCoordPointer_t opengl::TexCoordPointer;
	opengl::VertexAttribPointer_t opengl::VertexAttribPointer;

	opengl::DrawElements_t opengl::DrawElements;
	opengl::DrawArrays_t opengl::DrawArrays;

/*** language *************************/
	opengl::DeleteShader_t opengl::DeleteShader;
	opengl::DetachShader_t opengl::DetachShader;
	opengl::CreateShader_t opengl::CreateShader;
	opengl::ShaderSource_t opengl::ShaderSource;
	opengl::CompileShader_t opengl::CompileShader;

	opengl::CreateProgram_t opengl::CreateProgram;
	opengl::AttachShader_t opengl::AttachShader;
	opengl::LinkProgram_t opengl::LinkProgram;
	opengl::UseProgram_t opengl::UseProgram;
	opengl::DeleteProgram_t opengl::DeleteProgram;
	opengl::ValidateProgram_t opengl::ValidateProgram;

	opengl::GetShaderiv_t opengl::GetShaderiv;
	opengl::GetProgramiv_t opengl::GetProgramiv;
	opengl::GetShaderInfoLog_t opengl::GetShaderInfoLog;
	opengl::GetProgramInfoLog_t opengl::GetProgramInfoLog;

	opengl::GetUniformLocation_t opengl::GetUniformLocation;
	opengl::Uniform1fv_t opengl::Uniform1fv;
	opengl::Uniform2fv_t opengl::Uniform2fv;
	opengl::Uniform3fv_t opengl::Uniform3fv;
	opengl::Uniform4fv_t opengl::Uniform4fv;
	opengl::Uniform1iv_t opengl::Uniform1iv;
	opengl::Uniform2iv_t opengl::Uniform2iv;
	opengl::Uniform3iv_t opengl::Uniform3iv;
	opengl::Uniform4iv_t opengl::Uniform4iv;
	opengl::UniformMatrix2fv_t opengl::UniformMatrix2fv;
	opengl::UniformMatrix3fv_t opengl::UniformMatrix3fv;
	opengl::UniformMatrix4fv_t opengl::UniformMatrix4fv;

	opengl::GetAttribLocation_t opengl::GetAttribLocation;

/*** buffer ***************************/
	opengl::GenBuffers_t opengl::GenBuffers;
	opengl::DeleteBuffers_t opengl::DeleteBuffers;
	opengl::BindBuffer_t opengl::BindBuffer;

	opengl::BufferData_t opengl::BufferData;
	opengl::BufferSubData_t opengl::BufferSubData;
	opengl::MapBuffer_t opengl::MapBuffer;
	opengl::UnmapBuffer_t opengl::UnmapBuffer;

/*** texture **************************/
	opengl::GenTextures_t opengl::GenTextures;
	opengl::DeleteTextures_t opengl::DeleteTextures;
	opengl::BindTexture_t opengl::BindTexture;
	opengl::TexImage2D_t opengl::TexImage2D;
	opengl::TexSubImage2D_t opengl::TexSubImage2D;
	opengl::CompressedTexImage2D_t opengl::CompressedTexImage2D;
	opengl::CompressedTexSubImage2D_t opengl::CompressedTexSubImage2D;
	opengl::TexParameteri_t opengl::TexParameteri;
	opengl::TexEnvi_t opengl::TexEnvi;
	opengl::PixelStorei_t opengl::PixelStorei;
	opengl::ActiveTexture_t opengl::ActiveTexture;
	opengl::ClientActiveTexture_t opengl::ClientActiveTexture;

/*** light ****************************/
	opengl::Lightfv_t opengl::Lightfv;
}
