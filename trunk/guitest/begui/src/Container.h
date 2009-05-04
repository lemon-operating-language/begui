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

namespace begui {

class Container : public Component
{
protected:
	// The list of child components. The children are stored in inverse z-order (the top
	// component is the last in the list)
	std::vector<Component*>	m_children;
	Component				*m_pActiveComponent;

public:
	Container();
	virtual ~Container();
	
	void		addComponent(Component *pC);
	int			getChildrenNum() const	{ return (int)m_children.size(); }
	Component*	getChild(int pos)		{ return m_children[pos]; }
	void		remComponent(Component *pC);

	// overridables
	virtual void onMouseDownEx(int x, int y) { };
	virtual void onMouseMoveEx(int x, int y, int prevx, int prevy) { };
	virtual void onMouseUpEx(int x, int y) { };
	virtual void onKeyDownEx(int key) { };
	virtual void onKeyUpEx(int key) { };

	virtual void frameUpdate();
	virtual void frameRender();

protected:
	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);

	void bringChildToFront(int id);
};

};