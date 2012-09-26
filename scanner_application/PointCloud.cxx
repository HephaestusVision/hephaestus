#include <cassert>

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
  if (cloud->NumPoints > 0)
    {
    vtkPoints * pts = poly->GetPoints();
    pts->SetNumberOfPoints(cloud->NumPoints);
    vtkIdType numPoints = static_cast< vtkIdType >(cloud->NumPoints);
    for (vtkIdType ptId = 0; ptId < numPoints; ++ptId)
      pts->SetPoint(ptId, &(cloud->PointXYZ[ 3 * ptId ]));
    
    vtkPointData * pd = poly->GetPointData();

    vtkSmartPointer<vtkUnsignedCharArray> colorArray =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorArray->SetName("RGB");
    colorArray->SetNumberOfComponents(3);
    for (vtkIdType ptId = 0 ; ptId < numPoints; ++ptId)
      colorArray->SetTupleValue(ptId, &(cloud->PointRGB[ 3 * ptId ]));
    pd->AddArray(colorArray);

    vtkSmartPointer< vtkFloatArray > normalsArray =
      vtkSmartPointer< vtkFloatArray >::New();
    normalsArray->SetName("Normals");
    normalsArray->SetNumberOfComponents(3);
    for (vtkIdType ptId = 0 ; ptId < numPoints; ++ptId)
      normalsArray->SetTupleValue(ptId, &(cloud->Normals[ 3 * ptId ]));
    pd->AddArray(normalsArray);

    std::vector<bool> usedPoints(numPoints, false);

    vtkCellArray * verts = poly->GetVerts();
    vtkCellArray * triangles = poly->GetPolys();

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
    for (unsigned long int ptId = 0 ; ptId < cloud->NumPoints; ++ptId)
      {
      if (!usedPoints[ptId])
        {
        verts->InsertNextCell(1);
        verts->InsertCellPoint(static_cast< vtkIdType >(ptId));
        }
      }
    } // end_if (cloud->NumPoints > 0)
  
  output->ShallowCopy(poly);
  return 0;
}

