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

#include "CheckBox.h"
#include "Font.h"
#include "ResourceManager.h"
#include "Container.h"

using namespace begui;

CheckBox::CheckBox() : m_state(CheckBox::NOT_CHECKED), m_bHover(false),
	m_id(-1), m_pBoundVal(0)
{
}

void CheckBox::create(int x, int y, const std::string &title, int id, Functor1<int> &callback)
{
	m_title = title;
	m_state = CheckBox::NOT_CHECKED;
	m_onClick = callback;
	m_id = id;
	m_bHover = false;

	m_left = x;
	m_right = x + 12 + Font::stringLength(title);
	m_top = y;
	m_bottom = y+12;
}

void CheckBox::bindValue(bool *val)
{
	m_pBoundVal = val;
}

void CheckBox::onUpdate()
{
	if (m_pBoundVal)
	{
		if (m_state == CheckBox::NOT_CHECKED && *m_pBoundVal)
			m_state = CheckBox::CHECKED;
	}
}

void CheckBox::onRender()
{
	// set the texture of a window
	Texture *pTex = ResourceManager::inst()->getStockMap(ResourceManager::STD_CONTROLS);
	pTex->set();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int w = getWidth();
	int h = getHeight();

	// find the position
	int tL=388;
	int tR = tL+12;
	int tT = 2;
	int tB=tT+12;

	// render the icon
	glColor4f(1,1,1,1);
	if (m_bHover && m_state != CheckBox::INACTIVE)
		glColor4f(1,1,1,0.8);
	else if (m_state == CheckBox::INACTIVE)
		glColor4f(1,1,1,0.5);
	glBegin(GL_QUADS);
		glTexCoord2f(tL/512.0, tT/512.0);	glVertex3f(0, 0, 0);
		glTexCoord2f(tR/512.0, tT/512.0);	glVertex3f(12, 0, 0);
		glTexCoord2f(tR/512.0, tB/512.0);	glVertex3f(12, 12, 0);
		glTexCoord2f(tL/512.0, tB/512.0);	glVertex3f(0, 12, 0);
		
		// render the check mark
		if (m_state == CheckBox::CHECKED)
		{
			// size of the checkmark
			int chW = 17;
			int chH = 20;
			// pos in the texture file.
			int chU = 405;
			int chV = 4;
			glTexCoord2f(chU/512.0, chV/512.0);			glVertex3f(2, -2, 0);
			glTexCoord2f((chU+chW)/512.0, chV/512.0);	glVertex3f(11, -2, 0);
			glTexCoord2f((chU+chW)/512.0, (chV+chH)/512.0);	glVertex3f(11, h-1, 0);
			glTexCoord2f(chU/512.0, (chV+chH)/512.0);	glVertex3f(2, h-1, 0);
		}
	glEnd();
		
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	glColor3f(0.3,0.3,0.3);
	if (m_state == CheckBox::INACTIVE)
		glColor3f(0.6, 0.6, 0.6);
	Font::renderString(18, h-2, m_title);
}

void CheckBox::onMouseDown(int x, int y, int button)
{
}

void CheckBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (isPtInside(x,y))
		m_bHover = true;
	else
		m_bHover = false;
}

void CheckBox::onMouseUp(int x, int y, int button)
{
	if (m_state == CheckBox::INACTIVE)
		return;
	m_state = (m_state == CheckBox::CHECKED)?CheckBox::NOT_CHECKED:CheckBox::CHECKED;

	// Call the callback, if any
	m_onClick(m_id);

	// update the live var if any
	if (m_pBoundVal)
		*m_pBoundVal = (m_state == CheckBox::CHECKED);
}

void CheckBox::onKeyDown(int key)
{
}

void CheckBox::onKeyUp(int key)
{
}

bool CheckBox::isPtInside(int x, int y)
{
	if (x<m_left || x>m_left+18)
		return false;
	if (y<m_top || y>m_top+18)
		return false;
	return true;
}
