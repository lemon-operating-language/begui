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
#include "Container.h"

namespace begui {

class Window : public Container
{
private:
	std::string				m_title;

	bool			m_bResizing;
	bool			m_bMoving;
	bool			m_bResizable;
	bool			m_bMovable;
	bool			m_bLButtonDown;

public:
	Window();
	virtual ~Window();

	void create(int left, int top, int width, int height, const std::string &title);

	void setMovable(bool bMovable)		{ m_bMovable = bMovable; }
	void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
	
	virtual void onCreate() { };
	virtual void onRender();

	virtual void onMouseDownEx(int x, int y);
	virtual void onMouseMoveEx(int x, int y, int prevx, int prevy);
	virtual void onMouseUpEx(int x, int y);
	virtual bool isPtInside(int x, int y);
};

};