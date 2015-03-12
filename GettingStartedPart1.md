# Creating a new application using BeGUI #

<h2>BeGUI Application Model</h2>
<p>BeGUI can be used in two main ways:<br>
<ul>
<li>Using the BeGUI application framework: You can take advantage of the provided application framework classes (<code>BaseApp_Win</code> for Windows, more framework classes to come in the near future) to quickly setup an application, without caring about event routing etc. We will show an example of this approach in the rest of this page.</li>
<li>Using your own framework: BeGUI has been designed so that components can be taken in relative isolation and thrown into existing applications. In this case, you have to take care of feeding the BeGUI components with events etc. An example of how to use BeGUI in another application framework is given <a href='http://code.google.com/p/begui/wiki/tutorial_beGUI_in_GLUT_apps'>here (BeGUI on GLUT)</a>. An understanding of the role of <code>Container</code> and <code>Component</code> classes is important to use BeGUI components in isolation (see <a href='http://code.google.com/p/begui/wiki/GettingStartedPart3'>Part 3</a> of this series).</li>
</ul>
</p>

<h2>Creating an empty application window</h2>
<p>Open Visual Studio and start a new, empty Win32 project:</p>
<ul>
<blockquote><li>File -> New -> Project</li>
<li>Select "Win32 Project"</li>
<li>Give a name to the project and press OK</li>
<li>In Application Settings, check "Empty Project" and press Finish</li>
</ul></blockquote>

<p>Now we have our new, empty project. We start by creating a <code>WinMain</code> function as the entry point for our program. Create a new file, named <code>main.cpp</code>, and write the following in it:</p>
```
	int WINAPI WinMain(HINSTANCE	hInstance,		// Instance
			HINSTANCE	hPrevInstance,		// Previous Instance
			LPSTR		lpCmdLine,		// Command Line Parameters
			int		nCmdShow)		// Window Show State
	{
		return 0;
	}
```

<p>We just made the simplest possible Windows program. Now, we need to create our main window, initialize OpenGL and beGUI and get rendering. For this tutorial, we will use the base Win32 application class provided by beGUI. Notice that this is not necessary, you can pretty much grab beGUI controls and throw them into any OpenGL application.</p>

<p>The base application class provided by beGUI for Win32 platforms (the only one right now :P) is <code>BaseApp_Win</code>. We will derive a new class from this one. Let's name it with the imaginative name <code>Application</code>.Create a new file and name it <code>Application.h</code>.</p>

<p>First, we will include the beGUI header files:</p>
```
	#pragma once
	#include "bgui.h"
	#include <windows.h>
```

<p>Make sure that you add the path to the beGUI headers to the directories in Visual Studio. Also, make sure that you link to <code>begui.lib</code>.</p>

<p>Now, let's create our application class:</p>
```
	class Application : public BaseApp_Win
	{
	};
```

<p>Also, modify <code>WinMain</code> in the following way:</p>
```
	#include "Application.h"

	int WINAPI WinMain(HINSTANCE	hInstance,		// Instance
			HINSTANCE	hPrevInstance,		// Previous Instance
			LPSTR		lpCmdLine,		// Command Line Parameters
			int		nCmdShow)		// Window Show State
	{
                // Set the path of the resource directory. This path is relative
                // to the application working directory, and should be the path to
                // begui/resources/, if you are using the standard beGUI skin.
	        ResourceManager::inst()->setResourceDir("../resources/");

                // Set some options for the window we are about to create
        	FrameWindow::Options opt;
        	opt.bOwnDraw = true; // set this to false to get a conventional window
                                     // instead of an alpha-blended one with custom
                                     // borders (compatible with most systems)
	        opt.bFullScreen = false;  // full-screen support is not complete in v0.1.2
        	opt.nColorBits = 24;
        	opt.nDepthBits = 8;
        	opt.nStencilBits = 0;

                // Initialize the application (creates the main window and performs
                // all major initializations)
	        if (!Application::inst()->initialize("beGUI test", 800, 600, FrameWindow::MULTIPLE, &opt))
		       return false;

                // Enable sync rendering if you want your app to render new frames on
                // idle time. Disable it if you want your app to perform rendering only
                // when it receives WM_PAINT messages from Windows.
                Application::inst()->setSyncRendering(true);

                // Start the application's message loop
		return Application::inst()->run();
	}
```

<h3>Building</h3>
<p>Ok. One more step is missing. BeGUI is using the FreeType and GLEW libraries. So, go and download FreeType (2.3.5 used with beGUI, newer versions will probably work fine as well) from <a href='http://www.freetype.org/'><a href='http://www.freetype.org'>http://www.freetype.org</a></a>. Make sure that you set up the include and library directories in Visual Studio correctly (to include the corresponding directories of FreeType). You also need to get GLEW from <a href='http://glew.sourceforge.net/'><a href='http://glew.sourceforge.net'>http://glew.sourceforge.net</a></a>.</p>

<p>Link your application with:<br>
<ul>
<li><code>freetype???MT.lib</code> where '???' is the appropriate version number (eg 239 for the latest Freetype release)</li>
<li><code>glew32.lib</code></li>
<li><code>bgui.lib</code></li>
</ul>
To do so, in Visual Studio, add the above in the field <code>Project Properties-&gt;Linker-&gt;Input-&gt;Additional Dependencies</code>. Good. Let's compile and run this<br>
simple program, shall we? Before you run it, make sure that the OpenGL, GLEW and FreeType<br>
dlls are reachable (e.g. in the application folder). If everything went well, you should<br>
see an empty window after the program starts.</p>

<p>Yay! You just made your first application based on beGUI. Doesn't look all that<br>
exciting though, does it? We should add some things to it. Let's say, a menu, a couple of<br>
windows and a bunch of controls.</p>

<a href='http://code.google.com/p/begui/wiki/GettingStartedPart2'>continue to Part 2</a>

<h2>Troubleshooting</h2>
<p>Currently, there are certain issues with OpenGL rendering on layered windows (used to get the semi-transparent window borders),  especially on ATI graphics cards. If you experience very slow performance, artifacts, or a message box saying that your GPU doesnt support alpha-blended layered windows, set</p>
```
        	opt.bOwnDraw = false;
```
to use the standard windows provided by the OS. I am working on fixing these problems (until recently I didnt have an ATI card to test on).