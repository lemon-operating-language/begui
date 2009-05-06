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

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=true;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=false;	// Fullscreen Flag Set To Fullscreen Mode By Default


int lastMousePosX = 0;
int lastMousePosY = 0;

//more:
HDC hMemDC = 0;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

BaseApp_Win::BaseApp_Win() : 
	m_bLayeredWindow(true), 
	m_bOffscreenRendering(false),
	m_bSyncRendering(true)
{
}

BaseApp_Win::~BaseApp_Win()
{
}

bool BaseApp_Win::coreInitialize()
{
	// set some OpenGL states
	glClearColor(0, 0, 0, 0);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if (m_bLayeredWindow)
		glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

	// Get windows directory
	char win_dir[MAX_PATH+1];
	GetWindowsDirectory(win_dir, MAX_PATH);

	// Initialize font subsystem
	if (!FontManager::initialize())
		return false;
	if (!FontManager::setFont(strcat(win_dir, "\\Fonts\\tahoma.ttf"), 11))
		return false;
	
	// Initialize the window manager and load resources
	ResourceManager::inst()->loadResources();

	return true;
}

bool BaseApp_Win::initialize()
{
	return true;
}

void BaseApp_Win::resize(int width, int height)
{
	display::setSize(width, height);
	FrameWindow::inst()->resize(width, height);
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
	if (m_bOffscreenRendering)
		m_frameRenderPass.beginPass();

	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render the main window
	FrameWindow::inst()->frameRender();
	
	if (m_bOffscreenRendering) {
		static std::vector<unsigned char> data(display::getWidth()*display::getHeight()*4);
		static std::vector<unsigned char> data2(display::getWidth()*display::getHeight()*4);

		glReadPixels(0, 0, display::getWidth(), display::getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

		m_frameRenderPass.endPass();

		// convert the data for windows
		int w = display::getWidth();
		int h = display::getHeight();
		for (int j=0; j<h; ++j) {
			int j2 = h-j-1;
			unsigned char *p1 = &data[0] + 4*j2*w;
			unsigned char *p2 = &data2[0] + 4*j*w;
			for (int i=0; i<w; ++i)
			{
				*p2 = *(p1+2);
				*(p2+1) = *(p1+1);
				*(p2+2) = *p1;
				*(p2+3) = *(p1+3);
				p1 += 4;
				p2 += 4;
			}
		}

		// copy the image data to the window
		HDC dc = GetDC(0);
		HBITMAP hBMP = CreateBitmap(w, h, 1, 32, &data2[0]);
		HBITMAP bmpold = (HBITMAP)SelectObject(hMemDC, hBMP);
		SIZE sz = {w, h};
		POINT ptSrc = {0,0};
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		if (!UpdateLayeredWindow(hWnd, dc, 0, &sz, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA))
			Console::error("UpdateLayeredWindow failed\n");
		SelectObject(hMemDC, bmpold);
		DeleteObject(hBMP);
		ReleaseDC(0, dc);
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
bool BaseApp_Win::createGLWindow(const char* title, int width, int height, int bits, bool fullscreenflag)
{
	ASSERT(!(fullscreenflag && m_bLayeredWindow));	// sanity check: cant have layered fullscreen-mode windows
	
	if (m_bLayeredWindow)
		m_bOffscreenRendering = true;
	else
		m_bOffscreenRendering = false;

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		if (m_bLayeredWindow) {
			// make a window without caption and border if we are going to render everything ourselves
			dwExStyle=WS_EX_APPWINDOW;
			dwStyle=WS_POPUP;
		}
		else {
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle=WS_OVERLAPPEDWINDOW;
		}
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		killGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	// Initialize GLEW for extensions
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Console::error("GLEW initialization error: %s\n", glewGetErrorString(err));
		killGLWindow();
		return FALSE;
	}
	Console::print("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Check hardware support
	std::string err_str = "";
	bool bSimpleOk = true;
	if (!GL_VERSION_2_0)
		err_str = "WARNING: OpenGL 2.0 not supported. Try updating the graphics card drivers.";
	if (!GLEW_EXT_framebuffer_object)
		err_str = "frame buffer objects not supported";
	if (err_str.length() > 0) {
		Console::print("ERROR: " + err_str + "\n");
		if (bSimpleOk) {
			Console::print("WARNING: cannot use layered windows! Falling back to standard window style\n");
			MessageBox(hWnd, "WARNING: cannot use layered windows! Falling back to standard window style", "Error",
				MB_OK|MB_ICONEXCLAMATION);
		}
		else {
			Console::print("ERROR: OpenGL requirements not met. Cannot initialize BeGUI\n");
			MessageBox(hWnd, "ERROR: OpenGL requirements not met. Cannot initialize BeGUI", "Error",
				MB_OK|MB_ICONEXCLAMATION);
			killGLWindow();
		}
		m_bLayeredWindow = false;
		m_bOffscreenRendering = false;
	}

	// Create a layered window?
	if (m_bLayeredWindow)
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		HDC dc = GetDC(0);
		hMemDC = CreateCompatibleDC(0);
		if (!hMemDC)
			Console::error("failed to create dc\n");
		std::vector<unsigned char> data(width*height*4, 0);
		HBITMAP hBMP = CreateBitmap(width,height,1,32,&data[0]);
		if (!hBMP)
			Console::error("failed to create DIB bitmap\n");
		HBITMAP bmpold = (HBITMAP)SelectObject(hMemDC, hBMP);
		SIZE sz = {width,height};
		POINT ptSrc = {0,0};
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		if (!UpdateLayeredWindow(hWnd, dc, 0, &sz, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA))
			Console::error("UpdateLayeredWindow failed\n");
		SelectObject(hMemDC, bmpold);
		DeleteObject(hBMP);
	}
	else {
		// Reset the window style, in case we are falling back from layered window
		SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	}

	// Create an offscreen rendering surface
	if (m_bOffscreenRendering) {
		if (!setupOffscreenPass(width, height))
			return FALSE;
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	
	FrameWindow::inst()->create(width, height);
	resize(width, height);

	return TRUE;									// Success
}

void BaseApp_Win::killGLWindow()								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	return BaseApp_Win::inst()->wndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT BaseApp_Win::wndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_PAINT:
		{
			if (!m_bSyncRendering) {
				renderFrame();
				if (!m_bOffscreenRendering)
					SwapBuffers(hDC);
			}
			break;// let DefWindowProc remove the WM_PAINT message from the queue
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = true;
			int key = -1;
			switch (wParam) 
            { 
                case VK_LEFT: key = KEY_LEFT; break;
                case VK_RIGHT: key = KEY_RIGHT; break;
                case VK_UP: key = KEY_UP; break;
                case VK_DOWN: key = KEY_DOWN; break;
                case VK_HOME: key = KEY_HOME; break;
                case VK_END: key = KEY_END; break;
                case VK_INSERT: key = KEY_INSERT; break;
                case VK_DELETE: key = KEY_DELETE; break;
				case 33: key = KEY_PAGEUP; break;
				case 34: key = KEY_PAGEDOWN; break;
				case 16:	// BUG: cannot distringuish between left and right shift, the code is the same
					key = KEY_LSHIFT; break;
				case 17:	// BUG: cannot distringuish between left and right ctrl, the code is the same
					key = KEY_LCTRL; break;
			}
			if (key >= 0) {
				input::keyDown(key);
				FrameWindow::inst()->onKeyDown(key);
			}
			return 0;
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;
			int key = -1;
			switch (wParam) 
            { 
                case VK_LEFT: key = KEY_LEFT; break;
                case VK_RIGHT: key = KEY_RIGHT; break;
                case VK_UP: key = KEY_UP; break;
                case VK_DOWN: key = KEY_DOWN; break;
                case VK_HOME: key = KEY_HOME; break;
                case VK_END: key = KEY_END; break;
                case VK_INSERT: key = KEY_INSERT; break;
                case VK_DELETE: key = KEY_DELETE; break;
				case 33: key = KEY_PAGEUP; break;
				case 34: key = KEY_PAGEDOWN; break;
				case 16:	// BUG: cannot distringuish between left and right shift, the code is the same
					key = KEY_LSHIFT; break;
				case 17:	// BUG: cannot distringuish between left and right ctrl, the code is the same
					key = KEY_LCTRL; break;
			}
			if (key >= 0) {
				FrameWindow::inst()->onKeyUp(key);
				input::keyUp(key);
			}
			return 0;
		}
		case WM_CHAR:
			{
				//input::keyDown(wParam);
				FrameWindow::inst()->onKeyDown(wParam);
				break;
			}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			resize(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}

		case WM_LBUTTONDOWN:
			{
				int xPos = LOWORD(lParam); 
				int yPos = HIWORD(lParam);
				input::mouseButtonDown(xPos, yPos, MOUSE_BUTTON_LEFT);
				FrameWindow::inst()->onMouseDown(xPos, yPos, MOUSE_BUTTON_LEFT);
				lastMousePosX = xPos;
				lastMousePosY = yPos;
				return 0;
			}
		case WM_MOUSEMOVE:
			{
				int xPos = LOWORD(lParam); 
				int yPos = HIWORD(lParam);
				input::mousePos(xPos, yPos);
				FrameWindow::inst()->onMouseMove(xPos, yPos, lastMousePosX, lastMousePosY);
				lastMousePosX = xPos;
				lastMousePosY = yPos;
				return 0;
			}
		case WM_LBUTTONUP:
			{
				int xPos = LOWORD(lParam); 
				int yPos = HIWORD(lParam);
				input::mouseButtonUp(xPos, yPos, MOUSE_BUTTON_LEFT);
				FrameWindow::inst()->onMouseUp(xPos, yPos, MOUSE_BUTTON_LEFT);
				return 0;
			}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int BaseApp_Win::run(const std::string &title, size_t width, size_t height, FrameWindow::Style frame_style)
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	
	fullscreen=FALSE;							// Windowed Mode

	// Depending on the frame window style, set rendering options
	switch (frame_style) {
		case FrameWindow::MULTIPLE_SOLID:
		case FrameWindow::SINGLE:
			m_bLayeredWindow = false;
			break;
		case FrameWindow::SINGLE_OWNDRAW:
		case FrameWindow::MULTIPLE_SOLID_OWNDRAW:
		case FrameWindow::MULTIPLE_TRANSPARENT:
			m_bLayeredWindow = true;
			m_bSyncRendering = true;	// may not be needed later
			break;
	}
	FrameWindow::inst()->setStyle(frame_style);

	// Create Our OpenGL Window
	if (!createGLWindow(title.c_str(), (int)width, (int)height, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	// Init
	if (!coreInitialize())
	{
		killGLWindow();
		return false;
	}

	if (!initialize())
	{
		killGLWindow();
		return false;
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == 0)
			{
				// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
				if (active)								// Program Active?
				{
					updateFrame();
					if (m_bSyncRendering) {
						renderFrame();
						if (!m_bOffscreenRendering)
							SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
					}
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
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				updateFrame();
				if (m_bSyncRendering) {
					renderFrame();
					if (!m_bOffscreenRendering)
						SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				}
			}
		}
	}

	// Shutdown
	killGLWindow();

	return (msg.wParam);							// Exit The Program
}

bool BaseApp_Win::setupOffscreenPass(int width, int height)
{
	/*if (GLEW_EXT_framebuffer_object)
		;
	else {
		Console::error("shit\n");
		return false;
	}*/

	// create the render pass
	if (!m_frameRenderPass.setup(RenderPass::PIXEL_RGBA8, width, height, 0, false)) {
		Console::error("failed to create render pass for offscreen rendering\n");
		return false;
	}

	Console::print("created offscreen rendering surface (%d x %d)\n", width, height);

	return true;
}

HWND BaseApp_Win::getHWND()
{
	return hWnd;
}

HINSTANCE BaseApp_Win::getHINSTANCE()
{
	return hInstance;
}