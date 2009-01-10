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

CGcontext ShaderManager::m_context;
CGprofile ShaderManager::m_fragmentProfile, ShaderManager::m_vertexProfile;
//std::vector<Shader*> ShaderManager::m_shaders;

void ShaderManager::errorCallBack()
{
	CGerror err = cgGetError();
	Console::print("ERROR : %s\n", cgGetErrorString(err));
}

void ShaderManager::initialize()
{
	Console::print("initializing Cg...\n");

	// create context
	m_context = cgCreateContext();

	// initialize profiles and compiler options
	m_vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cgGLSetOptimalOptions(m_vertexProfile);
	m_fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(m_fragmentProfile);
	// Make sure that the appropriate profiles are available on the
    // user's system.
/*    if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
        m_vertexProfile = CG_PROFILE_ARBVP1;
    else {
        // try VP30
        if (cgGLIsProfileSupported(CG_PROFILE_VP30))
            m_vertexProfile = CG_PROFILE_VP30;
        else {
			Console::print("Neither arbvp1 or vp30 vertex profiles supported on this system.\n");
            exit(1);
        }
    }

    if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
        m_fragmentProfile = CG_PROFILE_ARBFP1;
    else {
        // try FP30
        if (cgGLIsProfileSupported(CG_PROFILE_FP30))
            m_fragmentProfile = CG_PROFILE_FP30;
        else {
            Console::print("Neither arbfp1 or fp30 fragment profiles supported on this system.\n");
            exit(1);
        }
    }
*/
	Console::print("\tvertex profile = %s\n", cgGetProfileString(m_vertexProfile));
	Console::print("\tfragment profile = %s\n", cgGetProfileString(m_fragmentProfile));

	// set the error callback
	cgSetErrorCallback(errorCallBack);

	Console::print("\tok\n");
}

void ShaderManager::free()
{
	//for (unsigned int i=0; i<m_shaders.size(); ++i)
	//	SAFE_DELETE(m_shaders[i]);

	cgDestroyContext(m_context);
	Console::print("Cg context destroyed\n");
}

bool ShaderManager::loadShader(Shader *shader, const std::string& filename, const std::string& mainFuncName, Shader::Type type)
{
	ASSERT(shader);
	shader->m_type = type;

	// Create the program
	if (type == Shader::VERTEX) {
		shader->m_program = cgCreateProgramFromFile(m_context, CG_SOURCE, filename.c_str(),
											m_vertexProfile, mainFuncName.c_str(), 0);
	}
	else
	{
		shader->m_program = cgCreateProgramFromFile(m_context, CG_SOURCE, filename.c_str(),
											m_fragmentProfile, mainFuncName.c_str(), 0);
	}

	// compile the shader
	if (!cgIsProgramCompiled(shader->m_program))
        cgCompileProgram(shader->m_program);

	// check for errors
	CGerror err = cgGetError();
	if (err != CG_NO_ERROR || shader->m_program == 0)
	{
		Console::print("ERROR: compiling shader %s : %s\n", filename.c_str(), cgGetErrorString(err));
		return false;
	}

	// Load the program
	cgGLLoadProgram(shader->m_program);
	err = cgGetError();
	if (err != CG_NO_ERROR)
	{
		Console::print("ERROR: loading shader %s : %s\n", filename.c_str(), cgGetErrorString(err));
		return false;
	}

	Console::print("\t-loaded shader: %s - %s\n", filename.c_str(), mainFuncName.c_str());
	return true;
}

void ShaderManager::enableProfiles()
{
	cgGLEnableProfile(m_vertexProfile);
	cgGLEnableProfile(m_fragmentProfile);
}

void ShaderManager::disableProfiles()
{
	cgGLDisableProfile(m_vertexProfile);
	cgGLDisableProfile(m_fragmentProfile);
}