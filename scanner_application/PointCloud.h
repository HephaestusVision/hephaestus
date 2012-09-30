/*
  PointCloud.h

  Copyright 2012 University of North Carolina at Chapel Hill.

  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/

  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License at

    LICENSE.txt in this repository or
    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

  * * *

  PointCloud is a simple class to represent point clouds AND meshes.
*/
#ifndef POINTCLOUD_H
#define POINTCLOUD_H
#include "vtkPolyData.h"

class PointCloud {
public:
  unsigned long int NumPoints;
  unsigned long int NumTriangles;
  float * PointXYZ; 
  float * Normals;
  unsigned char * PointRGB;
  unsigned long int * Triangles;
  PointCloud(unsigned long int NumPoints, unsigned long int NumTriangles);
  ~PointCloud();
private:
  PointCloud(const PointCloud & c); // noncopyable resource
  PointCloud& operator=(const PointCloud & c); // noncopyable resource
};

/* // Overwrites output object.  Example useage:
   void f(const PointCloud * cloud)
   {
     vtkSmartPointer< vtkPolyData > myVtkPolyData =
       vtkSmartPointer< vtkPolyData >::New();
     convertPointCloudToVtkPoly(cloud,myVtkPolyData);
     do_something_with(myVtkPolyData);
   } */
int convertPointCloudToVtkPoly(
  const PointCloud * cloud, vtkPolyData * output);

#endif /* POINTCLOUD_H */
