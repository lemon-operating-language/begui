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
#include "ListBox.h"
#include "Container.h"

namespace begui {

class ComboBox : public Container
{
protected:
	ListBox				m_listbox;
	int					m_curItem;
	EditableText		m_text;
	bool				m_bEditable;	// text in the text area is editable

	bool				m_bIsOpen;	// true if the list is open
	int					m_btnW, m_btnH;	// dimensions of the button to extend the list

public:
	ComboBox();
	virtual ~ComboBox();

	virtual void create(int x, int y, int width, int list_height);

	virtual void onUpdate();
	virtual void onRender();
	
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	virtual bool isPtInside(int x, int y);

	void		addItem(const std::string &item)	{ m_listbox.addItem(item); }
	int			itemsNum() const					{ return m_listbox.itemsNum(); }
	bool		itemEnabled(size_t i) const			{ return m_listbox.itemEnabled(i); }
	void		remItem(int pos)					{ m_listbox.remItem(pos); }
	void		remAllItems()						{ m_listbox.remAllItems(); }
	int			getCurrent() const					{ return m_curItem; }
	void		enableItem(size_t i)				{ m_listbox.enableItem(i); }
	void		disableItem(size_t i)				{ m_listbox.disableItem(i); }

	void	onItemClick(int i);
};

};