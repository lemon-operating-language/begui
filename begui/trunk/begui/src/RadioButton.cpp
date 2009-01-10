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

#include "RadioButton.h"
#include "Font.h"
#include "WindowResourceManager.h"
#include "Container.h"

using namespace begui;

RadioButton::RadioButton() : m_state(RadioButton::NOT_SELECTED), m_pCallback(0), m_bHover(false),
	m_id(-1), m_bIsRadio(true)
{
}

void RadioButton::create(int x, int y, const std::string &title, int id, void (*callback)(int), bool bIsRadio)
{
	m_title = title;
	m_state = RadioButton::NOT_SELECTED;
	m_pCallback = callback;
	m_id = id;
	m_bHover = false;

	m_left = x;
	m_right = x + 18 + Font::stringLength(title);
	m_top = y;
	m_bottom = y+18;
	m_bIsRadio = bIsRadio;
}

void RadioButton::frameUpdate()
{
}

void RadioButton::frameRender()
{
	// set the texture of a window
	Texture *pTex = WindowResourceManager::inst()->getChildWindowImage();
	pTex->set();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// find the position
	int tL=444;
	switch (m_state)
	{
	case RadioButton::SELECTED: tL = 444; break;
	case RadioButton::NOT_SELECTED: tL = 426; break;
	case RadioButton::INACTIVE: tL = 462; break;
	}
	int tR = tL+18;
	int tT = 3;
	int tB=tT+18;

	// render the icon
	glColor4f(1,1,1,1);
	if (m_bHover && m_state != RadioButton::INACTIVE)
		glColor4f(1,1,1,0.8);
	glBegin(GL_QUADS);
		glTexCoord2f(tL/512.0, tT/512.0);	glVertex3f(m_left, m_top, 0);
		glTexCoord2f(tR/512.0, tT/512.0);	glVertex3f(m_left+18, m_top, 0);
		glTexCoord2f(tR/512.0, tB/512.0);	glVertex3f(m_left+18, m_bottom, 0);
		glTexCoord2f(tL/512.0, tB/512.0);	glVertex3f(m_left, m_bottom, 0);
	glEnd();
		
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render the text
	glColor3f(0.3,0.3,0.3);
	if (m_state == RadioButton::INACTIVE)
		glColor3f(0.6, 0.6, 0.6);
	Font::renderString(m_left + 23, m_bottom-5, m_title);
}

void RadioButton::onMouseDown(int x, int y, int button)
{
	if (m_state == RadioButton::INACTIVE)
		return;

	// Deselect all other radio buttons in the group
	if (m_bIsRadio && m_pParent)
	{
		//Container *pCont = dynamic_cast<Container*>(m_pParent);
		//if (pCont)	// if parent is a container
		// dynamic_cast does not work here...
		Container *pCont = (Container*)m_pParent;

		int nc = pCont->getChildrenNum();
		for (int i=0; i<nc; ++i)
		{
			try {
				RadioButton *pRB = dynamic_cast<RadioButton*>(pCont->getChild(i));
				if (pRB && pRB!=this)	// if its a radio button
				{
					// deselect it
					if (pRB->getState() == RadioButton::SELECTED)
						pRB->setState(RadioButton::NOT_SELECTED);
				}
			}
			catch (std::bad_cast) {
			}
		}
	}
	
	if (m_bIsRadio)
		m_state = RadioButton::SELECTED;
	else
		m_state = (m_state == RadioButton::SELECTED)?RadioButton::NOT_SELECTED:RadioButton::SELECTED;

	// Call the callback, if any
	if (m_pCallback)
		m_pCallback(m_id);
}

void RadioButton::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (isPtInside(x,y))
		m_bHover = true;
	else
		m_bHover = false;
}

void RadioButton::onMouseUp(int x, int y, int button)
{
}

void RadioButton::onKeyDown(int key)
{
}

void RadioButton::onKeyUp(int key)
{
}

bool RadioButton::isPtInside(int x, int y)
{
	if (x<m_left || x>m_left+18)
		return false;
	if (y<m_top || y>m_top+18)
		return false;
	return true;
}
