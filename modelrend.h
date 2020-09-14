#pragma once

#include <osg/Camera>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/Geometry>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation/BasicAnimationManager>         

class VirtualCamera {
public:
	void createVirtualCamera(osg::ref_ptr<osg::Camera> cam, int width, int height);
	void updatePosition(double r, double p, double h, double x, double y, double z);

	double angle;
	osg::Matrix rotation;
	osg::Matrix translation;
	osg::ref_ptr<osg::Camera> camera;
	osg::ref_ptr<osg::Image> image;
};

class BackgroundCamera {
public:
	BackgroundCamera();
	void update(uint8_t* data, int cols, int rows);
	osg::Geode* createCameraPlane(int textureWidth, int textureHeight);
	osg::Camera* createCamera(int textureWidth, int textureHeight);

	osg::ref_ptr<osg::Image> img;
};

class Modelrender {
public:
	osgViewer::Viewer viewer;
	BackgroundCamera bgCamera;
	VirtualCamera vCamera;
	double angleRoll;
	int width, height;

	Modelrender(int cols, int rows);
	uint8_t* rend(uint8_t* inputimag);
};
