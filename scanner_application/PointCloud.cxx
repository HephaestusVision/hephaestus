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
using std::max;
using std::min;

#include "PointCloud.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkUnsignedCharArray.h"

#include "vtkContourFilter.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkUnsignedCharArray.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"

#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
#define setInputData(x,y) ((x)->SetInput(y))
#else
#define setInputData(x,y) ((x)->SetInputData(y))
#endif

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
    colorArray->SetName("rgb_colors");
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

// PointCloud {
//   unsigned long int NumPoints;
//   unsigned long int NumTriangles;
//   float * PointXYZ;
//   float * Normals;
//   unsigned char * PointRGB;
//   unsigned long int * Triangles;
// };

static inline double sq(double x) { return x * x; }
void blobify(const PointCloud * cloud, vtkPolyData * output)
{
  assert(cloud != NULL);
  unsigned long int numPoints = static_cast< unsigned long int >(cloud->NumPoints);
  if (numPoints < 1)
    return;

  const static unsigned long int RESOLUTION = 50;
  const static unsigned long int RESOLUTION3 = RESOLUTION * RESOLUTION * RESOLUTION;
  double density[RESOLUTION3];
  double rdens[RESOLUTION3];
  double gdens[RESOLUTION3];
  double bdens[RESOLUTION3];
  for(unsigned long int i = 0; i < RESOLUTION3; ++i)
    density[i] = rdens[i] = gdens[i] = bdens[i] = 0.0;

  float minimimXYZ[3], maximumXYZ[3];
  minimimXYZ[0] = maximumXYZ[0] = cloud->PointXYZ[0];
  minimimXYZ[1] = maximumXYZ[1] = cloud->PointXYZ[1];
  minimimXYZ[2] = maximumXYZ[2] = cloud->PointXYZ[2];

  for (unsigned long int ptIdx = 1 ; ptIdx < numPoints; ++ptIdx)
    {
    float x = cloud->PointXYZ[ (3 * ptIdx) + 0 ];
    if (x < minimimXYZ[0])
      minimimXYZ[0] = x;
    if (x > maximumXYZ[0])
      maximumXYZ[0] = x;

    float y = cloud->PointXYZ[ (3 * ptIdx) + 1 ];
    if (y < minimimXYZ[1])
      minimimXYZ[1] = y;
    if (y > maximumXYZ[1])
      maximumXYZ[1] = y;

    float z = cloud->PointXYZ[ (3 * ptIdx) + 2 ];
    if (z < minimimXYZ[2])
      minimimXYZ[2] = z;
    if (z > maximumXYZ[2])
      maximumXYZ[2] = z;
    // char r = cloud->PointRGB[ (3 * ptIdx) + 0 ];
    // char g = cloud->PointRGB[ (3 * ptIdx) + 1 ];
    // char b = cloud->PointRGB[ (3 * ptIdx) + 2 ];
    }

  float scale = maximumXYZ[0] - minimimXYZ[0];
  if ((maximumXYZ[1] - minimimXYZ[1]) > scale)
    scale = maximumXYZ[1] - minimimXYZ[1];
  if ((maximumXYZ[2] - minimimXYZ[2]) > scale)
    scale = maximumXYZ[2] - minimimXYZ[2];

  float orignXYZ[3];
  orignXYZ[0] = 0.5 * (maximumXYZ[0] + minimimXYZ[0] - scale);
  orignXYZ[1] = 0.5 * (maximumXYZ[1] + minimimXYZ[1] - scale);
  orignXYZ[2] = 0.5 * (maximumXYZ[2] + minimimXYZ[2] - scale);
  double step = scale / static_cast< double >(RESOLUTION - 1);
  double invstep = static_cast< double >(RESOLUTION - 1) / scale;

  double factor = -5.0 * static_cast< double >(RESOLUTION * RESOLUTION) /
    (scale * scale); // close enough/

  for (unsigned long int ptIdx = 1 ; ptIdx < numPoints; ++ptIdx)
    {
    float x = cloud->PointXYZ[ (3 * ptIdx) + 0 ];
    float y = cloud->PointXYZ[ (3 * ptIdx) + 1 ];
    float z = cloud->PointXYZ[ (3 * ptIdx) + 2 ];
    char r = cloud->PointRGB[ (3 * ptIdx) + 0 ];
    char g = cloud->PointRGB[ (3 * ptIdx) + 1 ];
    char b = cloud->PointRGB[ (3 * ptIdx) + 2 ];

    int closestX = (x - orignXYZ[0]) * invstep;
    int closestY = (y - orignXYZ[1]) * invstep;
    int closestZ = (z - orignXYZ[2]) * invstep;

    for (int xidx = max(0, closestX - 3);
         xidx < min(static_cast<int>(RESOLUTION - 1), closestX + 3);
         ++ xidx)
      {
      double xdsq = sq(x - (orignXYZ[0] + (step * xidx)));
      for (int yidx = max(0, closestY - 3);
           yidx < min(static_cast<int>(RESOLUTION - 1), closestY + 3);
           ++ yidx)
        {
        double ydsq = sq(y - (orignXYZ[1] + (step * yidx)));
        for (int zidx = max(0, closestZ - 3);
             zidx < min(static_cast<int>(RESOLUTION - 1), closestZ + 3);
             ++ zidx)
          {
          double zdsq = sq(z - (orignXYZ[2] + (step * yidx)));
          double dsq = xdsq * ydsq * zdsq;
          double dens = exp(factor * dsq);
          int array_idx = xidx + (RESOLUTION * (yidx + (RESOLUTION * zidx)));
          density[array_idx] += dens;
          rdens[array_idx] += dens * static_cast< double >(r);
          gdens[array_idx] += dens * static_cast< double >(g);
          bdens[array_idx] += dens * static_cast< double >(b);
          }
        }
      }
    }
  for(unsigned long int i = 0; i < RESOLUTION3; ++i)
    {
    if (density[i] > 0.0)
      {
      double inverse_density = 1.0 / density[i];
      rdens[i] *= inverse_density;
      gdens[i] *= inverse_density;
      bdens[i] *= inverse_density;
      }
    }

  vtkSmartPointer< vtkImageData > imageData = 
    vtkSmartPointer< vtkImageData >::New();
  imageData->SetOrigin(orignXYZ[0],orignXYZ[1],orignXYZ[2]);
  imageData->SetSpacing(step,step,step);
  imageData->SetExtent(0, RESOLUTION-1, 0, RESOLUTION-1, 0, RESOLUTION-1);
  
  vtkPointData * pd = imageData->GetPointData();
  assert(pd != NULL);
  
  vtkSmartPointer<vtkUnsignedCharArray> colorArray =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colorArray->SetName("rgb_colors");
  colorArray->SetNumberOfComponents(3);
  colorArray->SetNumberOfTuples(RESOLUTION3);
  for (vtkIdType ptId = 0 ; ptId < static_cast< vtkIdType >(RESOLUTION3); ++ptId)
    {
    unsigned char RGB[3];
    RGB[0] = rdens[ptId];
    RGB[1] = gdens[ptId];
    RGB[2] = bdens[ptId];
    colorArray->SetTupleValue(ptId, RGB);
    }
  pd->AddArray(colorArray);
  
  vtkSmartPointer< vtkDoubleArray > densityArray =
    vtkSmartPointer< vtkDoubleArray >::New();
  densityArray->SetName("point_density");
  densityArray->SetNumberOfComponents(1);
  densityArray->SetNumberOfTuples(RESOLUTION3);
  for (vtkIdType ptId = 0 ; ptId < static_cast< vtkIdType >(RESOLUTION3); ++ptId)
    densityArray->SetValue(ptId, density[ptId]);
  pd->SetScalars(densityArray);

  vtkSmartPointer< vtkContourFilter > isoSurfaceFilter = 
    vtkSmartPointer< vtkContourFilter >::New();
  setInputData(isoSurfaceFilter,imageData);
  isoSurfaceFilter->SetValue(0, 0.5);
  isoSurfaceFilter->ComputeNormalsOn();
  isoSurfaceFilter->Update();

  vtkPolyData * poly = isoSurfaceFilter->GetOutput();
  output->ShallowCopy(poly);
  output->GetPointData()->SetActiveScalars("rgb_colors");
}
void blobify2(const PointCloud * cloud, vtkPolyData * output)
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
    colorArray->SetName("rgb_colors");
    colorArray->SetNumberOfComponents(3);
    colorArray->SetNumberOfTuples(numPoints);
    for (vtkIdType ptId = 0 ; ptId < numPoints; ++ptId)
      colorArray->SetTupleValue(ptId, &(cloud->PointRGB[ 3 * ptId ]));
    pd->AddArray(colorArray);

    std::vector<bool> usedPoints(numPoints, false);
    vtkSmartPointer< vtkCellArray > verts =
      vtkSmartPointer< vtkCellArray >::New();

    if (cloud->NumTriangles > 0)
      {
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
      poly->SetPolys(triangles);
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
    } // end_if (cloud->NumPoints > 0)

  vtkSmartPointer< vtkGlyph3D > glyphFilter = 
    vtkSmartPointer< vtkGlyph3D >::New();
  vtkSmartPointer< vtkSphereSource > sphereSource = 
    vtkSmartPointer< vtkSphereSource >::New();
  sphereSource->SetRadius(0.0002);
  sphereSource->SetThetaResolution(6);
  sphereSource->SetPhiResolution(6);
  setInputData(glyphFilter,poly);
  glyphFilter->SetSourceConnection(sphereSource->GetOutputPort());
  glyphFilter->ScalingOff();	
  glyphFilter->Update();

  output->ShallowCopy(glyphFilter->GetOutput());
  output->GetPointData()->SetActiveScalars("rgb_colors");
  return;
}
