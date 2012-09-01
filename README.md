The Hephaestus Project
======================

###Tweet###

> _Hephaestus facilitates creation of 3d scenes from a depth camera that can be shared in our online community and interacted with on tablets._

## Project Introduction ##

The Hephaestus team is Frank Ferro, Christopher Leslie, and Hal Canary.

Hephaestus will consist of three related applications:

1. A content acquisition application that makes use of the Kinect and the [Point Cloud Library](http://pointclouds.org/) to construct a digital representation of a scene (possibly in [VTK](http://www.vtk.org/) format).  That digital representation willbe in the form of surfaces arranged in scenes.  This application will handle converting point clouds to surfaces and merging point coulds to form a 360&#176; view of the objects.

2. A backend server that stores and manages the collection of scenes from users and organizations, possibly using [MIDAS](http://www.midasplatform.org/).  The server will allow groups of users to share scenes with each other.

3. A set of viewing apps for rendering scenes (possibly making use of [VES](http://www.vtk.org/Wiki/VES)).  The primary goal is the iOS and Android apps, but we also want a WebGL app and the ability to directly download files.

## User Stories ##

(These user stories need to be run by Patrick before they are finalized.)

An engineer wants to create a 3D model of an object and share it with collaborators across the world, some of whom only have smartphones.  He connects a Kinect to a laptop, runs our capture application, and points the Kinect at the object from several angles.  The application merges the point clouds and creates a triangulated mesh.  When the engineer is happy with the mesh, he uploads the object mesh into the cloud where it is available for everyone on the team to see.  His collaborators can get the mesh off the cloud and view the it via a WebGL/iOS/Android application, or simply download the mesh in a standard file format to edit as they please.

A salesman carries his tablet with him to show off 3D models of the devices he is selling.  Ahead of time, his team had created scenes of the devices in use, in place, using the capture application.

A technician fitting a medical devices such as a prosthetic leg uses our capture application to scan the prosthetic and the patient.  The technictian's team then could collaborate on fitting the prothetic correctly.

## Hephaestus Team Information ##

[Hephaestus Team Information](team.html)

## Repository for all Deliverables ##

[https://github.com/HephaestusVision/hephaestus/](https://github.com/HephaestusVision/hephaestus/)

## Related Links ##

- [Markdown Syntax](http://daringfireball.net/projects/markdown/syntax) - used for documentation.
- [Point Cloud Library](http://pointclouds.org/)
- [VTK](http://www.vtk.org/)
- [Kitware](http://www.kitware.com/)
- [MIDAS](http://www.midasplatform.org/).
- [VES](http://www.vtk.org/Wiki/VES)
- [XTK](https://github.com/xtk/X#readme) (WegGL Viz)
- [KiwiViewer](http://www.kiwiviewer.org/)
- [CMake](http://www.cmake.org/)
- [Git](http://git-scm.com/documentation)
- [research.microsoft.com/pubs/155378/ismar2011.pdf](http://research.microsoft.com/pubs/155378/ismar2011.pdf)
- [research.microsoft.com/pubs/155416/kinectfusion-uist-comp.pdf](http://research.microsoft.com/pubs/155416/kinectfusion-uist-comp.pdf)

