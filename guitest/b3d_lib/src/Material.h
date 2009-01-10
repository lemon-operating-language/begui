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
#include "Color.h"
class Shader;

class Material
{
public:
	std::string	m_name;
	Color	m_diffuse;
	Color	m_ambient;
	Color	m_specular;
	Color	m_emissive;
	float	m_specPower;
	float	m_reflectivity;
	float	m_transparency;
	float	m_fresnelOffs;
	float	m_fresnelPower;
	float	m_fresnelScale;

	Shader	*m_pVertShader;
	Shader	*m_pFragShader;

	Material() : m_diffuse(1,1,1), m_ambient(1,1,1), m_specPower(0), 
		m_reflectivity(0),
		m_transparency(0),
		m_fresnelOffs(0.1f),
		m_fresnelScale(4.2f),
		m_fresnelPower(4.4f),
		m_pVertShader(0),
		m_pFragShader(0)
	{ }
};