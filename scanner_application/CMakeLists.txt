#   An example of a VTK-QT application.
#
#   Copyright 2012 University of North Carolina at Chapel Hill.
#
#   Licensed under the Apache License, Version 2.0 (the "License"); you
#   may not use this file except in compliance with the License.  You
#   may obtain a copy of the License at
#
#     LICENSE.txt in this repository or
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
#   implied.  See the License for the specific language governing
#   permissions and limitations under the License.

cmake_minimum_required(VERSION 2.6)

project( vtk_test )

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "-g -Wall")

set(VTK_DIR "/home/hal/build/VTK-5-10")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(QT_USE_OPENGL TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(SRC
  main.cxx
  CameraWidget.cxx
  Cloudy.cxx
  DepthScannerMainWindow.cxx
  PointCloud.cxx
  QVTKPolyViewWidget.cxx
)

set(MOC
  CameraWidget.h
  DepthScannerMainWindow.h
)
qt4_wrap_cpp(MOC_GENERATED ${MOC})

SOURCE_GROUP("Generated" FILES ${MOCSrcs} ${RCS_SOURCES} )
 
add_executable(vtk_test
    ${SRC}
    ${MOC_GENERATED} 
)
#add_library( vtk SHARED IMPORTED ${VTK_LIBRARIES} )

LINK_DIRECTORIES(
  ${QT_LIBRARIES}
  ${QT_LIBRARY_DIR}
  ${QT_QTOPENGL_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_MAIN_LIBRARY}
  )


target_link_libraries( vtk_test ${VTK_LIBRARIES} QVTK ${QT_QTOPENGL_LIBRARY})


