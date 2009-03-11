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

class Button : public Component
{
public:
	enum State {
		DOWN,
		UP,
		INACTIVE
	};

private:
	std::string		m_title;
	int				m_id;
	Functor1<int>	m_onClick;	// arg1: the id of the button
	bool			m_bHover;
	State			m_status;

public:
	Button();

	void create(int x, int y, const std::string &title, int id, Functor1<int> &callback = Functor1<int>());

	virtual void onUpdate();
	virtual void onRender();

	void setState(State state)	{ m_status = state; }
	State getState() const		{ return m_status; }
	void setTitle(const std::string& title)	{ m_title = title; }

	virtual void onMouseDown(int x, int y, int button);
	virtual void onMouseMove(int x, int y, int prevx, int prevy);
	virtual void onMouseUp(int x, int y, int button);
	virtual void onKeyDown(int key);
	virtual void onKeyUp(int key);
	virtual bool isPtInside(int x, int y);
};

};