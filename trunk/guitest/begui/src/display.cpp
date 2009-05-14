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
	int fw = FrameWindow::inst()->getRight()-FrameWindow::inst()->getLeft();
	int fh = FrameWindow::inst()->getBottom()-FrameWindow::inst()->getTop();

	if (FrameWindow::inst()->getOptions().bOwnDraw) {
		x -= FrameWindow::inst()->getLeft();
		y -= FrameWindow::inst()->getTop();
	}

	glScissor(x,fh-y-h+1,w,h);
	glEnable(GL_SCISSOR_TEST);

/*	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, fh, fh, 0, -1, 10);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glColor4f(0,0,0,0);
	glBegin(GL_QUADS);
		glVertex3f(0 , 0 , -1);
		glVertex3f(fw, 0 , -1);
		glVertex3f(fw, fh, -1);
		glVertex3f(0 , fh, -1);
	glEnd();

	// draw a quad to the depth buffer, at depth 0
	glBegin(GL_QUADS);
		glVertex3f(x  ,y  ,100);
		glVertex3f(x+w,y  ,100);
		glVertex3f(x+w,y+h,100);
		glVertex3f(x  ,y+h,100);
	glEnd();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);*/
}

// Remove any previous masks. Rendering can be done anywhere
// on the screen after this call.
void display::unmask()
{
	int fw = FrameWindow::inst()->getRight()-FrameWindow::inst()->getLeft();
	int fh = FrameWindow::inst()->getBottom()-FrameWindow::inst()->getTop();

	glDisable(GL_SCISSOR_TEST);
/*
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, fh, fh, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
		glVertex3f(0 , 0 , 100);
		glVertex3f(fw, 0 , 100);
		glVertex3f(fw, fh, 100);
		glVertex3f(0 , fh, 100);
	glEnd();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();*/
}