cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

set(OSG_DIR "/home/gs/OpenSceneGraph")
set(OSG_LIBS 
${OSG_DIR}/build/lib/libOpenThreads.so           ${OSG_DIR}/build/lib/libosgParticle.so.202        ${OSG_DIR}/build/lib/libosgText.so.3.7.0
${OSG_DIR}/build/lib/libOpenThreads.so.21        ${OSG_DIR}/build/lib/libosgParticle.so.3.7.0      ${OSG_DIR}/build/lib/libosgUI.so
${OSG_DIR}/build/lib/libOpenThreads.so.3.3.1     ${OSG_DIR}/build/lib/libosgPresentation.so        ${OSG_DIR}/build/lib/libosgUI.so.202
${OSG_DIR}/build/lib/libosgAnimation.so          ${OSG_DIR}/build/lib/libosgPresentation.so.202    ${OSG_DIR}/build/lib/libosgUI.so.3.7.0
${OSG_DIR}/build/lib/libosgAnimation.so.202      ${OSG_DIR}/build/lib/libosgPresentation.so.3.7.0  ${OSG_DIR}/build/lib/libosgUtil.so
${OSG_DIR}/build/lib/libosgAnimation.so.3.7.0    ${OSG_DIR}/build/lib/libosgShadow.so              ${OSG_DIR}/build/lib/libosgUtil.so.202
${OSG_DIR}/build/lib/libosgDB.so                 ${OSG_DIR}/build/lib/libosgShadow.so.202          ${OSG_DIR}/build/lib/libosgUtil.so.3.7.0
${OSG_DIR}/build/lib/libosgDB.so.202             ${OSG_DIR}/build/lib/libosgShadow.so.3.7.0        ${OSG_DIR}/build/lib/libosgViewer.so
${OSG_DIR}/build/lib/libosgDB.so.3.7.0           ${OSG_DIR}/build/lib/libosgSim.so                 ${OSG_DIR}/build/lib/libosgViewer.so.202
${OSG_DIR}/build/lib/libosgFX.so                 ${OSG_DIR}/build/lib/libosgSim.so.202             ${OSG_DIR}/build/lib/libosgViewer.so.3.7.0
${OSG_DIR}/build/lib/libosgFX.so.202             ${OSG_DIR}/build/lib/libosgSim.so.3.7.0           ${OSG_DIR}/build/lib/libosgVolume.so
${OSG_DIR}/build/lib/libosgFX.so.3.7.0           ${OSG_DIR}/build/lib/libosg.so                    ${OSG_DIR}/build/lib/libosgVolume.so.202
${OSG_DIR}/build/lib/libosgGA.so                 ${OSG_DIR}/build/lib/libosg.so.202                ${OSG_DIR}/build/lib/libosgVolume.so.3.7.0
${OSG_DIR}/build/lib/libosgGA.so.202             ${OSG_DIR}/build/lib/libosg.so.3.7.0              ${OSG_DIR}/build/lib/libosgWidget.so
${OSG_DIR}/build/lib/libosgGA.so.3.7.0           ${OSG_DIR}/build/lib/libosgTerrain.so             ${OSG_DIR}/build/lib/libosgWidget.so.202
${OSG_DIR}/build/lib/libosgManipulator.so        ${OSG_DIR}/build/lib/libosgTerrain.so.202         ${OSG_DIR}/build/lib/libosgWidget.so.3.7.0
${OSG_DIR}/build/lib/libosgManipulator.so.202    ${OSG_DIR}/build/lib/libosgTerrain.so.3.7.0       
${OSG_DIR}/build/lib/libosgManipulator.so.3.7.0  ${OSG_DIR}/build/lib/libosgText.so
${OSG_DIR}/build/lib/libosgParticle.so           ${OSG_DIR}/build/lib/libosgText.so.202
)

project(test)

find_package(OpenCV REQUIRED)
find_package(OpenThreads)
find_package(osg)
find_package(osgDB)
find_package(osgUtil)
find_package(osgGA)
find_package(osgViewer)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

find_package(glfw3 REQUIRED)

add_library(modelrend
	modelrend.h
	modelrend.cpp
)

add_executable(test test.cpp)

target_link_libraries(test modelrend ${OpenCV_LIBS})
target_link_libraries(modelrend ${OSG_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw)
