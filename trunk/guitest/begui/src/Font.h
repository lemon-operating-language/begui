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

#ifndef _FONT_H42631_INCLUDED_
#define _FONT_H42631_INCLUDED_

#pragma once

#include "common.h"
#include "../../bcore/src/Rect.h"
#include "ResourceManager.h"

// FreeType
extern "C" {
	#include <ft2build.h>
	#include FT_FREETYPE_H
}


namespace begui {

/******************************************************************************
 * Font:
 *
 ******************************************************************************/
class Font
{
	friend class FontManager;

	class Character {
	public:
		int				m_char;
		int				m_left, m_right, m_top, m_bottom;
		int				m_horiBearingX;
		int				m_horiBearingY;
		int				m_horiAdvance;
		Texture			*m_pTexture;
		unsigned char	*m_pDrawingBuffer;	// not guaranteed to be valid after FontManager::endFontCaching is called
		int				m_drawingBufferPitch;

	public:
		Character() : m_char(0), m_left(0), m_right(0), m_top(0), m_bottom(0), 
			m_horiBearingX(0), m_horiBearingY(0), m_horiAdvance(0),
			m_pTexture(0), m_pDrawingBuffer(0), m_drawingBufferPitch(0) { }
	};

public:
	Font();
	virtual ~Font();

	static void renderString(int x, int y, const std::string& str, 
							std::vector< Rect<int> > *char_pos_out = 0,
							bool bRender = true,
							int vpWidth=-1, int vpHeight=-1);
	static int	stringLength(const std::string& str);
	
	void renderStringMultiline(int x, int y, int lineWidth, const std::string& str,
							std::vector< Rect<int> > *char_pos_out = 0, bool bRender = true);

	const std::string&	getFontFileName() const		{ return m_fontFileName; }
	int					getFontSize() const			{ return m_fontSize; }
	int					getLineHeight() const		{ return m_lineHeight; }
	Character*			getChar(int c)				{ if (c<m_startChar || c>=(int)m_character.size()) return 0; return &m_character[c - m_startChar]; }

protected:
	bool createFont(const std::string &font_file, int font_size);	// use FontManager to create a font!

private:
	std::vector<Character>	m_character;
	int						m_startChar;	// the first char in the letters
	int						m_lineHeight;
	int						m_tabSize;		// size of tabs in spaces (not pixels)
	std::string				m_fontFileName;
	int						m_fontSize;

	void renderString_i(int x, int y, const std::string& str, std::vector< Rect<int> > *char_pos_out, bool bRender);
};

/******************************************************************************
 * FontManager:
 *
 ******************************************************************************/
class FontManager
{
	friend class Font;

private:
	static std::vector<Font*> m_fonts;
	static int				m_curFont;						// the currently selected font
	static FT_Library		m_freetype;						// Handle to the freetype library
	static bool				m_ftInitialized;				// true if freetype has been initialized already

	static std::vector<Texture*>		m_textureList;		// the textures where the characters are stored
	static std::vector<unsigned char*>	m_fontDrawingMem;	// system memory to draw font faces before copying them to the textures
	static int	m_lastCharRight, m_lastCharTop, m_lastCharBottom; // the last coordinates where a character was drawn (to be replaced by a quad tree)
	static int	m_texWidth, m_texHeight;					// the dimensions of the allocated textures

public:
	static bool initialize();
	static void clear();
	static bool isInitialized()	{ return m_ftInitialized; }

	static bool setFont(const std::string &font_name, int font_size);
	static Font* getCurFont()	{ if (m_curFont < 0) return NULL; return m_fonts[m_curFont]; }

	// set the size of the texture pages used to cache fonts
	static void setCachePageSize(int w, int h)		{ m_texWidth = w; m_texHeight = h; }

protected:
	static FT_Library	getFTLib()	{ return m_freetype; }

	// starts font caching (drawing font faces into textures)
	static void beginFontCaching();

	// ends font caching. In this step, the textures are created
	// from the temporary images in system memory (so this call can be costly)
	static void endFontCaching();

	// Get the drawing area for a character. The drawing area includes a pointer to the
	// corresponding texture, as well as the coordinates of the area
	static Font::Character allocCharacterDrawingArea(int width, int height);
};


/******************************************************************************
 * Text:
 *
 ******************************************************************************/
class Text {
public:
	struct Appearance {
		Font	*m_pFont;
		double	m_xScale, m_yScale;	// font scaling in x and y
		int		m_xOffs, m_yOffs;	// offsets in the x and y positioning of the font
		int		m_xAdvanceOffs;		// offset for advancing the cursor after this character
		double	m_rotation;			// rotation in degrees
		Color	m_color;
	};

public:
	static void		setCursorPos(int x, int y);
	static Vector2i getCursorPos()					{ return Vector2i(m_cursorX, m_cursorY); }
	static void		display(const std::string& text, bool bMultiline, const std::vector<Appearance>* format = 0, 
							std::vector< Rect<int> > *char_pos_out = 0);
	static void		getCharPositions(const std::string& text, bool bMultiline, const std::vector<Appearance>* format, 
							std::vector< Rect<int> > *char_pos_out);
	static int		getStringLength(const std::string& text, const std::vector<Appearance>* format = 0);

private:
	static int	m_cursorX, m_cursorY;

};

};

#endif