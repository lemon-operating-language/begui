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

#pragma once

#pragma warning (push)
#pragma warning (disable : 4996)	// to avoid this, use namespace stdext:: instead of std...
									// but this breaks compilation with VS 6... thanks MS!

#include "common.h"
#include <hash_map>

namespace begui {

/**
 *===================================================================================
 * ResourceManager: This class manages resources used by the GUI. This includes
 *			textures, strings, and scalar/vector values that define the GUI objects.
 *			It provides for organized loading/unloading of texture resources, and
 *			allows the use of parametrized skins.
 *===================================================================================
 */
class ResourceManager
{
public:
	/**
	* ImageRef: defines a loaded image, by the texture on which it has
	*		been loaded, and the coordinates of the rectangle of that
	*		image on the texture. This allows for many small images to be
	*		packed in few large textures.
	*/
	class ImageRef {
		long	m_id;
	public:
		Texture *m_texture;
		Vector2 m_topLeft;
		Vector2 m_bottomRight;
		int		m_width, m_height;

	public:
		ImageRef() : m_texture(0), m_width(0), m_height(0) { }
	};

	/**
	 * Stock images available
	 */
	enum StockMap {
		FRAME_BACKGROUND,
		WINDOW_RES,
		STD_CONTROLS
	};

private:
	static ResourceManager	*m_instance;
	std::string				m_resourceDir;
	std::vector<Texture*>	m_loadedTextures;
	std::vector<ImageRef>	m_stockImages;

	class ClassProps {
	public:
		stdext::hash_map<std::string, std::string>	m_strProps;
		stdext::hash_map<std::string, double>		m_numProps;
	};
	stdext::hash_map<std::string, ClassProps> m_classProps;
	
	ResourceManager();

public:
	static inline ResourceManager* inst() { if (!m_instance) m_instance = new ResourceManager(); return m_instance; }
	
	virtual ~ResourceManager();

	void loadResources();
	void freeResources();

	Texture*	getStockMap(StockMap i);
	ImageRef	loadImage(const std::string &filename, bool bPack = true);

	std::string getResourceDir() const;
	void		setResourceDir(const std::string& resdir);

	// Property handling
	void loadPropertyFile(const std::string& fname);

	int			getPropI(const std::string& className, const std::string& propName);
	double		getPropF(const std::string& className, const std::string& propName);
	std::string getPropS(const std::string& className, const std::string& propName);
};

#pragma warning (pop)

};