#   Hephaestus Scanner Application
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
#
# YOU MUST set the VTK_DIR, OpenCV_DIR, LIB_FREENECT_SRC, and
# LIB_FREENECT_BUILD variables.
#
# build openkinect/LibFreenect with
# cmake \
#     -DBUILD_CV:BOOL="1" \
#     -DOpenCV_DIR:PATH="...path-to.../build/OpenCV"

cmake_minimum_required(VERSION 2.6)

project( hephaestusscan )

set( VTK_DIR            "" CACHE PATH "location of VTK libraries" )
set( OpenCV_DIR         "" CACHE PATH "location of OpenCV libraries" )
set( LIB_FREENECT_SRC   "" CACHE PATH "location of libfreenext source" )
set( LIB_FREENECT_BUILD "" CACHE PATH "location of libfreenext libraries" )

set(CMAKE_BUILD_TYPE debug)

if( ${CMAKE_COMPILER_IS_GNUCXX} )
  set(PCL_HEADER_COMPILE_OPTIONS "-Wno-unknown-pragmas -Wno-deprecated-declarations")
  set(CMAKE_CXX_FLAGS "-g -Wall ${PCL_HEADER_COMPILE_OPTIONS}")
endif( ${CMAKE_COMPILER_IS_GNUCXX} )
if( ${MSVC} )
  ## insert MS Visual C++-specific options.
endif( ${MSVC} )

## PCL ##
find_package(PCL 1.3 REQUIRED COMPONENTS common io search)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

## VTK ##
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(QT_USE_OPENGL TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtScript REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

## OpenCV ##
find_package(OpenCV REQUIRED)

include_directories(
  "${LIB_FREENECT_SRC}/include/"
  "${LIB_FREENECT_SRC}/wrappers/opencv/"
  "${LIB_FREENECT_SRC}/wrappers/c_sync/" )

link_directories(
  "${LIB_FREENECT_BUILD}/lib"
  ${OpenCV_LIB_DIR_DBG}
  ${OpenCV_LIB_DIR_OPT}
)

set(SRC
  main.cxx
  CameraWidget.cxx
  Cloudy.cxx
  DepthScannerMainWindow.cxx
  QVTKPolyViewWidget.cxx
  PickFolder.cxx
  LoginDialog.cxx
  MidasUpload.cxx
  Parameters.cxx
)

set(MOC
  CameraWidget.h
  DepthScannerMainWindow.h
  PickFolder.h
  LoginDialog.h
  Parameters.h
  Cloudy.h
)

qt4_wrap_cpp(MOC_GENERATED ${MOC})

add_executable(
  hephaestusscan
  ${SRC}
  ${MOC_GENERATED}
)

target_link_libraries(
  hephaestusscan
  ${VTK_LIBRARIES}
  QVTK
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${OpenCV_LIBS}
  ${PCL_COMMON_LIBRARIES}
  ${PCL_IO_LIBRARIES}
  ${PCL_KDTREE_LIBRARY}
  ${PCL_SEARCH_LIBRARY}
  ${PCL_SURFACE_LIBRARY}
  ${PCL_FEATURES_LIBRARY}
  freenect_cv
)

