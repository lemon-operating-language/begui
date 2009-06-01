#include "Application.h"
#include "../timeseries.h"

Application *Application::m_pInst = NULL;

void ChildWnd1::onCreate()
{
	// create the tabs
	m_tabs.create(8, 8, 400, 360);
	m_tabs.addTab("tab 1");
	m_tabs.addTab("tab 2");
	addComponent(&m_tabs);

	// create a simple push button
	m_button1.create(260, 40, "Button", 1);
	//m_button1.create(260, 40, 80, 60, "Button", 1);
	m_button1.setIcon(ResourceManager::inst()->loadImage("gears.png"), Button::NEAR_LEFT, 32,32);
	//m_button1.setFace(Button::UP, ResourceManager::inst()->loadImage("gears.png"));
	m_tabs.addComponent(&m_button1, 0);

	m_checkbox.create(260, 70, "check here", 102);
	m_tabs.addComponent(&m_checkbox, 0);

	// create a group of radio buttons
	m_radio1.create(20, 20, "Radio Button 1", 2);
	m_radio2.create(20, 40, "Radio Button 2", 3);
	m_group.create(20, 20, 200, 80, "Radio group");
	m_group.addComponent(&m_radio1);
	m_group.addComponent(&m_radio2);
	m_tabs.addComponent(&m_group, 0);

	// create a slider (also an example of using a live
	// variable)
	m_slider.create(20, 120, 300, 4);
	m_slider.bindValue(&m_sliderPos);
	m_slider.setBounds(0, 10, 2, 20);
	m_tabs.addComponent(&m_slider, 0);

	// create a textbox
	m_textbox.create(230, 160, 130, 180, true, true);
	m_tabs.addComponent(&m_textbox, 0);
	
	m_listbox.create(20, 140, 200, 200, ListBox::MULTI_SELECT_SINGLECLICK);
	m_listbox.addItem("item 1");
	m_listbox.addItem("disabled item 2");
	m_listbox.addItem("item 3");
	m_listbox.addItem("item 4");
	m_listbox.addItem("item 5");
	for (size_t i=0; i<20; ++i)
		m_listbox.addItem("item");
	m_listbox.addItem("last item");
	m_listbox.disableItem(1);
	m_tabs.addComponent(&m_listbox, 0);

	m_combobox.create(230, 140, 80, 200);
	m_combobox.addItem("item 1");
	m_combobox.addItem("item 2");
	m_combobox.addItem("item 3");
	m_combobox.addItem("item 4");
	m_combobox.disableItem(2);
	m_tabs.addComponent(&m_combobox, 0);

	// change the size of the window, so that the client area is large
	// enough to hold the contents
	setClientAreaSize(416, 410);
}

bool Application::onCreate()
{
	// Get a pointer to the menu of the main window
	Menu &menu = *FrameWindow::inst()->createMainMenu();

	// Create a new drop down menu
	Menu *pFileMenu = menu.addMenuItem("File", 1, makeFunctor(*this, &Application::onMenu));

	// Add menu items to this menu.
	pFileMenu->addMenuItem("Open image", 101, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Save image", 102, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Save image as..", 103, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addSeparator();
	pFileMenu->addMenuItem("Page Setup", 104, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addMenuItem("Print Preview", 105, makeFunctor(*this, &Application::onMenu));
	pFileMenu->addSeparator();
	pFileMenu->addMenuItem("Exit", 106, makeFunctor(*this, &Application::onMenu));
	
	Menu *pMenu2 = menu.addMenuItem("Menu 02", 2, makeFunctor(*this, &Application::onMenu));
	pMenu2->addMenuItem("Item 1", 201, makeFunctor(*this, &Application::onMenu));
	pMenu2->addMenuItem("Item 2", 202, makeFunctor(*this, &Application::onMenu));

	// create a child window
	m_wnd1.create(30, 50, 420, 400, "child window");
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

	// Set the path of the resource directory
	ResourceManager::inst()->setResourceDir("../resources/");

	FrameWindow::Options opt;
	opt.bOwnDraw = true;
	opt.bFullScreen = false;
	opt.nColorBits = 24;
	opt.nDepthBits = 8;
	opt.nStencilBits = 0;
	if (!Application::inst()->initialize("beGUI test", 800, 600, FrameWindow::MULTIPLE, &opt))
		return false;

	return Application::inst()->run();
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