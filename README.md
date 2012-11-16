The Hephaestus Project
======================

###Tweet###

> _Hephaestus facilitates creation of 3d scenes from a depth camera that can be shared in our online community and interacted with on tablets._

## Project Introduction ##

The Hephaestus team is Frank Ferro, Christopher Leslie, and Hal Canary.

Hephaestus will consist of three related applications:

1. A content acquisition application that makes use of the Kinect and the [Point Cloud Library](http://pointclouds.org/) to construct a digital representation of a scene (possibly in [VTK](http://www.vtk.org/) format).  That digital representation will be in the form of surfaces arranged in scenes.  This application will handle converting point clouds to surfaces and merging point clouds to form a 360&#176; view of the objects.

2. A backend server that stores and manages the collection of scenes from users and organizations, possibly using [MIDAS](http://www.midasplatform.org/).  The server will allow groups of users to share scenes with each other.

3. A set of viewing apps for rendering scenes (possibly making use of [VES](http://www.vtk.org/Wiki/VES)).  The primary goal is the iOS and Android apps, but we also want a WebGL app and the ability to directly download files.

* * *

See the [Hephaestus Wiki](https://github.com/HephaestusVision/hephaestus/wiki) for more information.
