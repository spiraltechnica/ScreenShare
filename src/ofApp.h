#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "..\..\..\SpoutSDK\Source\Spout.h"

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

		void exit();

		bool InitGLtexture(GLuint &texID, unsigned int width, unsigned int height);

		SpoutSender *spoutsender;		// A sender object
		char sendername[256];			// Shared memory name
		GLuint sendertexture;			// Local OpenGL texture used for sharing
		bool bInitialized;				// Initialization result
		bool bMemoryShare;				// Texture share compatibility
		unsigned int g_Width, g_Height;	// Global width and height


		ofTexture capturedTexture;

		int capture_src_width;
		int capture_src_height;
		int capture_dst_width;
		int capture_dst_height;
		HDC hSourceDC;
		HDC hCaptureDC;
		HBITMAP hCaptureBitmap;
		HWND window_handle;
		BITMAPINFO bmi = { 0 };

		unsigned char* capturedPixelData;

		ofFbo fbo;
		
		
};
