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

#include "TextBox.h"
#include "Font.h"
#include "util.h"

using namespace begui;

TextBox::TextBox() : //m_bEditable(false), m_bMultiline(false),
	m_bTextSelectable(true),
	m_textPadding(0,0,0,0),
	m_textColor(0.1f,0.1f,0.1f)
{
}

void TextBox::create(int x, int y, int width, int height, bool bEditable, bool bMultiline,
					 const std::string &style_name)
{
	m_left = x;
	m_right = x+width;
	m_top = y;
	m_bottom = y+height;

	// load the textbox stylesheet
	ResourceManager::Style style = ResourceManager::inst()->getClassDef("TextBox").style(style_name);
	m_bg = ResourceManager::inst()->loadImage(style.get_img("face"));
	m_activeArea = style.get_rect("active_area");
	m_resizableArea = style.get_rect("resizable_area");
	if (style.hasProp("text_padding"))
		m_textPadding = style.get_rect("text_padding");
	if (style.hasProp("text_color"))
		m_textColor = style.get_c("text_color");

	// create the text object
	m_text.create(m_textPadding.left, m_textPadding.top, width-m_textPadding.right, bMultiline, bEditable);
	m_text.setTextColor(m_textColor, (bEditable)?0.9f:0.5f);
	m_text.setText("aa aaaaaaaaa aaaaaaaaaaa\naagbdf\ndgdrh");
}

void TextBox::onUpdate()
{
	m_text.update();
}

void TextBox::onRender()
{
	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);

	int w = getWidth();
	int h = getHeight();

	// render the textbox background
	/*glBegin(GL_QUADS);
		for (int ln=0; ln<=(m_bottom - m_top)/pFont->getLineHeight(); ++ln)
		{
			if (m_text.isEditable())
			{
				if (ln%2 == 0)
					glColor4f(0.93, 0.97, 1, 1);
				else
					glColor4f(1,1,1,1);
			}
			else
				glColor4f(0,0,0,0.1);
			
			int bottom = (ln+1)*pFont->getLineHeight();
			if (bottom > h)
				bottom = h;
			glVertex3f(0, ln*pFont->getLineHeight(), 0);
			glVertex3f(w, ln*pFont->getLineHeight(), 0);
			glVertex3f(w, bottom, 0);
			glVertex3f(0, bottom, 0);
		}
	glEnd();
	
	glColor4f(0.2, 0.2, 0.2, 0.5);
	glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, 0);
		glVertex2f(w, h);
		glVertex2f(w, h);
		glVertex2f(0, h);
		glVertex2f(0, h);
		glVertex2f(0, 0);
	glEnd();*/

	// render the background
	glEnable(GL_BLEND);
	glColor4f(1,1,1,1);
	Component::drawImageWtBorders(m_bg, -m_activeArea.left, -m_activeArea.top, 
		getWidth()+m_activeArea.left + (m_bg.m_width-m_activeArea.right), 
		getHeight()+m_activeArea.top + (m_bg.m_height-m_activeArea.bottom), 
		m_resizableArea);

	Vector2i wpos = Component::localToWorld(Vector2i(0, 0));
	display::pushMask(wpos.x, wpos.y, getWidth(), getHeight());

	// render the text
	if (m_text.isEditable())
		glColor4f(m_textColor.r*255, m_textColor.g*255, m_textColor.b*255, 0.5f);
	else
		glColor4f(m_textColor.r*255, m_textColor.g*255, m_textColor.b*255, 0.2f);
	m_text.renderString();

	display::popMask();
}

bool TextBox::onMouseDown(int x, int y, int button)
{
	m_text.onMouseDown(x-m_left, y-m_top, button);

	return Component::onMouseDown(x,y,button);
}

bool TextBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	m_text.onMouseMove(x-m_left, y-m_top, prevx-m_left, prevy-m_top);

	return Component::onMouseMove(x,y,prevx,prevy);
}

bool TextBox::onMouseUp(int x, int y, int button)
{
	m_text.onMouseUp(x-m_left, y-m_top, button);

	return Component::onMouseUp(x,y,button);
}

void TextBox::onKeyDown(int key)
{
	m_text.onKeyDown(key);

	Component::onKeyDown(key);
}

void TextBox::onKeyUp(int key)
{
	m_text.onKeyUp(key);

	Component::onKeyUp(key);
}