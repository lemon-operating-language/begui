#include "Application.h"
#include "../timeseries.h"

Application *Application::m_pInst = NULL;

void ChildWnd1::onCreate()
{
	// create a simple push button
	m_button1.create(150, 150, "Button", 1, 0);
	addComponent(&m_button1);

	// create a group of radio buttons
	m_radio1.create(80, 20, "Radio Button 1", 2);
	m_radio2.create(80, 40, "Radio Button 2", 3);
	m_group.create(20, 20, 200, 80, "Radio group");
	m_group.addComponent(&m_radio1);
	m_group.addComponent(&m_radio2);
	addComponent(&m_group);

	// create a slider (also an example of using a live
	// variable)
	m_slider.create(40, 130, 300, 4);
	m_slider.bindValue(&m_sliderPos);
	m_slider.setBounds(0, 10, 2, 20);
	addComponent(&m_slider);

	// create a textbox
	m_textbox.create(40, 150, 300, 150, true, true);
	addComponent(&m_textbox);
}

bool Application::initialize()
{
	// initialize the main window
	if (!BaseApp_Win::initialize())
		return false;

	// Get a pointer to the menu of the main window
	Menu &menu = FrameWindow::inst()->getMenu();

	// Create a new drop down menu
	Menu *pFileMenu = menu.addMenuItem("File", 1, Application::onMenuCB);

	// Add menu items to this menu.
	pFileMenu->addMenuItem("Open image", 101, Application::onMenuCB);
	pFileMenu->addMenuItem("Save image", 102, Application::onMenuCB);
	pFileMenu->addMenuItem("Save image as..", 103, Application::onMenuCB);
	pFileMenu->addMenuItem("------------", -1, 0, true);
	pFileMenu->addMenuItem("Page Setup", 104, Application::onMenuCB);
	pFileMenu->addMenuItem("Print Preview", 105, Application::onMenuCB);
	pFileMenu->addMenuItem("------------", -1, 0, true);
	pFileMenu->addMenuItem("Exit", 106, Application::onMenuCB);
	
	Menu *pMenu2 = menu.addMenuItem("Menu 02", 2, Application::onMenuCB);
	pMenu2->addMenuItem("Item 1", 201, Application::onMenuCB);
	pMenu2->addMenuItem("Item 2", 202, Application::onMenuCB);

	// create a child window
	m_wnd1.create(30, 50, 400, 400, "test");
	FrameWindow::inst()->addComponent(&m_wnd1);

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, // Instance 
				   HINSTANCE hPrevInstance, // Previous Instance 
				   LPSTR lpCmdLine, // Command Line Parameters 
				   int nCmdShow) // Window Show State 
{
	// Uncomment this line to have a console for text output
	Console::create();

	return Application::inst()->run("beGUI test", 800, 600);
}

void Application::onMenuCB(int id)
{
	Application::inst()->onMenu(id);
}

void Application::onMenu(int id)
{
	switch (id)
	{
	case 106:
		exit(0);
		break;
	}
}