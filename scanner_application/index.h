/**
\mainpage Hephaestus Scanner Application
	
	The source code of the Scanner Application consists of 8 C++ classes
	-	DepthScannerMainWindow
	-	QVTKPolyViewWidget
	-	CameraWidget
	-	LoginDialog
	-	PickFolder
	-	MidasFolder
	-	PointCloud
	-	Cloudy
  - Parameters

	Plus a header file containing non-class routines:
	-	MidasUpload.h
	-	PointCloud.h

The program is structured as a QT application.  As such, the classes
DepthScannerMainWindow, QVTKPolyViewWidget, CameraWidget, LoginDialog,
PickFolder, and Parameters are subclasses of QT Widgets that do
specific things.

The MidasFolder and PointCloud classes are little more than structs
inteded to hold related information together.

Cloudy holds all of the computer-vision algorithms.

MidasUpload.h holds the functions for talking to a MIDAS server.

<hr>

-	http://hephaestusvision.github.com/
-	https://github.com/HephaestusVision/hephaestus/wiki
-	git://github.com/HephaestusVision/hephaestus.git
-	https://github.com/HephaestusVision/hephaestus/wiki/Installation-Guide
-	https://github.com/HephaestusVision/hephaestus/wiki/User-Manual

















 */
