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

#include "Font.h"

using namespace begui;

std::vector<Font*>		FontManager::m_fonts;
FT_Library				FontManager::m_freetype;
bool					FontManager::m_ftInitialized = false;
int						FontManager::m_curFont = -1;
std::vector<Texture*>	FontManager::m_textureList;
int						FontManager::m_lastCharRight = 0;
int						FontManager::m_lastCharTop = 0;
int						FontManager::m_lastCharBottom = 0;
std::vector<unsigned char*>	FontManager::m_fontDrawingMem;
int						FontManager::m_texWidth = 512;
int						FontManager::m_texHeight = 512;

bool FontManager::initialize()
{
	// initialize freetype
	int error = FT_Init_FreeType( &m_freetype );
	if ( error ) {
		Console::print("ERROR: failed to initialize FreeType\n");
		return false;
	}
	Console::print("FreeType library initialized ok\n");

	return true;
}

void FontManager::clear()
{
	// destroy all fonts
	for (size_t i=0; i<m_fonts.size(); ++i)
		SAFE_DELETE(m_fonts[i]);
	m_fonts.clear();

	// destroy all textures
	for (size_t i=0; i<m_textureList.size(); ++i)
		SAFE_DELETE(m_textureList[i]);
	m_textureList.clear();
	m_lastCharRight = 0;
	m_lastCharTop = 0;
	m_lastCharBottom = 0;

	// clear font system memory
	for (size_t i=0; i<m_fontDrawingMem.size(); ++i)
		SAFE_DELETE(m_fontDrawingMem[i]);
	m_fontDrawingMem.clear();

	// close freetype
	FT_Done_FreeType(m_freetype);
}

bool FontManager::setFont(const std::string &font_name, int font_size)
{
	// check if the font is already loaded
	for (size_t i=0; i<m_fonts.size(); ++i) {
		if (m_fonts[i]->getFontFileName() == font_name && m_fonts[i]->getFontSize() == font_size)
			return true;
	}

	// create the new font
	Font *pFont = new Font();
	if (!pFont->createFont(font_name, font_size))
		return false;
	m_fonts.push_back(pFont);
	m_curFont = (int)m_fonts.size()-1;

	return true;
}

void FontManager::beginFontCaching()
{
	// nothing to do here
}

Font::Character FontManager::allocCharacterDrawingArea(int width, int height)
{
	// TODO: update this method to use a quad-tree to keep track
	// of the free space in each texture, in order to reduce wasted space.

	Font::Character ref;

	// if we already have some allocated textures, and if the last of
	// them is still writable (font drawing memory is still allocated),
	// check if this texture has enough space for this character
	if (m_textureList.size() > 0 && m_fontDrawingMem.back() != 0)
	{
		Texture *pLastTex = m_textureList.back();

		// check if we have enough space in the last texture used
		if (m_lastCharRight + width + 3 < pLastTex->getWidth())
		{
			// fill in the returned image ref
			ref.m_pTexture = pLastTex;
			ref.m_top = m_lastCharTop;
			ref.m_left = m_lastCharRight + 2;
			ref.m_bottom = ref.m_top + height;
			ref.m_right = ref.m_left + width;
			ref.m_pDrawingBuffer = m_fontDrawingMem.back();
			ref.m_drawingBufferPitch = m_texWidth;

			// advance the last character position
			m_lastCharRight = ref.m_right;
			m_lastCharTop = ref.m_top;
			if (m_lastCharBottom < ref.m_bottom)
				m_lastCharBottom = ref.m_bottom;	// lastCharBottom is the bottom of the whole "line" (wasted space here)

			return ref;
		}
		if (m_lastCharBottom + height + 3 < pLastTex->getHeight())
		{
			// go to the next line

			// fill in the returned image ref
			ref.m_pTexture = pLastTex;
			ref.m_top = m_lastCharBottom + 2;
			ref.m_left = 2;
			ref.m_bottom = ref.m_top + height;
			ref.m_right = ref.m_left + width;
			ref.m_pDrawingBuffer = m_fontDrawingMem.back();
			ref.m_drawingBufferPitch = m_texWidth;

			// advance the last character position
			m_lastCharRight = ref.m_right;
			m_lastCharTop = ref.m_top;
			m_lastCharBottom = ref.m_bottom;

			return ref;
		}
	}

	// if we got here, no space was available. Create a new texture
	Texture *pTex = new Texture();
	m_textureList.push_back(pTex);
	// and allocate some system memory to draw on..
	m_fontDrawingMem.push_back(new unsigned char[m_texWidth * m_texHeight * 4]);
	memset(m_fontDrawingMem.back(), 0, m_texWidth * m_texHeight * 4);
	// and reset the drawing position
	m_lastCharRight = 0;
	m_lastCharTop = 0;
	m_lastCharBottom = 0;

	ref.m_pTexture = pTex;
	ref.m_top = 2;
	ref.m_left = 2;
	ref.m_bottom = ref.m_top + height;
	ref.m_right = ref.m_left + width;
	ref.m_pDrawingBuffer = m_fontDrawingMem.back();
	ref.m_drawingBufferPitch = m_texWidth;

	return ref;
}

void FontManager::endFontCaching()
{
	// create all textures, copying data from the corresponding drawing surfaces
	for (size_t i=0; i<m_textureList.size(); ++i)
	{
		// only re-create textures where the font drawing memory contains data
		if (m_fontDrawingMem[i] == 0)
			continue;

		// create the texture
		m_textureList[i]->create(m_texWidth, m_texHeight, GL_RGBA, m_fontDrawingMem[i]);
	}
}