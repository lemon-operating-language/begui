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

#pragma once

#include "common.h"
#include <cg/cg.h>
#include <cg/cgGL.h>
class Texture;
class Texture3D;
class CubeTexture;
class BaseTextFile;

class Shader
{
	friend class ShaderManager;

public:
	/** Param: This class represents a shader parameter. It holds the name, type and
		value of the parameter. It also holds a SetOnce flag. If set, the
		parameter will be set to the stored value every frame, before the shader
		is used. If the flag is not set, the program is responsible for generating
		and setting a value for the parameter. This flag is not set for values
		like the various transformation matrices, bones etc.
		In this case, the program deduces which value it has to set from the 
		parameter name. This means that for auto-gen params, only certain preset
		names can be used.
	*/
	class Param
	{
		friend class Shader;
	public:
		enum Type {
			PARAM_DOUBLE1,
			PARAM_FLOAT1,
			PARAM_FLOAT2,
			PARAM_FLOAT3,
			PARAM_FLOAT4,
			PARAM_ARRAY_DOUBLE1,
			PARAM_ARRAY_FLOAT1,
			PARAM_ARRAY_FLOAT2,
			PARAM_ARRAY_FLOAT3,
			PARAM_ARRAY_FLOAT4,
			PARAM_MATRIX4,
			PARAM_SAMPLER1D,
			PARAM_SAMPLER2D,
			PARAM_SAMPLER3D,
			PARAM_SAMPLERCUBE,
			PARAM_NOTSET	// invalid param
		};
	private:
		Type		m_type;			// type of the parameter
		bool		m_bSetOnce;		// true if this parameter needs to be set only once after loading the shader
									// false if it has to be set on every frame
		void		*m_pValue;		// value of the parameter;
		std::string	m_name;

	public:
		// constructor/destructor
		Param(std::string &name) : m_name(name), m_bSetOnce(false), m_type(PARAM_NOTSET), m_pValue(0) { }
		Param(const Param& p);
		~Param() { SAFE_DELETE(m_pValue); }

		void setAuto(Type type)	{ m_type = type; m_bSetOnce = false; }

		// set the value of the parameter. These methods are supposed to be used to set values
		// that remain constant between several frames.
		void set(double val) { m_bSetOnce = true; m_pValue = new double(val); m_type = PARAM_DOUBLE1; }
		void set(float val) {m_bSetOnce = true; m_pValue = new float(val); m_type = PARAM_FLOAT1; }
		void set(Vector2& val) {m_bSetOnce = true; m_pValue = new Vector2(val); m_type = PARAM_FLOAT2; }
		void set(Vector3& val) {m_bSetOnce = true; m_pValue = new Vector3(val); m_type = PARAM_FLOAT3; }
		void set(Vector4& val) {m_bSetOnce = true; m_pValue = new Vector4(val); m_type = PARAM_FLOAT4; }
		void set(std::vector<double>& vals) { m_bSetOnce = true; m_pValue = new std::vector<double>(vals); m_type = PARAM_ARRAY_DOUBLE1; }
		void set(std::vector<float>& vals) {m_bSetOnce = true; m_pValue = new std::vector<float>(vals); m_type = PARAM_ARRAY_FLOAT1; }
		void set(std::vector<Vector2>& vals) {m_bSetOnce = true; m_pValue = new std::vector<Vector2>(vals); m_type = PARAM_ARRAY_FLOAT2; }
		void set(std::vector<Vector3>& vals) {m_bSetOnce = true; m_pValue = new std::vector<Vector3>(vals); m_type = PARAM_ARRAY_FLOAT3; }
		void set(std::vector<Vector4>& vals) {m_bSetOnce = true; m_pValue = new std::vector<Vector4>(vals); m_type = PARAM_ARRAY_FLOAT4; }
		void set(Matrix4& val) {m_bSetOnce = true; m_pValue = new Matrix4(val); m_type = PARAM_MATRIX4; }
		void set(Texture* val) {m_bSetOnce = true; m_pValue = new (Texture*); (*(Texture**)m_pValue) = (val); m_type = PARAM_SAMPLER2D; }
		void set(Texture3D* val) {m_bSetOnce = true; m_pValue = new (Texture3D*)(val); m_type = PARAM_SAMPLER3D; }
		void set(CubeTexture* val) {m_bSetOnce = true; m_pValue = new (CubeTexture*)(val); m_type = PARAM_SAMPLERCUBE; }
	};

	enum Type {
		VERTEX = 0,
		FRAGMENT
	};

protected:
	CGprogram	m_program;
	Type		m_type;
	std::list<Param>	m_params;

public:
	Shader();
	~Shader();
	virtual bool load(const std::string& filename, const std::string& mainFuncName, Shader::Type type);
	virtual void set();

	virtual bool parseParamsAndLoad(BaseTextFile& file);

	Type getType() const	{ return m_type; }
	std::list<Param>& getParams()	{ return m_params; }

	void setParamd(const std::string& param_name, double val);
	void setParam1f(const std::string& param_name, float val);
	void setParam2f(const std::string& param_name, const Vector2 &val);
	void setParam3f(const std::string& param_name, const Vector3 &val);
	void setParam4f(const std::string& param_name, const Vector4 &val);
	void setParamArray1d(const std::string& param_name, double *vals, int nVals);
	void setParamArray1f(const std::string& param_name, float *vals, int nVals);
	void setParamArray2f(const std::string& param_name, Vector2 *vals, int nVals);
	void setParamArray3f(const std::string& param_name, Vector3 *vals, int nVals);
	void setParamArray4f(const std::string& param_name, Vector4 *vals, int nVals);
	void setParamMatrix4(const std::string& param_name, const Matrix4 &mat);
	void setSampler(const std::string& param_name, GLuint sampler);

	void setModelViewMatrix(const std::string& param_name, bool bInverse=false, bool bTranspose=false);
	void setProjMatrix(const std::string& param_name, bool bInverse=false, bool bTranspose=false);
	void setModelViewProjMatrix(const std::string& param_name, bool bInverse=false, bool bTranspose=false);

protected:
	virtual void setShaderConstants() { };	// constants set once and for all
	virtual void setFrameConstants() { };	// constants set per-frame
};