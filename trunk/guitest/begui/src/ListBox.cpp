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

#include "ListBox.h"
#include "util.h"
#include "Font.h"

using namespace begui;

ListBox::ListBox() : 
	m_curItem(-1), m_bSelectMultiple(false), m_selStart(-1), m_selEnd(-1)
{
}

void ListBox::create(int x, int y, int width, int height, bool bSelectMultiple)
{
	setPos(x,y);
	setSize(width, height);
	m_bSelectMultiple = bSelectMultiple;

	m_scroller.create(x+width-ScrollBar::SCROLL_WIDTH, y, height, ScrollBar::SCROLL_VERTICAL);

	m_items.push_back("test1");
	m_items.push_back("test2");
	m_items.push_back("test3");
}

void ListBox::frameUpdate()
{
	m_scroller.frameUpdate();
}

void ListBox::frameRender()
{
	Vector2 wpos = Component::localToWorld(Vector2(getLeft(), getTop()));
	display::maskRect(wpos.x, wpos.y, getWidth()+1-ScrollBar::SCROLL_WIDTH, getHeight()+1);

	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);

	// render the listbox background
	glBegin(GL_QUADS);
		for (int ln=0; ln<=(getBottom() - getTop())/pFont->getLineHeight(); ++ln)
		{
			if (ln%2 == 0)
				glColor4f(0.85, 0.93, 1, 1);
			else
				glColor4f(1,1,1,1);
			
			int bottom = getTop() + (ln+1)*pFont->getLineHeight();
			if (bottom > getBottom())
				bottom = getBottom();
			glVertex3f(getLeft(), getTop() + ln*pFont->getLineHeight(), 0);
			glVertex3f(getRight()-ScrollBar::SCROLL_WIDTH, getTop() + ln*pFont->getLineHeight(), 0);
			glVertex3f(getRight()-ScrollBar::SCROLL_WIDTH, bottom, 0);
			glVertex3f(getLeft(), bottom, 0);
		}
	glEnd();

	// render a frame
	glColor4f(0,0,0,0.8);
	glBegin(GL_LINES);
		glVertex2f(getLeft(), getTop());
		glVertex2f(getRight()-ScrollBar::SCROLL_WIDTH, getTop());
		glVertex2f(getRight()-ScrollBar::SCROLL_WIDTH, getTop());
		glVertex2f(getRight()-ScrollBar::SCROLL_WIDTH, getBottom());
		glVertex2f(getRight()-ScrollBar::SCROLL_WIDTH, getBottom());
		glVertex2f(getLeft(), getBottom());
		glVertex2f(getLeft(), getBottom());
		glVertex2f(getLeft(), getTop());
	glEnd();

	// render the items
	if (isEnabled())
		glColor4f(0,0,0,0.9);
	else
		glColor4f(0,0,0,0.3);
	for (size_t i=0; i<m_items.size(); ++i)
	{
		Font::renderString(getLeft() + 5, getTop() + (i+1)*pFont->getLineHeight() - 2, m_items[i]);
	}

	display::unmask();

	glColor4f(1,1,1,1);
	m_scroller.frameRender();
}

void ListBox::onMouseDown(int x, int y, int button)
{
	Component::onMouseDown(x,y,button);
}

void ListBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	Component::onMouseMove(x,y,prevx,prevy);
}

void ListBox::onMouseUp(int x, int y, int button)
{
	Component::onMouseUp(x,y,button);
}

void ListBox::onKeyDown(int key)
{
	Component::onKeyDown(key);
}

void ListBox::onKeyUp(int key)
{
	Component::onKeyUp(key);
}
