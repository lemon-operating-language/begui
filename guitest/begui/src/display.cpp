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

#include "util.h"
#include "FrameWindow.h"

using namespace begui;

int	g_displayWidth = 0;
int g_displayHeight = 0;

int display::getWidth()
{
	return g_displayWidth;
}

int display::getHeight()
{
	return g_displayHeight;
}

void display::setSize(int w, int h)
{
	ASSERT(w >= 0);
	ASSERT(h >= 0);

	g_displayWidth = w;
	g_displayHeight = h;
}

// Mask the screen except this given rectangle. Any
// rendering after this call will be restricted only within
// this rectangle
void display::maskRect(int x, int y, int w, int h)
{
	Rect<int> fb = FrameWindow::inst()->getInactiveBorders();

	int fw = FrameWindow::inst()->getRight()-FrameWindow::inst()->getLeft() + (fb.left+fb.right);
	int fh = FrameWindow::inst()->getBottom()-FrameWindow::inst()->getTop() + (fb.top+fb.bottom);

	if (FrameWindow::inst()->getOptions().bOwnDraw) {
		x -= FrameWindow::inst()->getLeft();
		y -= FrameWindow::inst()->getTop();
	}

	glScissor(x,fh-y-h+1,w,h);
	glEnable(GL_SCISSOR_TEST);
}

// Remove any previous masks. Rendering can be done anywhere
// on the screen after this call.
void display::unmask()
{
	glDisable(GL_SCISSOR_TEST);
}