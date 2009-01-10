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
#include "Shader.h"

class ShaderManager
{
	friend class Shader;
private:
	static CGcontext m_context;
	static CGprofile m_vertexProfile, m_fragmentProfile;
	//static std::vector<Shader*>	m_shaders;

public:
	static void initialize();
	static void free();

	static void		enableProfiles();
	static void		disableProfiles();

	static Shader*		newShader()	{ return new Shader; }

private:
	static bool	loadShader(Shader *shader, const std::string& filename, const std::string& mainFuncName, Shader::Type type);

	static void	errorCallBack();
};