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
#include "ScrollBar.h"

namespace begui {

class ListBox : public Component
{
protected:
	std::vector<std::string> m_items;
	int						m_curItem;
	bool					m_bSelectMultiple;
	int						m_selStart, m_selEnd;

	ScrollBar				m_scroller;

public:
	ListBox();

	virtual void create(int x, int y, int width, int height, bool bSelectMultiple = false);

	virtual void frameUpdate();
	virtual void frameRender();
	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	void		addItem(const std::string &item)	{ m_items.push_back(item); }
	std::string& itemAt(int pos)					{ return m_items[pos]; }
	int			itemsNum() const					{ return (int)m_items.size(); }
	void		remItem(int pos)					{ m_items.erase(m_items.begin()+pos); }
	int			getCurrent() const					{ return m_curItem; }
	int			getSelStart() const					{ return m_selStart; }
	int			getSelEnd() const					{ return m_selEnd; }
};

};