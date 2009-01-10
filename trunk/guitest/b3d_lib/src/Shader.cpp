/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of Be3D library.
//
//    Be3D is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Be3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Be3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CgManager.h"
#include "BaseTextFile.h"
#include "TextureManager.h"
#include <direct.h>
#include "Texture.h"

Shader::Param::Param(const Shader::Param& p) : m_name(p.m_name), m_bSetOnce(p.m_bSetOnce), m_type(p.m_type)
{
	switch (p.m_type)
	{
	case Param::PARAM_DOUBLE1: m_pValue = new double(*(double*)p.m_pValue); break;
	case Param::PARAM_FLOAT1: m_pValue = new float(*(double*)p.m_pValue); break;
	case Param::PARAM_FLOAT2: m_pValue = new Vector2(*(Vector2*)p.m_pValue); break;
	case Param::PARAM_FLOAT3: m_pValue = new Vector3(*(Vector3*)p.m_pValue); break;
	case Param::PARAM_FLOAT4: m_pValue = new Vector4(*(Vector4*)p.m_pValue); break;
	case Param::PARAM_ARRAY_DOUBLE1: m_pValue = new std::vector<double>(*(std::vector<double>*)p.m_pValue); break;
	case Param::PARAM_ARRAY_FLOAT1: m_pValue = new std::vector<float>(*(std::vector<float>*)p.m_pValue); break;
	case Param::PARAM_ARRAY_FLOAT2: m_pValue = new std::vector<Vector2>(*(std::vector<Vector2>*)p.m_pValue); break;
	case Param::PARAM_ARRAY_FLOAT3: m_pValue = new std::vector<Vector3>(*(std::vector<Vector3>*)p.m_pValue); break;
	case Param::PARAM_ARRAY_FLOAT4: m_pValue = new std::vector<Vector4>(*(std::vector<Vector4>*)p.m_pValue); break;
	case Param::PARAM_MATRIX4: m_pValue = new Matrix4(*(Matrix4*)p.m_pValue); break;
	case Param::PARAM_SAMPLER1D: m_pValue = new (Texture*)(*(Texture**)p.m_pValue); break;
	case Param::PARAM_SAMPLER2D: m_pValue = new (Texture*)(*(Texture**)p.m_pValue); break;
	case Param::PARAM_SAMPLER3D: m_pValue = new (Texture3D*)(*(Texture3D**)p.m_pValue); break;
	case Param::PARAM_SAMPLERCUBE: m_pValue = new (CubeTexture*)(*(CubeTexture**)p.m_pValue); break;
	}
}
			

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::load(const std::string &filename, const std::string &mainFuncName, Shader::Type type)
{
	if (!ShaderManager::loadShader(this, filename, mainFuncName, type))
		return false;


	setShaderConstants();

	return true;
}

void Shader::set()
{
	cgGLBindProgram(m_program);

	setFrameConstants();

	
	// set all preset parameters here
	for (std::list<Param>::iterator it=m_params.begin(); it!=m_params.end(); ++it)
	{
		Param &param = *it;
		if (!param.m_bSetOnce)
			continue;

		switch (param.m_type)
		{
		case Param::PARAM_DOUBLE1:
			setParamd(param.m_name, *(double*)param.m_pValue);
			break;
		case Param::PARAM_FLOAT1:
			setParam1f(param.m_name, *(float*)param.m_pValue);
			break;
		case Param::PARAM_FLOAT2:
			setParam2f(param.m_name, *(Vector2*)param.m_pValue);
			break;
		case Param::PARAM_FLOAT3:
			setParam3f(param.m_name, *(Vector3*)param.m_pValue);
			break;
		case Param::PARAM_FLOAT4:
			setParam4f(param.m_name, *(Vector4*)param.m_pValue);
			break;
		case Param::PARAM_ARRAY_DOUBLE1: {
				std::vector<double> *pVals = (std::vector<double>*)param.m_pValue;
				setParamArray1d(param.m_name, &(*pVals)[0], pVals->size());
				break;
			}
		case Param::PARAM_ARRAY_FLOAT1: {
				std::vector<float> *pVals = (std::vector<float>*)param.m_pValue;
				setParamArray1f(param.m_name, &(*pVals)[0], pVals->size());
				break;
			}
		case Param::PARAM_ARRAY_FLOAT2: {
				std::vector<Vector2> *pVals = (std::vector<Vector2>*)param.m_pValue;
				setParamArray2f(param.m_name, &(*pVals)[0], pVals->size());
				break;
			}
		case Param::PARAM_ARRAY_FLOAT3: {
				std::vector<Vector3> *pVals = (std::vector<Vector3>*)param.m_pValue;
				setParamArray3f(param.m_name, &(*pVals)[0], pVals->size());
				break;
			}
		case Param::PARAM_ARRAY_FLOAT4: {
				std::vector<Vector4> *pVals = (std::vector<Vector4>*)param.m_pValue;
				setParamArray4f(param.m_name, &(*pVals)[0], pVals->size());
				break;
			}
		case Param::PARAM_MATRIX4:
			setParamMatrix4(param.m_name, *(Matrix4*)param.m_pValue);
			break;
		case Param::PARAM_SAMPLER1D:
			setSampler(param.m_name, (*(Texture**)param.m_pValue)->getGLTex());
			break;
		case Param::PARAM_SAMPLER2D:
			setSampler(param.m_name, (*(Texture**)param.m_pValue)->getGLTex());
			break;
		case Param::PARAM_SAMPLER3D:
			setSampler(param.m_name, (*(Texture3D**)param.m_pValue)->getGLTex());
			break;
		case Param::PARAM_SAMPLERCUBE:
			setSampler(param.m_name, (*(CubeTexture**)param.m_pValue)->getGLTex());
			break;
		}
	}
}

bool Shader::parseParamsAndLoad(BaseTextFile& file)
{
	// Load the shader and all shader params from a file
	// the format of the description of the shader should be:
	// [vert/frag]Shader "shader_path_and_file.cg" {
	//	   param_type param_name [opt]param_value;
	// }
	// param definitions can be multi-line
	std::string token;
	
	// read the shader type
	file >> token;
	if (token == "vertShader")
		m_type = Shader::VERTEX;
	else if (token == "fragShader")
		m_type = Shader::FRAGMENT;
	else
	{
		Console::print("\terror parsing shader desc: unknown shader type '%s'\n", token);
		return false;
	}

	// read the shader main func name
	std::string shaderMainFunc;
	file >> shaderMainFunc;
	std::string shaderFilename;
	file >> shaderFilename;

	// add the current dir to the shader filename if needed
	char *curDir = getcwd(0,0);
	shaderFilename = "\\" + shaderFilename;	// PLATFORM-DEPENDENT
	shaderFilename = curDir + shaderFilename;
	free(curDir);

	// '{' follows
	file >> token;
	if (token != "{")
	{
		Console::print("\terror parsing shader desc: '{' expected.\n");
		return false;
	}

	// now start parsing parameters, until we find a '}' which means
	// the end of the parameter list
	std::string paramType;
	std::string paramName;
	while (!file.eof())
	{
		file >> paramType;
		if (paramType == "}")
			break;	// end of parameter list
		
		file >> paramName;
		file >> token;
		Param param(paramName);

		// parse the parameter desc for each param type
		if (paramType == "float")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_FLOAT1);
			else {
				param.set((float)atof(token.c_str()));
				file >> token;	// read the trailing ';'
			}
		}
		else if (paramType == "double")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_DOUBLE1);
			else {
				param.set((double)atof(token.c_str()));
				file >> token;
			}
		}
		else if (paramType == "vec2f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_FLOAT2);
			else {
				Vector2 val;
				val.x = atof(token.c_str());
				file >> token;
				val.y = atof(token.c_str());
				file >> token;	// trailing ';'
				param.set(val);
			}
		}
		else if (paramType == "vec3f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_FLOAT3);
			else {
				Vector3 val;
				val.x = atof(token.c_str());
				file >> token;
				val.y = atof(token.c_str());
				file >> token;
				val.z = atof(token.c_str());
				file >> token;	// trailing ';'
				param.set(val);
			}
		}
		else if (paramType == "vec4f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_FLOAT4);
			else {
				Vector4 val;
				val.x = atof(token.c_str());
				file >> token;
				val.y = atof(token.c_str());
				file >> token;
				val.z = atof(token.c_str());
				file >> token;
				val.w = atof(token.c_str());
				file >> token;	// trailing ';'
				param.set(val);
			}
		}
		else if (paramType == "array1f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_FLOAT1);
			else {
				std::vector<float> val;
				while (token != ";") {
					val.push_back(atof(token.c_str()));
					file >> token;
				}
				param.set(val);
			}
		}
		else if (paramType == "array1d")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_DOUBLE1);
			else {
				std::vector<double> val;
				while (token != ";") {
					val.push_back(atof(token.c_str()));
					file >> token;
				}
				param.set(val);
			}
		}
		else if (paramType == "array2f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_FLOAT2);
			else {
				std::vector<Vector2> val;
				while (token != ";") {
					Vector2 vec;
					vec.x = atof(token.c_str());
					file >> token;
					vec.y = atof(token.c_str());
					file >> token;
					val.push_back(vec);
				}
				param.set(val);
			}
		}
		else if (paramType == "array3f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_FLOAT3);
			else {
				std::vector<Vector3> val;
				while (token != ";") {
					Vector3 vec;
					vec.x = atof(token.c_str());
					file >> token;
					vec.y = atof(token.c_str());
					file >> token;
					vec.z = atof(token.c_str());
					file >> token;
					val.push_back(vec);
				}
				param.set(val);
			}
		}
		else if (paramType == "array4f")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_FLOAT4);
			else {
				std::vector<Vector4> val;
				while (token != ";") {
					Vector4 vec;
					vec.x = atof(token.c_str());
					file >> token;
					vec.y = atof(token.c_str());
					file >> token;
					vec.z = atof(token.c_str());
					file >> token;
					vec.w = atof(token.c_str());
					file >> token;
					val.push_back(vec);
				}
				param.set(val);
			}
		}
		else if (paramType == "matrix4")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_ARRAY_FLOAT4);
			else {
				Matrix4 val;
				for (int i=0; i<4; ++i)
					for (int j=0; j<4; ++j)
					{
						val.m[i][j] = atof(token.c_str());
						file >> token;
					}
				param.set(val);
				file >> token;
			}
		}
		else if (paramType == "sampler1D")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_SAMPLER1D);
			else {
				Texture *pTex = TextureManager::loadTexture(token);
				param.set(pTex);
				file >> token;
			}
		}
		else if (paramType == "sampler2D")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_SAMPLER2D);
			else {
				Texture *pTex = TextureManager::loadTexture(token);
				param.set(pTex);
				file >> token;
			}
		}
		else if (paramType == "sampler3D")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_SAMPLER3D);
			else {
				Texture3D *pTex = TextureManager::loadTexture3D(token);
				param.set(pTex);
				file >> token;
			}
		}
		else if (paramType == "samplerCube")
		{
			if (token == ";")
				param.setAuto(Param::PARAM_SAMPLERCUBE);
			else {
				CubeTexture *pTex = TextureManager::loadTextureCube(token);
				param.set(pTex);
				file >> token;
			}
		}

		// check that the param desc ends with ';'
		if (token != ";")
		{
			Console::print("\terror parsing shader desc: ';' expected but '%s' found\n", token.c_str());
			break;
		}

		// add the new parameter
		m_params.push_back(param);
	}
	
	// load the shader
	if (!load(shaderFilename, shaderMainFunc, m_type))
		return false;

	return true;
}

void Shader::setParamd(const std::string& param_name, double val)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameter1d(param, val);
}

void Shader::setParam1f(const std::string& param_name, float val)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameter1f(param, val);
}

void Shader::setParam2f(const std::string& param_name, const Vector2 &val)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameter2f(param, val.x, val.y);
}

void Shader::setParam3f(const std::string& param_name, const Vector3 &val)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameter3f(param, val.x, val.y, val.z);
}

void Shader::setParam4f(const std::string& param_name, const Vector4 &val)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameter4f(param, val.x, val.y, val.z, val.w);
}

void Shader::setParamArray1d(const std::string& param_name, double *vals, int nVals)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameterArray1d(param, 0, nVals, vals);
}

void Shader::setParamArray1f(const std::string& param_name, float *vals, int nVals)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameterArray1f(param, 0, nVals, vals);
}

void Shader::setParamArray2f(const std::string& param_name, Vector2 *vals, int nVals)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameterArray2f(param, 0, nVals, (float*)vals);
}

void Shader::setParamArray3f(const std::string& param_name, Vector3 *vals, int nVals)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameterArray3f(param, 0, nVals, (float*)vals);
}

void Shader::setParamArray4f(const std::string& param_name, Vector4 *vals, int nVals)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetParameterArray4f(param, 0, nVals, (float*)vals);
}

void Shader::setParamMatrix4(const std::string& param_name, const Matrix4 &mat)
{
	ASSERT(m_program);
	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetMatrixParameterdr(param, (double*)mat.m);
}

void Shader::setSampler(const std::string& param_name, GLuint sampler)
{
	CGparameter tex = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetTextureParameter(tex, sampler);
	cgGLEnableTextureParameter(tex);
}

void Shader::setModelViewMatrix(const std::string& param_name, bool bInverse, bool bTranspose)
{
	CGGLenum transform = CG_GL_MATRIX_IDENTITY;
	if (bInverse && bTranspose)
		transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
	else if (bInverse)
		transform = CG_GL_MATRIX_INVERSE;
	else if (bTranspose)
		transform = CG_GL_MATRIX_TRANSPOSE;

	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetStateMatrixParameter(param, CG_GL_MODELVIEW_MATRIX,
							transform);
}

void Shader::setProjMatrix(const std::string& param_name, bool bInverse, bool bTranspose)
{
	CGGLenum transform = CG_GL_MATRIX_IDENTITY;
	if (bInverse && bTranspose)
		transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
	else if (bInverse)
		transform = CG_GL_MATRIX_INVERSE;
	else if (bTranspose)
		transform = CG_GL_MATRIX_TRANSPOSE;

	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetStateMatrixParameter(param, CG_GL_PROJECTION_MATRIX,
							transform);
}

void Shader::setModelViewProjMatrix(const std::string& param_name, bool bInverse, bool bTranspose)
{
	CGGLenum transform = CG_GL_MATRIX_IDENTITY;
	if (bInverse && bTranspose)
		transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
	else if (bInverse)
		transform = CG_GL_MATRIX_INVERSE;
	else if (bTranspose)
		transform = CG_GL_MATRIX_TRANSPOSE;

	CGparameter param = cgGetNamedParameter(m_program, param_name.c_str());
	cgGLSetStateMatrixParameter(param, CG_GL_MODELVIEW_PROJECTION_MATRIX,
							transform);
}