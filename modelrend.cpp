#include "modelrend.h"

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation/BasicAnimationManager>

#include <osgViewer/GraphicsWindow>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Image>
#include <osg/BufferObject>
#include <osgUtil/Optimizer>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>

void VirtualCamera::createVirtualCamera(osg::ref_ptr<osg::Camera> cam, int width, int height)
{
	camera = cam;
	// Initial Values
	camera->setProjectionMatrixAsPerspective(320, 1., 1., 100.);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER);
	image = new osg::Image;
	image->allocateImage(width, height, 1, GL_BGR, GL_UNSIGNED_BYTE);
	camera->attach(osg::Camera::COLOR_BUFFER, image.get());
}

void VirtualCamera::updatePosition(double r, double p, double h, double x, double y, double z)
{
	osg::Matrixd myCameraMatrix;

	// Update Rotation
	rotation.makeRotate(
		osg::DegreesToRadians(r), osg::Vec3(0, 1, 0), // roll
		osg::DegreesToRadians(p), osg::Vec3(1, 0, 0), // pitch
		osg::DegreesToRadians(h), osg::Vec3(0, 0, 1)); // heading

													   // Update Translation
	translation.makeTranslate(x, y, z);
	myCameraMatrix = rotation * translation;
	osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
	camera->setViewMatrix(i*osg::Matrix::rotate(-(osg::PI_2), 1, -0, 0));
}



BackgroundCamera::BackgroundCamera() {
	// Create OSG Image from CV Mat
	img = new osg::Image;
}

void flipImageV(unsigned char* top, unsigned char* bottom, unsigned int rowSize, unsigned int rowStep)
{
	while (top<bottom)
	{
		unsigned char* t = top;
		unsigned char* b = bottom;
		for (unsigned int i = 0; i<rowSize; ++i, ++t, ++b)
		{
			unsigned char temp = *t;
			*t = *b;
			*b = temp;
		}
		top += rowStep;
		bottom -= rowStep;
	}
}

void BackgroundCamera::update(uint8_t* data, int width, int height)
{
	unsigned char* top = data;
	unsigned char* bottom = top + (height - 1)*3*width;

	flipImageV(top, bottom, width*3, width*3);

	img->setImage(width, height, 3,
		GL_RGB, GL_BGR, GL_UNSIGNED_BYTE,
		data,
		osg::Image::AllocationMode::NO_DELETE, 1);
	img->dirty();
}

osg::Geode* BackgroundCamera::createCameraPlane(int textureWidth, int textureHeight)
{
	// CREATE PLANE TO DRAW TEXTURE
	osg::ref_ptr<osg::Geometry> quadGeometry = osg::createTexturedQuadGeometry(osg::Vec3(0.0f, 0.0f, 0.0f),
		osg::Vec3(textureWidth, 0.0f, 0.0f),
		osg::Vec3(0.0, textureHeight, 0.0),
		0.0f,
		1.0f,
		1.0f,
		0.0f);
	// PUT PLANE INTO NODE
	osg::ref_ptr<osg::Geode> quad = new osg::Geode;
	quad->addDrawable(quadGeometry);
	// DISABLE SHADOW / LIGHTNING EFFECTS
	int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
	quad->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values);
	quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setTextureSize(textureWidth, textureHeight);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	texture->setResizeNonPowerOfTwoHint(false);

	texture->setImage(img);

	// Apply texture to quad
	osg::ref_ptr<osg::StateSet> stateSet = quadGeometry->getOrCreateStateSet();
	stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

	return quad.release();
}

osg::Camera* BackgroundCamera::createCamera(int textureWidth, int textureHeight)
{
	osg::ref_ptr<osg::Geode> quad = createCameraPlane(textureWidth, textureHeight);
	//Bind texture to the quadGeometry, then use the following camera:
	osg::Camera* camera = new osg::Camera;
	// CAMERA SETUP
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	// use identity view matrix so that children do not get (view) transformed
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setClearColor(osg::Vec4(0.f, 0.f, 0.f, 1.0));
	camera->setProjectionMatrixAsOrtho(0.f, textureWidth, 0.f, textureHeight, 1.0, 500.f);
	// set resize policy to fixed
	camera->setProjectionResizePolicy(osg::Camera::ProjectionResizePolicy::FIXED);
	// we don't want the camera to grab event focus from the viewers main camera(s).
	camera->setAllowEventFocus(false);
	// only clear the depth buffer
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//camera->setViewport( 0, 0, screenWidth, screenHeight );
	camera->setRenderOrder(osg::Camera::NESTED_RENDER);
	camera->addChild(quad);
	return camera;
}

Modelrender::Modelrender(int cols, int rows)
{
	width = cols;
	height = rows;
	// OSG STUFF
	// Create viewer
	viewer.setUpViewInWindow(50, 50, width, height);

	// Main Camera
	osg::ref_ptr<osg::Camera>  camera = viewer.getCamera();
	vCamera.createVirtualCamera(camera, width, height);

	// Background-Camera (OpenCV Feed)
	osg::Camera* backgroundCamera = bgCamera.createCamera(width, height);

	// Load Truck Model as Example Scene
	osg::ref_ptr<osg::Node> truckModel = osgDB::readNodeFile("avatar.osg"); //spaceship.osgt; º¬¶¯»­µÄ£º nathan.osg, avatar.osg, bignathan.osg,
	osg::Group* truckGroup = new osg::Group();
	// Position of truck
	osg::PositionAttitudeTransform* position = new osg::PositionAttitudeTransform();

	osgAnimation::BasicAnimationManager* anim =
		dynamic_cast<osgAnimation::BasicAnimationManager*>(truckModel->getUpdateCallback());
	const osgAnimation::AnimationList& list = anim->getAnimationList();
	anim->playAnimation(list[0].get());

	truckGroup->addChild(position);
	position->addChild(truckModel);

	// Set Position of Model
	osg::Vec3 modelPosition(0, 100, 0);
	position->setPosition(modelPosition);

	// Create new group node
	osg::ref_ptr<osg::Group> group = new osg::Group;
	osg::Node* background = backgroundCamera;
	osg::Node* foreground = truckGroup;
	background->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");
	foreground->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
	group->addChild(background);
	group->addChild(foreground);
	background->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	foreground->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	// Add the groud to the viewer
	viewer.setSceneData(group.get());

	angleRoll = 0.0;
}

uint8_t* Modelrender::rend(uint8_t * inputimage)
{
	bgCamera.update(inputimage, width, height);

	//angleRoll += 0.5;

	// Position Parameters: Roll, Pitch, Heading, X, Y, Z
	vCamera.updatePosition(angleRoll, 0, 0, 0, 0, 0);
	viewer.frame();
	//vCamera.image->flipVertical();
	return vCamera.image->data();
}

