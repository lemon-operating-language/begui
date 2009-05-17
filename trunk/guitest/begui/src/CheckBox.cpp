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

CheckBox::CheckBox() : m_state(false), m_bHover(false),
	m_id(-1)
{
}

CheckBox::~CheckBox()
{
}

void CheckBox::create(int x, int y, const std::string &title, int id, const Functor1<int> &callback)
{
	m_title = title;
	m_state = false;
	m_onClick = callback;
	m_id = id;
	m_bHover = false;

	m_left = x;
	m_right = x + 12 + Font::stringLength(title);
	m_top = y;
	m_bottom = y+12;
}

void CheckBox::onUpdate()
{
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
	float tL=388;
	float tR = tL+12;
	float tT = 2;
	float tB=tT+12;

	// render the icon
	if (!isEnabled())
		glColor4f(1.0f,1.0f,1.0f,0.5f);
	else if (m_bHover)
		glColor4f(1.0f,1.0f,1.0f,0.8f);
	else
		glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
		glTexCoord2f(tL/512.0f, tT/512.0f);	glVertex3f(0, 0, 0);
		glTexCoord2f(tR/512.0f, tT/512.0f);	glVertex3f(12, 0, 0);
		glTexCoord2f(tR/512.0f, tB/512.0f);	glVertex3f(12, 12, 0);
		glTexCoord2f(tL/512.0f, tB/512.0f);	glVertex3f(0, 12, 0);
		
		// render the check mark
		if (m_state == true)
		{
			// size of the checkmark
			float chW = 17;
			float chH = 20;
			// pos in the texture file.
			float chU = 405;
			float chV = 4;
			glTexCoord2f(chU/512.0f, chV/512.0f);			glVertex3f(2, -2, 0);
			glTexCoord2f((chU+chW)/512.0f, chV/512.0f);	glVertex3f(11, -2, 0);
			glTexCoord2f((chU+chW)/512.0f, (chV+chH)/512.0f);	glVertex3f(11, (float)(h-1), 0);
			glTexCoord2f(chU/512.0f, (chV+chH)/512.0f);	glVertex3f(2, (float)(h-1), 0);
		}
	glEnd();
		
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	glColor3f(0.3f,0.3f,0.3f);
	if (!isEnabled())
		glColor3f(0.6f, 0.6f, 0.6f);
	Font::renderString(18, h-2, m_title);
}

bool CheckBox::onMouseDown(int x, int y, int button)
{
	return true;
}

bool CheckBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (isPtInside(x,y))
		m_bHover = true;
	else
		m_bHover = false;
	return true;
}

bool CheckBox::onMouseUp(int x, int y, int button)
{
	if (!isEnabled())
		return false;
	m_state = !m_state;

	// Call the callback, if any
	m_onClick(m_id);

	return true;
}

void CheckBox::onKeyDown(int key)
{
}

void CheckBox::onKeyUp(int key)
{
}

