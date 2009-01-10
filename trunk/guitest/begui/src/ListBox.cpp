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
	m_curItem(0), m_prevItem(-1), m_selectMode(MULTI_SELECT), m_style(STYLE_FLAT),
	m_bHighlightMouseOver(false), m_mouseOverItem(-1)
{
}

ListBox::~ListBox()
{
}

void ListBox::create(int x, int y, int width, int height, SelectionMode selMode, Style style)
{
	setPos(x,y);
	setSize(width, height);
	m_selectMode = selMode;
	m_style = style;

	m_scroller.create(width-ScrollBar::SCROLL_WIDTH, 0, height, ScrollBar::SCROLL_VERTICAL);
}

void ListBox::onUpdate()
{
	m_scroller.frameUpdate();
}

void ListBox::onRender()
{
	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);
	int lineHeight = pFont->getLineHeight()+2;
	if (m_style == STYLE_BUTTONS)
		int lineHeight = pFont->getLineHeight()+4;
	
	int content_height = m_items.size()*lineHeight;
	bool bNeedsScrolling = (content_height > getHeight());
	int content_y_offs = 0;
	if (bNeedsScrolling) {
		m_scroller.setBounds(0, m_items.size()-getHeight()/lineHeight, (double)getHeight()/(content_height));
		content_y_offs = -(int)(m_scroller.getPos()*lineHeight);
	}
	
	Vector2 wpos = Component::localToWorld(Vector2(0, 0));
	display::maskRect(wpos.x-1, wpos.y, getWidth()+2-((bNeedsScrolling)? ScrollBar::SCROLL_WIDTH : 0), getHeight()+1);

	glEnable(GL_BLEND);

	int w = getWidth()-((bNeedsScrolling)? ScrollBar::SCROLL_WIDTH+1 : 0);
	int h = getHeight();

	// draw background
	if (m_style == STYLE_BUTTONS)
		glColor4f(0,0,0,0.3);
	else
		glColor4f(1,1,1,0.7);
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, h);
		glVertex2f(0, h);
	glEnd();
	
	// draw a line frame around the list box
	if (m_style == STYLE_BUTTONS)
		glColor4f(0,0,0,0.8);
	else
		glColor4f(0,0,0,0.4);
	glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, 0);
		glVertex2f(w, h);
		glVertex2f(w, h);
		glVertex2f(0, h);
		glVertex2f(0, h);
		glVertex2f(0, 0);
	glEnd();

	// draw items
	for (size_t i=0; i<m_items.size(); ++i)
	{
		int top = 2 + (int)i*lineHeight;
		int bottom = top + lineHeight - 2;
		int left = 2;
		int right = w - 2;
		Color textCl(0.2,0.2,0.2);
		float textAlpha = 0.9;
		if (!m_items[i].m_bEnabled)
			textAlpha = 0.2;
		Color bgCl(1,1,1);
		float bgAlpha = 0.8;

		switch (m_style) {
			case STYLE_FLAT:
				top = (int)i*lineHeight;
				bottom = top + lineHeight;
				left = 0;
				right = w-1;
				if (m_items[i].m_bSelected)
					bgCl = Color(0.5,0.7,1);
				if (i == m_mouseOverItem && m_bHighlightMouseOver)
					bgCl = Color(0.7,0.85,1);
				break;
			case STYLE_STRIPES:
				top = (int)i*lineHeight;
				bottom = top + lineHeight;
				left = 0;
				right = w-1;
				if (m_items[i].m_bEnabled && m_items[i].m_bSelected)
					bgCl = Color(0.5,0.7,1);
				else if (i%2 == 0)
					bgCl = Color(1,1,1);
				else
					bgCl = Color(0.83, 0.90, 0.95);
				bgAlpha = 1;
				break;
			case STYLE_BUTTONS:
				if (!m_items[i].m_bEnabled) {
					bgCl = Color(1,1,1);
					bgAlpha = 0.2;
				}
				else if (m_items[i].m_bSelected)
					bgCl = Color(1,0.7,0.2);
				else
					bgCl = Color(0.5,0.7,1);
				break;
		}

		// add the offset from scrolling
		top += content_y_offs;
		bottom += content_y_offs;

		m_items[i].m_rect = Rect<int>(left, right, top, bottom);

		// do some culling of invisible items to reduce rendering needs
		if (bottom < 0)
			continue;
		if (top > h)
			continue;
		
		// draw a rectange as item background
		glColor4f(bgCl.r, bgCl.g, bgCl.b, bgAlpha);
		if (m_style != STYLE_FLAT ||
			(m_items[i].m_bEnabled && m_items[i].m_bSelected) ||
			(i==m_mouseOverItem && m_bHighlightMouseOver)) {
			glBegin(GL_QUADS);
				glVertex2f(left, top);
				glVertex2f(right, top);
				glVertex2f(right, bottom);
				glVertex2f(left, bottom);
			glEnd();
		}
		
		// draw a line frame for the item
		if (m_style == STYLE_BUTTONS) {
			glColor4f(0,0,0,0.5);
			glBegin(GL_LINES);
				glVertex2f(left, top);
				glVertex2f(right, top);
				glVertex2f(right, top);
				glVertex2f(right, bottom);
				glVertex2f(right, bottom);
				glVertex2f(left, bottom);
				glVertex2f(left, bottom);
				glVertex2f(left, top);
			glEnd();
		}
		else if (m_style == STYLE_FLAT && i!=m_items.size()-1) {
			// draw lines separating the items
			glColor4f(0.4,0.4,0.4,0.3);
			glBegin(GL_LINES);
				glVertex2f(right, bottom);
				glVertex2f(left, bottom);
			glEnd();
		}

		glColor4f(textCl.r, textCl.g, textCl.b, textAlpha);
		Font::renderString(left + 2, bottom-3, m_items[i].m_text.getText());
	}
	
	display::unmask();

	if (bNeedsScrolling) {
		glColor4f(1,1,1,1);
		m_scroller.frameRender();
	}
}

void ListBox::onMouseDown(int x, int y, int button)
{
	if (m_scroller.isPtInside(x, y)) {
		Vector2 lPt = m_scroller.parentToLocal(Vector2(x,y));
		m_scroller.onMouseDown(lPt.x, lPt.y, button);
		return;
	}

	if (button == MOUSE_BUTTON_LEFT) {
		for (size_t i=0; i<m_items.size(); ++i)
		{
			if (m_items[i].m_bEnabled && m_items[i].m_rect.contains(x,y))
			{
				// item i clicked

				if (m_selectMode == MULTI_SELECT_SINGLECLICK) {
					m_items[i].m_bSelected = !m_items[i].m_bSelected;
					m_prevItem = i;
				}
				else if (m_selectMode == SINGLE_SELECT)
				{
					deselectAll();
					m_items[i].m_bSelected = true;
					m_prevItem = i;
				}
				else if (m_selectMode == MULTI_SELECT)
				{
					if (input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)) {
						m_items[i].m_bSelected = !m_items[i].m_bSelected;
						m_prevItem = i;
					}
					else if (input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT))
					{
						if (m_prevItem == -1)
							m_prevItem = i;
						selectRange(m_prevItem, i);
					}
					else {
						deselectAll();
						if (m_items[i].m_bEnabled)
							m_items[i].m_bSelected = true;
						m_prevItem = i;
					}
				}

				m_curItem = i;

				// call the event handler for item click
				m_onItemClick(i);
				
				return;
			}
		}
	}
	Component::onMouseDown(x,y,button);
}

void ListBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	if (m_scroller.isPtInside(x, y)) {
		Vector2 lPt = m_scroller.parentToLocal(Vector2(x,y));
		Vector2 lpPt = m_scroller.parentToLocal(Vector2(prevx, prevy));
		m_scroller.onMouseMove((int)lPt.x, (int)lPt.y, (int)lpPt.x, (int)lpPt.y);
		return;
	}
	
	// find the item under the mouse cursor
	if (x>=0 && x<getWidth() && y>=0 && y<getHeight()) {
		m_mouseOverItem = -1;
		for (size_t i=0; i<m_items.size(); ++i)
		{
			if (m_items[i].m_bEnabled && m_items[i].m_rect.contains(x,y))
			{
				// item i clicked
				m_mouseOverItem = i;
				break;
			}
		}
	}

	Component::onMouseMove(x,y,prevx,prevy);
}

void ListBox::onMouseUp(int x, int y, int button)
{
	if (m_scroller.isPtInside(x,y)) {
		Vector2 lPt = m_scroller.parentToLocal(Vector2(x,y));
		m_scroller.onMouseUp((int)lPt.x, (int)lPt.y, button);
		return;
	}

	Component::onMouseUp(x,y,button);
}

void ListBox::onKeyDown(int key)
{
	switch (key) {
		case KEY_UP:
			if (m_curItem > 0) {
				m_curItem--;
				if (input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT)) {
					if (m_prevItem == -1) m_prevItem = m_curItem;
					selectRange(m_prevItem, m_curItem);
				}
				else {
					if (m_selectMode != MULTI_SELECT_SINGLECLICK &&
						!(input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)))
						deselectAll();
					m_prevItem = m_curItem;
				}
			}
			break;
		case KEY_DOWN:
			if (m_curItem < m_items.size()-1)
				m_curItem++;
			if (input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT)) {
				if (m_prevItem == -1) m_prevItem = m_curItem;
				selectRange(m_prevItem, m_curItem);
			}
			else {
				if (m_selectMode != MULTI_SELECT_SINGLECLICK &&
					!(input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)))
					deselectAll();
				m_prevItem = m_curItem;
			}
			break;
		case KEY_HOME:
			m_curItem = 0;
			if (input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT)) {
				if (m_prevItem == -1) m_prevItem = m_curItem;
				selectRange(m_prevItem, m_curItem);
			}
			else {
				if (m_selectMode != MULTI_SELECT_SINGLECLICK &&
					!(input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)))
					deselectAll();
				m_prevItem = m_curItem;
			}
			break;
		case KEY_END:
			if (m_items.size() > 0) {
				m_curItem = m_items.size()-1;
				if (input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT)) {
					if (m_prevItem == -1) m_prevItem = m_curItem;
					selectRange(m_prevItem, m_curItem);
				}
				else {
					if (m_selectMode != MULTI_SELECT_SINGLECLICK &&
						!(input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)))
						deselectAll();
					m_prevItem = m_curItem;
				}
			}
			break;
		case ' ':
			switch (m_selectMode) {
			case SINGLE_SELECT:
				for (size_t j=0; j<m_items.size(); ++j)
					m_items[j].m_bSelected = false;
				m_items[m_curItem].m_bSelected = true;
				m_prevItem = m_curItem;
				break;
			case MULTI_SELECT:
				if (input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)) {
					m_items[m_curItem].m_bSelected = !m_items[m_curItem].m_bSelected;
					m_prevItem = m_curItem;
				}
				else {
					if (m_selectMode != MULTI_SELECT_SINGLECLICK &&
						!(input::isKeyDown(KEY_LSHIFT) || input::isKeyDown(KEY_RSHIFT)))
						deselectAll();
					m_items[m_curItem].m_bSelected = true;
					m_prevItem = m_curItem;
				}
				break;
			case MULTI_SELECT_SINGLECLICK:
				m_items[m_curItem].m_bSelected = !m_items[m_curItem].m_bSelected;
				m_prevItem = m_curItem;
				break;
			}
			break;
		case 'A':
		case 'a':	// Ctrl+A
			if (m_selectMode != SINGLE_SELECT && (input::isKeyDown(KEY_LCTRL) || input::isKeyDown(KEY_RCTRL)))
			{			
				for (size_t j=0; j<m_items.size(); ++j) {
					if (!m_items[j].m_bEnabled) continue;
					m_items[j].m_bSelected = true;
				}
			}
			break;
	}
	Component::onKeyDown(key);
}

void ListBox::onKeyUp(int key)
{
	Component::onKeyUp(key);
}

void ListBox::selectRange(size_t start, size_t end)
{
	if (start > end)
		std::swap(start, end);
	
	for (size_t j=0; j<m_items.size(); ++j)
	{
		// only select enabled items
		if (!m_items[j].m_bEnabled) continue;

		// enable items in the selection range
		if (j<start || j>end)
			m_items[j].m_bSelected = false;
		else
			m_items[j].m_bSelected = true;
	}
}

void ListBox::deselectAll()
{
	for (size_t j=0; j<m_items.size(); ++j)
	{
		// only select enabled items
		if (!m_items[j].m_bEnabled) continue;

		m_items[j].m_bSelected = false;
	}
}