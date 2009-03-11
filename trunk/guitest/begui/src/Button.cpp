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

#include "Button.h"
#include "Font.h"
#include "WindowResourceManager.h"

using namespace begui;

Button::Button() : 
	m_bHover(false),
	m_id(-1),
	m_status(Button::UP)
{
}

void Button::create(int x, int y, const std::string &title, int id, Functor1<int> &callback)
{
	m_title = title;
	m_onClick = callback;
	m_id = id;
	m_status = Button::UP;
	m_bHover = false;

	int w = 40 + Font::stringLength(title);
	if (w < 80)
		w = 80;

	m_left = x;
	m_right = x + w;
	m_top = y;
	m_bottom = y+26;
}

void Button::onUpdate()
{
}

void Button::onRender()
{
	// set the texture of a window
	Texture *pTex = WindowResourceManager::inst()->getChildWindowImage();
	pTex->set();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	int w = getWidth();
	int h = getHeight();

	// render the icon
	if (m_status == Button::INACTIVE)
		glColor4f(1,1,1, 0.5);
	else if (m_bHover)
		glColor4f(1,1,1,0.8);
	else
		glColor4f(1,1,1,1);
	int offs = (m_status == Button::DOWN)?1:0;
	Component::drawBorderedQuad(offs, offs, w-offs, h-offs,
							13, 13, w-13, h-13,
							410/512.0, 423/512.0, 505/512.0 , 492/512.0,
							32/512.0, 45/512.0, 58/512.0, 45/512.0);
		
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	int center = getWidth()/2;
	if (m_status == Button::INACTIVE)
		glColor3f(0.3, 0.3, 0.3);
	else
		glColor3f(1,1,1);
	Font::renderString(center - Font::stringLength(m_title)/2, h-9, m_title);
}

void Button::onMouseDown(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		m_status = Button::DOWN;
	}
}

void Button::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_status != Button::INACTIVE && isPtInside(x,y))
		m_bHover = true;
	else
		m_bHover = false;
}

void Button::onMouseUp(int x, int y, int button)
{
	if (m_status != Button::INACTIVE) {
		m_status = Button::UP;
		m_onClick(m_id);
	}
}

void Button::onKeyDown(int key)
{
}

void Button::onKeyUp(int key)
{
}

bool Button::isPtInside(int x, int y)
{
	if (x<m_left || x>m_right)
		return false;
	if (y<m_top || y>m_bottom)
		return false;
	return true;
}
