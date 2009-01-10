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
	m_bTextSelectable(true)//, m_selStart(0), m_selEnd(0),
	//m_cursorPos(0),
	//m_cursorSteps(0)
{
}

void TextBox::create(int x, int y, int width, int height, bool bEditable, bool bMultiline)
{
//width=50;
	m_left = x;
	m_right = x+width;
	m_top = y;
	m_bottom = y+height;
	//m_bEditable = bEditable;
	//m_bMultiline = bMultiline;

	m_text.create(2,0,width, bMultiline, bEditable);
	m_text.setText("aa aaaaaaaaa aaaaaaaaaaa\naagbdf\ndgdrh");
}

void TextBox::onUpdate()
{
	m_text.update();
}

void TextBox::onRender()
{
	Vector2 wpos = Component::localToWorld(Vector2(0, 0));
	display::maskRect(wpos.x-1, wpos.y, getWidth()+1, getHeight()+1);

	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);

	int w = getWidth();
	int h = getHeight();

	// render the textbox background
	glBegin(GL_QUADS);
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
	glEnd();

	if (m_text.isEditable())
		glColor4f(0,0,0,0.5);
	else
		glColor4f(0,0,0,0.2);

	m_text.renderString();

	display::unmask();
}

void TextBox::onMouseDown(int x, int y, int button)
{
	/*if (button == BEGUI_MOUSE_BUTTON_LEFT)
	{
		int charPos = getCharPosAt(x,y);
		if (charPos != -1) {
			m_cursorPos = charPos;
			m_selStart = charPos;
			m_selEnd = charPos;
		}
	}*/
	m_text.onMouseDown(x, y, button);

	Component::onMouseDown(x,y,button);
}

void TextBox::onMouseMove(int x, int y, int prevx, int prevy)
{
	/*if (input::isMouseButtonDown(BEGUI_MOUSE_BUTTON_LEFT))
	{
		int charPos = getCharPosAt(x,y);
		if (charPos != -1) {
			m_selEnd = charPos;
			m_cursorPos = charPos;
		}
	}*/
	m_text.onMouseMove(x, y, prevx, prevy);

	Component::onMouseMove(x,y,prevx,prevy);
}

void TextBox::onMouseUp(int x, int y, int button)
{
	/*if (button == BEGUI_MOUSE_BUTTON_LEFT)
	{
		int charPos = getCharPosAt(x,y);
		if (charPos != -1) {
			if (charPos < m_selStart) {
				m_selEnd = m_selStart;
				m_selStart = charPos;
			}
			else
				m_selEnd = charPos;
			m_cursorPos = charPos;
		}
	}
	if (input::isMouseButtonDown(BEGUI_MOUSE_BUTTON_RIGHT))
	{
		// display a pop-up menu
		//TODO
	}*/
	m_text.onMouseUp(x, y, button);

	Component::onMouseUp(x,y,button);
}

void TextBox::onKeyDown(int key)
{
	/*Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);

	// get the current cursor position
	int curline = 0;
	int curlinepos = 0;
	for (int i = 0; i<m_cursorPos; ++i)
	{
		// walk through the lines
		curlinepos++;
		if (curlinepos >= m_lines[curline].charOffs.size()) {
			curline++;
			curlinepos = 0;
		}
	}

	switch (key)
	{
	// take care of backspace and navigation keys
	case BEGUI_KEY_LEFT:
		if (m_cursorPos > 0)
			m_cursorPos--;
		break;
	case BEGUI_KEY_RIGHT:
		if (m_cursorPos < m_cursorSteps-1)
			m_cursorPos++;
		break;
	case BEGUI_KEY_UP:
		if (m_bMultiline && curline > 0)
			m_cursorPos = m_lines[curline-1].startCursorPos + 
			((curlinepos > m_lines[curline-1].charOffs.size()-1) ? m_lines[curline-1].charOffs.size()-1 : curlinepos);
		break;
	case BEGUI_KEY_DOWN:
		if (m_bMultiline && curline < m_lines.size()-1)
			m_cursorPos = m_lines[curline+1].startCursorPos + 
			((curlinepos > m_lines[curline+1].charOffs.size()-1) ? m_lines[curline+1].charOffs.size()-1 : curlinepos);
		break;
	case '\b':
		if (m_bEditable && m_cursorPos > 0) {
			m_text.erase(m_cursorPos-1, 1);
			m_cursorPos--;
		}
		break;
	default:
		// if a printable character, add it to the cursor position
		if (m_bEditable)
		{
			if (key < 256 && (isspace(key) || isprint(key)))
			{
				char str[2] = {key, 0};
				m_text.insert(m_cursorPos, str);
				m_cursorPos++;
			}
		}
	}*/
	m_text.onKeyDown(key);

	Component::onKeyDown(key);
}

void TextBox::onKeyUp(int key)
{
	m_text.onKeyUp(key);

	Component::onKeyUp(key);
}

bool TextBox::isPtInside(int x, int y)
{
	if (x < m_left || x > m_right)
		return false;
	if (y < m_top || y > m_bottom)
		return false;
	return true;
}

/*
int TextBox::getCharPosAt(int x, int y)
{
	Font *pFont = FontManager::getCurFont();
	ASSERT(pFont);
	int line = -1;

	x -= 4;
	y -= 4;

	//Console::print("char at: %d, %d\n", x, y);

	// locate the line
	for (size_t i=0; i<m_lines.size(); ++i)
	{
		if (y >= m_lines[i].y - m_lines[i].lineHeight &&
			y <= m_lines[i].y)
		{
			line = i;
			break;
		}
	}
	if (line == -1)
	{
		if (y < m_lines[0].y-m_lines[0].lineHeight)
			return 0;
		if (y > m_lines.back().y)
			return m_cursorSteps-1;
	}
	else
	{
		// locate the character in the line
		if (x < m_lines[line].x)
			return m_lines[line].startCursorPos;
		for (size_t j=1; j<m_lines[line].charOffs.size(); ++j)
		{
			int cpos = m_lines[line].startTextPos + j-1;
			int c = m_text[cpos];
			if (!pFont->getChar(c))
				continue;

			if (x >= m_lines[line].x + m_lines[line].charOffs[j] &&
				x <= m_lines[line].x + m_lines[line].charOffs[j] + pFont->getChar(c)->m_horiAdvance)
			{
				return m_lines[line].startCursorPos + j;
			}
		}
		TextBox::Line lineS = m_lines[line];
		return m_lines[line].startCursorPos + m_lines[line].charOffs.size()-1;
	}
	return 0;
}*/