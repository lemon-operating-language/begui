/* 
// Copyright 2007 Alexandros Panagopoulos
//
// This software is distributed under the terms of the GNU Lesser General Public Licence
//
// This file is part of BeGUI library.
//
//    BeGUI is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    BeGUI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with BeGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ResourceManager.h"
#include "../../b3d_lib/src/BaseTextFile.h"
#include "../../b3d_lib/src/Image.h"
#include <direct.h>

#pragma warning (disable : 4996)

using namespace begui;

// The singleton instance
ResourceManager *ResourceManager::m_instance;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	freeResources();
}

void ResourceManager::loadResources()
{
	freeResources();

	// load all textures
	// Initialize the window manager and load resources
	
	Image img;
	img.load(getResourceDir() + "bg.bmp");
	Texture *bgtex = new Texture();
	bgtex->create(img);
	m_loadedTextures.push_back(bgtex);

	Texture *wnd_controls = new Texture();
	img.loadWithAlpha(getResourceDir() + "widgets.bmp", getResourceDir() + "widgets_alpha.bmp");
	wnd_controls->create(img);
	m_loadedTextures.push_back(wnd_controls);
}

void ResourceManager::freeResources()
{
	// Free all allocated resources
	m_stockImages.clear();
	for (size_t i=0; i<m_loadedTextures.size(); ++i)
		SAFE_DELETE(m_loadedTextures[i]);
	m_loadedTextures.clear();
}

Texture* ResourceManager::getStockMap(ResourceManager::StockMap i)
{
	switch (i) {
		case FRAME_BACKGROUND: return m_loadedTextures[0];
		case WINDOW_RES: return m_loadedTextures[1];
		case STD_CONTROLS: return m_loadedTextures[1];
	}
	return 0;
}

ResourceManager::ImageRef ResourceManager::loadImage(const std::string &filename, bool bPack)
{
	ImageRef iref;

	// load the texture
	Image img;
	if (!img.load(getResourceDir() + filename))	// TEMP! should check for full path/relative path, and append backslashes
		return iref;
	Texture *tex = new Texture; // TEMP! should try to pack image in other textures, if possible
	tex->create(img);
	m_loadedTextures.push_back(tex);
	
	iref.m_texture = tex;
	iref.m_topLeft = Vector2(0,0);
	iref.m_bottomRight = Vector2(img.getWidth(), img.getHeight());

	return iref;
}

std::string ResourceManager::getResourceDir() const
{
	if (m_resourceDir.length() == 0) {
		char currentPath[2048];
		_getcwd(currentPath, 2000);
		return std::string(strcat(currentPath, "/../../../begui/resources/"));
	}
	return m_resourceDir;
}

void ResourceManager::setResourceDir(const std::string &resdir)
{
	m_resourceDir = resdir;
}