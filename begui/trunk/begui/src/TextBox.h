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

#pragma once

#include "common.h"
#include "Component.h"
#include "EditableText.h"

namespace begui {

class TextBox : public Component
{
private:
	//std::string	m_text;
	EditableText m_text;
	//bool		m_bEditable;
	//bool		m_bMultiline;
	bool		m_bTextSelectable;
	//int			m_selStart, m_selEnd;
	//int			m_cursorPos;

	//std::vector<Vector2> m_charPos;
	/*class Line
	{
	public:
		int x, y;
		int lineHeight;
		std::vector<int> charOffs;
		std::vector<char> chars;
		std::vector<int> charW;
		int	startTextPos;
		int startCursorPos;
	};
	std::vector<Line> m_lines;
	int m_cursorSteps;*/

public:
	TextBox();

	virtual void create(int x, int y, int width, int height, bool bEditable=true, bool bMultiline=true);

	virtual void frameUpdate();
	virtual void frameRender();
	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);
	virtual bool isPtInside(int x, int y);

	//std::string	getText() const						{ return m_text; }
	//void		setText(const std::string& text)	{ m_text = text; m_cursorPos = (int)m_text.length(); }
	bool		isEditable() const					{ return true; }//m_bEditable; }
	bool		isMultiline() const					{ return true; }//m_bMultiline; }
	std::string	getSelectedText() const;

private:
	//int			getCharPosAt(int x, int y);
};

};