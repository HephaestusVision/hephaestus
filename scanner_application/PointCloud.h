/**
  @file PointCloud.h

	PointCloud is a simple class to represent point clouds AND meshes.

  @authors
  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/
    
  @copyright
  Copyright 2012 University of North Carolina at Chapel Hill.

  @copyright
  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License at
    LICENSE.txt in this repository or
    http://www.apache.org/licenses/LICENSE-2.0

  @copyright
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.
*/

#ifndef POINTCLOUD_H
#define POINTCLOUD_H
#include "vtkPolyData.h"

/**
  PointCloud is a simple class to represent point clouds AND meshes.
*/
class PointCloud {
public:
	/**
		 number of points (vertices).
	 */
  unsigned long int NumPoints;
	/**
		 number of triangles.  Can be zero.
	 */
  unsigned long int NumTriangles;
	/**
		 pointer to array of xyz coordinates.
		 Format is assumed to be:
		 \code
		 float x = point_cloud->PointXYZ[ (3 * point_index) + 0 ];
		 float y = point_cloud->PointXYZ[ (3 * point_index) + 1 ];
		 float z = point_cloud->PointXYZ[ (3 * point_index) + 2 ];
		 \endcode
	 */
  float * PointXYZ; 
	/**
		 pointer to array of normal vectors.
		 Format is assumed to be:
		 \code
		 float normal_x = point_cloud->Normals[ (3 * point_index) + 0 ];
		 float normal_y = point_cloud->Normals[ (3 * point_index) + 1 ];
		 float normal_z = point_cloud->Normals[ (3 * point_index) + 2 ];
		 \endcode
	 */
  float * Normals;
	/**
		 pointer to array of color values.
		 Format is assumed to be:
		 \code
		 unsigned char R = point_cloud->PointRGB[ (3 * point_index) + 0 ];
		 unsigned char G = point_cloud->PointRGB[ (3 * point_index) + 1 ];
		 unsigned char B = point_cloud->PointRGB[ (3 * point_index) + 2 ];
		 \endcode
	 */
  unsigned char * PointRGB;
	/**
		 pointer to array of Triangles.
		 Format is assumed to be:
		 \code
		 unsigned long int v1 = point_cloud->PointRGB[ (3 * triangle_index) + 0 ];
		 unsigned long int v2 = point_cloud->PointRGB[ (3 * triangle_index) + 1 ];
		 unsigned long int v3 = point_cloud->PointRGB[ (3 * triangle_index) + 2 ];
		 float * vertex_coordinates_1 = &(point_cloud->PointXYZ[3 * v1]);
		 float * vertex_coordinates_2 = &(point_cloud->PointXYZ[3 * v2]);
		 float * vertex_coordinates_3 = &(point_cloud->PointXYZ[3 * v3]);
		 //...
		 \endcode
	 */
  unsigned long int * Triangles;
	/**
		 Will initialize PointXYZ, Normals, PointRGB, and Triangles to
		 newly allocated arrays.
	 */
  PointCloud(unsigned long int NumPoints, unsigned long int NumTriangles);
  ~PointCloud();
private:
  PointCloud(const PointCloud & c); // noncopyable resource
  PointCloud& operator=(const PointCloud & c); // noncopyable resource
};

/**
	 Function to convert to vtkPolyData. Overwrites output object.

	 Example useage:
	 \code
   void f(const PointCloud * cloud)
   {
     vtkSmartPointer< vtkPolyData > myVtkPolyData =
       vtkSmartPointer< vtkPolyData >::New();
     convertPointCloudToVtkPoly(cloud,myVtkPolyData);
     do_something_with(myVtkPolyData);
   }
	 \endcode
 */
int convertPointCloudToVtkPoly(
  const PointCloud * cloud, vtkPolyData * output);

/**
	 Function to convert to vtkPolyData. Overwrites output object.
	 
	 @warning Experimental - does not work well.
 */
void blobify(const PointCloud * cloud, vtkPolyData * output);

/**
	 Function to convert to vtkPolyData. Overwrites output object.
	 
	 @warning Experimental - does not work well.
 */
void blobify2(const PointCloud * cloud, vtkPolyData * output);

#endif /* POINTCLOUD_H */
