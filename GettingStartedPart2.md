# Creating a new application using BeGUI #

<h2>Part 2: Creating a menu</h2>
<p>The menu will be created during the initialization of our application. To do that, we should override the method <code>onCreate()</code> of <code>BaseApp_Win</code>, to include any additional initialization code (<code>onCreate</code> is called at the end of <code>Application::initialize(...)</code>):</p>
```
	class Application : public BaseApp_Win {
		...
	public:
		virtual bool onCreate();
		...
	};
```

<p>In the <code>Application.cpp</code> file, we create the menu by making <code>onCreate()</code> look as following:</p>
```
	bool Application::onCreate()
	{
		// Get a reference to the menu of the main window
		Menu &menu = *FrameWindow::inst()->createMainMenu();

		// Create a new drop down menu
		Menu *pFileMenu = menu.addMenuItem("File", 1, makeFunctor(*this, &Application::onMenu));

		// Add menu items to this menu.
		pFileMenu->addMenuItem("Open", 101, makeFunctor(*this, &Application::onMenu));
		pFileMenu->addMenuItem("Save", 102, makeFunctor(*this, &Application::onMenu));
		pFileMenu->addSeparator();
		pFileMenu->addMenuItem("Exit", 103, makeFunctor(*this, &Application::onMenu));

		// Create a second drop down menu
		Menu *pSecondMenu = menu.addMenuItem("Another menu", 2, makeFunctor(*this, &Application::onMenu));
		pSecondMenu->addMenuItem("Choice one", 201, makeFunctor(*this, &Application::onMenu));
		pSecondMenu->addMenuItem("Choice two", 201, makeFunctor(*this, &Application::onMenu));

		return true;
	}
```

<p>Here, <code>Application::onMenu(int)</code> is the method that will handle all menu-related events. It takes one argument, the id of the menu or menuitem clicked. Notice that this menu item id is the second argument in <code>addMenuItem(...)</code>. Also, notice that to create a submenu, we just add menu items to a menu item. Method <code>addSeparator()</code> creates a small horizontal line in the corresponding position in the menu, separating the menu items.</p>

<p>To use <code>Application::onMenu</code> as a callback, we provide a functor object, created using <code>makeFunctor</code>. The first argument of <code>makeFunctor</code> is the instance of the class to which the callback method belongs, and the second argument is the name of the callback method. The method <code>addMenuItem</code> accepts a functor of type <code>Functor1&lt;int&gt;</code>, which means a functor to a method that accepts one argument of type <code>int</code>, and returns <code>void</code>. Functor classes are defined in <code>callback.h</code> for the curious.</p>

<p>The callback for the menu is defined like this:</p>
```
	class Application : public BaseApp_Win {
		...
	private:
		void onMenu(int id);
		...
	};
```
<p>and in Application.cpp:</p>
```
	void Application::onMenu(int id)
	{
		switch (id)
		{
			// handle menu events here
                case 101:
                        // Open was clicked. Do something.
                        break;
			...
		}
	}
```

<p>Compile and run. Voila, a menu!</p>