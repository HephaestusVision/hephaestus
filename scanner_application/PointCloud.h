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
int convertPointCloudToVtkPoly(const PointCloud * cloud, vtkPolyData * output);

#endif /* POINTCLOUD_H */
