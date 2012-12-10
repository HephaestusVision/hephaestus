/**
\mainpage Hephaestus Scanner Application
	
	The source code of the Scanner Application consists of 8 C++ classes
	-	DepthScannerMainWindow
	-	QVTKPolyViewWidget
	-	CameraWidget
	-	LoginDialog
	-	PickFolder
	-	MidasFolder
	-	Cloudy
  - Parameters

	Plus a header file containing non-class routines:
	-	MidasUpload.h
	-	Cloudy.h

The program is structured as a QT application.  As such, the classes
DepthScannerMainWindow, QVTKPolyViewWidget, CameraWidget, LoginDialog,
PickFolder, and Parameters are subclasses of QT Widgets that do
specific things.

The MidasFolder and classe is little more than structs
inteded to hold related information together.

Cloudy holds all of the computer-vision algorithms, as well as
routines to convert from Point Cloud Library data structures to VTK
data structures.

MidasUpload.h holds the functions for talking to a MIDAS server.

The overall architecture of the Hephaestus system:<br>
<img src="http://hephaestusvision.github.com/images/hephestus_arch_1.png">

The architecture of the Hephaestus Scanner Application:<br>
<img src="http://hephaestusvision.github.com/images/hephestus_arch_2.png">

<hr>

-	http://hephaestusvision.github.com/
-	https://github.com/HephaestusVision/hephaestus/wiki
-	git://github.com/HephaestusVision/hephaestus.git
-	https://github.com/HephaestusVision/hephaestus/wiki/Installation-Guide
-	https://github.com/HephaestusVision/hephaestus/wiki/User-Manual

















 */
