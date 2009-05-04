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
	EditableText m_text;
	bool		m_bTextSelectable;

public:
	TextBox();

	virtual void create(int x, int y, int width, int height, bool bEditable=true, bool bMultiline=true);

	virtual void onUpdate();
	virtual void onRender();	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	bool		isEditable() const					{ return m_text.isEditable(); }
	bool		isMultiline() const					{ return m_text.isMultiLine(); }
	std::string	getSelectedText() const;

};

};