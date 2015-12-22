#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "..\..\..\SpoutSDK\Spout.h"

using namespace cv;
using namespace ofxCv;


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void capture_screen();
		void capture_screen2();
		void capture_screen3();


		cv::Mat inputImg,correctedColorImg;
		ofImage p;


		void exit();

		bool InitGLtexture(GLuint &texID, unsigned int width, unsigned int height);

		SpoutSender *spoutsender;		// A sender object
		char sendername[256];			// Shared memory name
		GLuint sendertexture;			// Local OpenGL texture used for sharing
		bool bInitialized;				// Initialization result
		bool bMemoryShare;				// Texture share compatibility
		ofImage myTextureImage;			// Texture image for the 3D demo
		unsigned int g_Width, g_Height;	// Global width and height

		
};
