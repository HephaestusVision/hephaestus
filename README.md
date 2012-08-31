The Hephaestus Project
======================

## Project Introduction ##

> _Create, View, and Share 3D Scenes Using a Depth Camera and a WebGL/iOS/Android application._

The Hephaestus team is Frank Ferro, Christopher Leslie, and Hal Canary.

Hephaestus will consist of three related applications:

1. A content acquisition application that makes use of the Kinect and the [Point Cloud Library](http://pointclouds.org/) to construct a digital representation of a scene (possibly in [VTK](http://www.vtk.org/) format).  That digital representation willbe in the form of surfaces arranged in scenes.  This application will handle converting point clouds to surfaces and merging point coulds to form a 360&#176; view of the objects.

2. A backend server that stores and manages the collection of scenes from users and organizations, possibly using [MIDAS](http://www.midasplatform.org/).  The server will allow groups of users to share scenes with each other.

3. A set of viewing apps for rendering scenes (possibly making use of [VES](http://www.vtk.org/Wiki/VES)).  The primary goal is the iOS and Android apps, but we also want a WebGL app and the ability to directly download files.

## User Stories ##

An engineer wants to create a 3D model of an object and share it with collaborators across the world, some of whom only have smartphones.  He connects a Kinect to a laptop, runs out capture application, and point the Kinect at the object from several angles.  The application merges the point clouds and creates a triangulated mesh.  When the engineer is happy with the mesh, he uploads the object mesh into the cloud where it is available for everyone on the team to see.  His collaborators can get the mesh off the cloud and view it via a WebGL/iOS/Android application, or simply download the mesh in a standard file format to edit as they please.

A salesman carries his tablet with him to show off 3D models of the devices he is selling.  Ahead of time, his team had created scenes of the devices in use, in place, using the capture application.

...Something about medical devices...


* * *

## Team Roles &amp; Contact Information ##

Team:
- [Frank Ferro](http://www.unc.edu/~fferro/) (fferro at live.unc.edu, 631-848-8043) - project manager, editor
- [Christopher Leslie](http://cs.unc.edu/~cdleslie/) (cdleslie at live.unc.edu, 919-649-3129) - client manager, computer vision developer
- [Hal Canary](http://cs.unc.edu/~hal/) (hal at cs.unc.edu, 239-313-9375) - chief developer

Client:
- [Patrick Reynolds](http://www.cs.unc.edu/~cpatrick/) (cpreynolds at gmail.com)

## Schedule of Regular Meetings ##

- Wednesday @ 10 a.m. - meet with client
- Friday @ 10 a.m. - meet with professor
- other group meetings, as needed

## Team Rules ##

### Team Behavior ###

We will all make it to each team meeting, on time, unless prior notification is given.

We will deliver things on time.  If a deadline is approaching and someone isn't going to make it, he must ask the rest of the team for help well before the deadline.

We will all respond to email in a timely manner (within 24 hours at worst).

If any of us break the rules, it will be noted in the project's peer-evaluation.

### Coding Practices ###

We will adhere to Kitware's coding style and guidelines, since they are our client.  Hal is familiar with the [VTK Coding Standards](http://www.vtk.org/Wiki/VTK_Coding_Standards).

## Journal of Meetings and Decisions ##

...

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

## Repository for all Deliverables ##

[here](https://github.com/HephaestusVision/hephaestus/)
