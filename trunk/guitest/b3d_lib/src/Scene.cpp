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

#include "scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	for (size_t i=0; i<m_meshes.size(); ++i)
		SAFE_DELETE(m_meshes[i]);
	m_meshes.clear();
	for (size_t i=0; i<m_lights.size(); ++i)
		SAFE_DELETE(m_lights[i]);
	m_lights.clear();
}

void Scene::initialize()
{
/*	// setup light
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	Light *light = addLight();
	light->diffuse = Color(1.0f, 0.9f, 0.8f);
	light->ambient = Color(0.2f, 0.2f, 0.2f);
	light->type = Light::DIRECTIONAL;
	light->dir = Vector3(0,1,1);

	light = addLight();
	light->diffuse = Color(0.2f, 0.2f, 0.4f);
	light->ambient = Color(0, 0, 0);
	light->type = Light::POINT;
	light->pos = Vector3(-2, -5, -20);*/
}

Mesh* Scene::addMesh()
{
	Mesh *pMesh = new Mesh();
	m_meshes.push_back(pMesh);
	return pMesh;
}

Light* Scene::addLight()
{
	Light *pLight = new Light();
	m_lights.push_back(pLight);
	return pLight;
}

void Scene::render(bool bShowNormals, bool bShowBBox)
{
	// render the scene

	// render the objects
	for (size_t i=0; i<m_meshes.size(); ++i)
	{
		// Render the mesh's bbox
		if (bShowBBox)
			m_meshes[i]->getBBox().render();

		// Set materials and colors
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		GLfloat mat_diffuse_cl[] = { 1, 1, 1, 1.0f };
		GLfloat mat_specular_cl[] = { 0.9f, 0.9f, 0.9f, 1.0f };
		GLfloat shininess = 100.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_cl);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_cl);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);

		m_meshes[i]->render();
		
		// Render normals.
		if (bShowNormals)
		{
			glColor3f(0.3, 1, 0.4);
			m_meshes[i]->renderVertexNormals();
		}
	}
}