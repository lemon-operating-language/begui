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

#include "BaseApp_Win.h"
#include "util.h"
#include "FrameWindow.h"
#include "Font.h"
#include "ResourceManager.h"

using namespace begui;

BaseApp_Win	*BaseApp_Win::m_pInst = NULL;


BaseApp_Win::BaseApp_Win() : m_bSyncRendering(true)
{
}

BaseApp_Win::~BaseApp_Win()
{
}

bool BaseApp_Win::initialize(const std::string &title, size_t width, size_t height, Window::Style frame_style)
{
	// set the desired options of the new window
	FrameWindow::Options opt;

	// Create Our OpenGL Window
	try {
		FrameWindow::inst()->create(0, 0, width, height, title, 0);
	}
	catch (std::exception &e) {
		Console::error("Failed to create main window: %s\n", e.what());
		return false;
	}

	// do any extra initialization here
	if (!onCreate())
		return false;

	return true;
}

void BaseApp_Win::resize(int width, int height)
{
//	display::setSize(width, height);
//	FrameWindow::inst()->setSize(width, height);
}

void BaseApp_Win::updateFrame()
{
	// update all timeseries objects with the current time
	Updater::inst()->update_all_current_time();

	// update the main window
	FrameWindow::inst()->frameUpdate();
}

void BaseApp_Win::renderFrame()
{
	FrameWindow::inst()->frameRender();
}

int BaseApp_Win::run()
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == 0)
			{
				// Draw The Scene.  Watch for Quit Messages From DrawGLScene()
				if (FrameWindow::inst()->isActive())				// Program Active?
				{
					updateFrame();
					if (m_bSyncRendering)
						renderFrame();
				}
			}
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch for Quit Messages From DrawGLScene()
			if (FrameWindow::inst()->isActive())
			{
				updateFrame();
				if (m_bSyncRendering)
					renderFrame();
			}
		}
	}

	// Shutdown
	FrameWindow::inst()->free();

	return (msg.wParam);
}