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

#include "Label.h"
#include "Font.h"

using namespace begui;

Label::Label()
{
}

void Label::create(int x, int y, const std::string& text)
{
	m_text = text;
	m_left = x;
	m_right = x + Font::stringLength(text);
	m_top = y;
	m_bottom = y+10;
}

void Label::onUpdate()
{
}

void Label::onRender()
{	
	glColor4f(0.3, 0.3, 0.3, 0.8);
	Font::renderString(0, getHeight(), m_text);
}

void Label::onMouseDown(int x, int y, int button)
{
}

void Label::onMouseMove(int x, int y, int prevx, int prevy)
{
}

void Label::onMouseUp(int x, int y, int button)
{
}

void Label::onKeyDown(int key)
{
}

void Label::onKeyUp(int key)
{
}

bool Label::isPtInside(int x, int y)
{
	// a label is passive, does not respond to mouse movements etc.
	// so, just dont bother..
	return false;
}
