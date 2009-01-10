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

class Group : public Container
{
public:
	enum Frame {
		NO_FRAME,
		SQUARE,
		ROUNDED
	};

private:
	std::string m_title;
	Frame	m_frameStyle;
	Color	m_frameColor;

public:
	Group();

	void create(int left, int top, int width, int height, const std::string &title="");
	virtual void onRender();
	virtual bool isPtInside(int x, int y);
};

};