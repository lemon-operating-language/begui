# Creating child windows and adding controls #

<h2>The BeGUI object model</h2>
BeGUI follows a simple model, which defines how components are displayed, updated and how they influence each other. The are two important base classes:

<ul>
<li><code>Component</code>: every component in BeGUI, that is every class that implements an entity that is visible and/or interacts with the user, inherits the class <code>Component</code>. <code>Component</code> provides all the core functionality shared between all visible entities in BeGUI.</li>
<li><code>Container</code>: every component that can contain other components must inherit the class <code>Container</code>. <code>Container</code> provides functionality to forward messages to its children, change between coordinate systems, keep track of mouse and keyboard focus etc. <code>Container</code> inherits the class <code>Component</code>. A container may contain only objects that have <code>Component</code> as a base class (therefore, it can also contain other containers, creating a hierarchy).</li>
</ul>

<p>Classes like <code>Window</code> and <code>GroupBox</code> inherit the class <code>Container</code>, since they act as containers of other components. Classes like <code>Button</code>, <code>Slider</code> etc. inherit the class <code>Component</code>. The base <code>Container</code> class can be directly used to transparently batch controls together, which is especially useful when the BeGUI application framework is not used.</p>

<p>Components are added to a container using the <code>Container::addComponent(...)</code> method. This method takes a pointer to the component to be added. After adding a component A to a container B, A will be displayed whenever B is displayed, and A will receive messages sent to B, if they are applicable to A.</p>

<p>Each container maintains a z-ordering of the components it contains, as well as pointers to the active component, and the ones with mouse and keyboard focus (usually all three are the same). The z-ordering of components changes when the user clicks on a component, bringing it to the top. A component can have a fixed z-ordering if this behavior is not desired. The order of components with fixed z-ordering in a container is determined by the order of the respective <code>addComponent</code> calls.</p>


<h2>Creating a child window</h2>
<p>Creating a new child window inside the frame window can be pretty simple. For a generic window, we could just write:</p>
```
	Window wnd;
	...
	wnd.create(left, top, width, height, "title");
	FrameWindow::inst()->addComponent(&wnd);
```

<p>(Of course, make sure that the window object is not going to be destroyed, as might not be so obvious in the example above, after the call to <code>addComponent()</code>)</p>

<p>However, in most cases we might want to have a derived class for a window in our application. In that case, just derive the base <code>begui::Window</code> class, and override methods (like <code>onCreate()</code>) to change the window behavior. Let's see an example:</p>

```
	// header file:
	class MyWindow : public Window
	{
	private:
		// components:
		Button m_btn;
		...
	public:
		...
		virtual void onCreate();
		...
	};
	...
	// cpp file:
	...
	void MyWindow::onCreate()
	{
		// create the button here
		m_btn.create(10, 10, "OK");

		// add this button as a child of the window. This is
		// necessary for the button to be displayed and function.
		addComponent(&m_btn);
	}
        ...
```

<p>We would use the derived class just like the base <code>Window</code> class in the first example. For a more complete example, see the <code>guitest</code> project in the beGUI source package.</p>