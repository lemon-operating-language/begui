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

#include "../common.h"
#include "../Material.h"

class FileLoader
{
public:
	interface ISceneCallback {
	public:
		virtual void onSceneBegin(int nModels = -1) = 0;
		virtual void onSceneEnd() = 0;
		virtual void onNewMesh() = 0;
	};
	interface IMeshCallback {
	public:
		virtual void onMeshBegin(int nVertices, int nFaces) = 0;
		virtual void onMeshEnd() = 0;
		virtual void onNewVertex(const Vector3* pos, const Vector3* norm = 0, const Vector2 *uv = 0) = 0;
		virtual void onNewFace(const std::vector<int>& ids, int material=0, const std::vector<int>* texids=0) = 0;
		virtual void onNewMaterial(const Material& mat) = 0;
		virtual void onNewUV2(const Vector2* uv) = 0;
	};

protected:
	ISceneCallback	*m_pSceneCallback;
	IMeshCallback	*m_pMeshCallback;

public:
	FileLoader() : m_pSceneCallback(0), m_pMeshCallback(0) { }

	void setSceneCallback(ISceneCallback *cb)	{ m_pSceneCallback = cb; }
	void setMeshCallback(IMeshCallback *cb)		{ m_pMeshCallback = cb; }
	virtual bool loadFile(const std::string& filename) = 0;
};