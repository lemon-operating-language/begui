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

#include "ComboBox.h"
#include "Font.h"
#include "ResourceManager.h"

using namespace begui;

ComboBox::ComboBox() : m_bIsOpen(false), m_bEditable(false), m_curItem(0)
{
}

ComboBox::~ComboBox()
{
}

void ComboBox::create(int x, int y, int width, int list_height)
{
	m_btnW = 14;
	m_btnH = 14;
	setPos(x,y);
	setSize(width, m_btnH);

	m_listbox.create(0, m_btnH, width-m_btnW, list_height, ListBox::SINGLE_SELECT, ListBox::STYLE_FLAT);
	m_listbox.setHighlightOnMouseOver(true);
	m_listbox.handleOnItemClick(makeFunctor(*this, &ComboBox::onItemClick));
	m_bIsOpen = false;
	m_curItem = 0;
	
	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);
	m_text.create(2, m_btnH-pFont->getLineHeight()-3, false, m_bEditable, m_bEditable);
	m_text.setText("<select>");
}

void ComboBox::onUpdate()
{
}

void ComboBox::onRender()
{
	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);
	int lineHeight = pFont->getLineHeight()+2;

	int w = getWidth()-m_btnW;
	int h = (m_btnH > lineHeight) ? m_btnH : lineHeight;
	int bottom = getHeight();
	
	glEnable(GL_BLEND);

	int tL = 369;
	int tT = 2;
	int tR = tL + 16;
	int tB = tT + 16;

	// draw background
	glBegin(GL_QUADS);
		glColor4f(1,1,1,0.7);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, bottom);
		glVertex2f(0, bottom);
		
		/*glColor4f(0,0,0,0.7);
		glVertex2f(w+1, 0);
		glVertex2f(w+1+m_btnW, 0);
		glVertex2f(w+1+m_btnW, bottom);
		glVertex2f(w+1, bottom);*/
	glEnd();
		
	// set the texture of a window
	Texture *pTex = ResourceManager::inst()->getStockMap(ResourceManager::STD_CONTROLS);
	pTex->set();
	glBegin(GL_QUADS);
		glColor4f(1,1,1,1);
		glTexCoord2f(tL/512.0, tT/512.0);	glVertex2f(w+1, 0);
		glTexCoord2f(tR/512.0, tT/512.0);	glVertex2f(w+1+m_btnW, 0);
		glTexCoord2f(tR/512.0, tB/512.0);	glVertex2f(w+1+m_btnW, bottom);
		glTexCoord2f(tL/512.0, tB/512.0);	glVertex2f(w+1, bottom);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// draw a line frame around the list box
	glColor4f(0,0,0,0.4);
	glBegin(GL_LINES);
		glVertex2f(0, 1);
		glVertex2f(w, 1);
		glVertex2f(w, 1);
		glVertex2f(w, bottom);
		glVertex2f(w, bottom);
		glVertex2f(0, bottom);
		glVertex2f(0, bottom);
		glVertex2f(0, 1);
	glEnd();

	if (m_bEditable)
		m_text.setTextColor(Color(0,0,0), 0.4f);
	else
		m_text.setTextColor(Color(0.5f,0.5f,0.5f), 0.8f);
	m_text.renderString();
}

bool ComboBox::onMouseDown(int x, int y, int button)
{
	// if the expand button was clicked:
	if (x >= m_right-m_btnW && x<=m_right && y>=m_top && y<=m_top+m_btnH) {
		if (!m_bIsOpen) {
			m_bIsOpen = true;
			addComponent(&m_listbox);
		}
		else {
			m_bIsOpen = false;
			remComponent(&m_listbox);
		}
	}
	return Container::onMouseDown(x,y,button);
}

bool ComboBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	return Container::onMouseMove(x,y,prevx,prevy);
}

bool ComboBox::onMouseUp(int x, int y, int button)
{
	return Container::onMouseUp(x,y,button);
}

void ComboBox::onKeyDown(int key)
{
	Container::onKeyDown(key);
}

void ComboBox::onKeyUp(int key)
{
	Container::onKeyUp(key);
}

bool ComboBox::isPtInside(int x, int y)
{
	if (m_bIsOpen) {
		Vector2i lPt = Component::parentToLocal(Vector2i(x,y));
		if (m_listbox.isPtInside(lPt.x,lPt.y))
			return true;
	}
	if (x<m_left || x>m_right || y<m_top || y>m_bottom)
		return false;
	return true;
}

void ComboBox::onItemClick(int i)
{
	// set the new selected item
	if (i >= 0) {
		m_text.setText(m_listbox.itemText(i));
		m_curItem = i;
	}

	// close the listbox
	m_bIsOpen = false;
	remComponent(&m_listbox);
}