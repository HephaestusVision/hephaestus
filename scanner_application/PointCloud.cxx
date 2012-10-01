/*
  PointCloud.cxx

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
#include <cassert>
#include <vector>
#include <algorithm>

#include "PointCloud.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkUnsignedCharArray.h"

PointCloud::PointCloud(unsigned long int NumPoints,
                       unsigned long int NumTriangles) :
  NumPoints(NumPoints),
  NumTriangles(NumTriangles),
  PointXYZ(new float[3 * NumPoints]),
  Normals(new float[3 * NumPoints]),
  PointRGB(new unsigned char[3 * NumPoints]),
  Triangles(new unsigned long int[3 * NumTriangles]) { }
PointCloud::~PointCloud()
{
  delete [] this->PointXYZ;
  delete [] this->Normals;
  delete [] this->PointRGB;
  delete [] this->Triangles;
}

int convertPointCloudToVtkPoly(const PointCloud * cloud, vtkPolyData * output)
{
  vtkSmartPointer< vtkPolyData > poly =
    vtkSmartPointer< vtkPolyData >::New(); // create new data object
  vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
  poly->SetPoints(pts);
  if (cloud->NumPoints > 0)
    {
    pts->SetNumberOfPoints(cloud->NumPoints);
    vtkIdType numPoints = static_cast< vtkIdType >(cloud->NumPoints);
    for (vtkIdType ptId = 0; ptId < numPoints; ++ptId)
      pts->SetPoint(ptId, &(cloud->PointXYZ[ 3 * ptId ]));

    vtkPointData * pd = poly->GetPointData();
    assert(pd != NULL);

    vtkSmartPointer<vtkUnsignedCharArray> colorArray =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorArray->SetName("RGB");
    colorArray->SetNumberOfComponents(3);
    colorArray->SetNumberOfTuples(numPoints);
    for (vtkIdType ptId = 0 ; ptId < numPoints; ++ptId)
      colorArray->SetTupleValue(ptId, &(cloud->PointRGB[ 3 * ptId ]));
    pd->SetScalars(colorArray);

    vtkSmartPointer< vtkFloatArray > normalsArray =
      vtkSmartPointer< vtkFloatArray >::New();
    normalsArray->SetName("Normals");
    normalsArray->SetNumberOfComponents(3);
    normalsArray->SetNumberOfTuples(numPoints);
    for (vtkIdType ptId = 0 ; ptId < numPoints; ++ptId)
      normalsArray->SetTupleValue(ptId, &(cloud->Normals[ 3 * ptId ]));
    pd->AddArray(normalsArray);
    std::vector<bool> usedPoints(numPoints, false);
    vtkSmartPointer< vtkCellArray > verts =
      vtkSmartPointer< vtkCellArray >::New();
    vtkSmartPointer< vtkCellArray > triangles =
      vtkSmartPointer< vtkCellArray >::New();

    triangles->Allocate(cloud->NumTriangles * 4);
    for (unsigned long int ptId = 0 ; ptId < cloud->NumTriangles; ++ptId)
      {
      triangles->InsertNextCell(3);
      unsigned long int * vertexIDs = &(cloud->Triangles[3 * ptId]);
      for(int i = 0; i < 3 ; ++i)
        {
        assert (vertexIDs[i] < (cloud->NumPoints));
        triangles->InsertCellPoint(static_cast< vtkIdType >(vertexIDs[i]));
        usedPoints[vertexIDs[i]] = true;
        }
      }
    verts->Allocate(
      4 * static_cast<int>(
        std::count(usedPoints.begin(), usedPoints.end(), false)));
    for (unsigned long int ptId = 0 ; ptId < cloud->NumPoints; ++ptId)
      {
      if (!usedPoints[ptId])
        {
        verts->InsertNextCell(1);
        verts->InsertCellPoint(static_cast< vtkIdType >(ptId));
        }
      }
    poly->SetVerts(verts);
    poly->SetPolys(triangles);
    } // end_if (cloud->NumPoints > 0)

  output->ShallowCopy(poly);
  return 0;
}

