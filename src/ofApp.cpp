#include "ofApp.h"
#include <Windows.h>
using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){

	/*capture_src_width = ofGetScreenWidth();
	capture_src_height = ofGetScreenHeight();
	capture_dst_width = ofGetScreenWidth(); 
	capture_dst_height = ofGetScreenHeight();*/

	hSourceDC = GetDC(NULL);//GetDC( NULL ); for entire desktop	
	capture_src_width = GetDeviceCaps(hSourceDC, HORZRES);
	capture_src_height = GetDeviceCaps(hSourceDC, VERTRES);
	capture_dst_width = GetDeviceCaps(hSourceDC, HORZRES);
	capture_dst_height = GetDeviceCaps(hSourceDC, VERTRES);


	fbo.allocate(capture_dst_width, capture_dst_height, GL_RGBA);

	capturedPixelData = new unsigned char[capture_dst_width * capture_dst_height * 3];
	capturedTexture.allocate(capture_dst_width, capture_dst_height, GL_RGB);
	
	
	
	
	ofEnableAlphaBlending();
	ofBackground(0, 0, 0, 255);

	ofSetVerticalSync(true);							// lock to monitor
	

	bInitialized = false;							// Spout sender initialization

	
	strcpy_s(sendername, "ScreenShare");			// Set the sender name
	ofSetWindowTitle(sendername);						// show it on the title bar
	//ofSetWindowShape(g_Width, g_Height);				// Set the window size
	// 3D drawing setup for a sender
	//glEnable(GL_DEPTH_TEST);							// enable depth comparisons and update the depth buffer
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	ofDisableArbTex();								// needed for textures to work


	spoutsender = new SpoutSender;						// Create a new Spout sender
	sendertexture = 0;									// Initially there is no local OpenGL texture ID
	//InitGLtexture(sendertexture, ofGetWindowWidth(), ofGetWindowHeight());	// Create an OpenGL texture for data transfers
	
	fbo.begin();
	ofClear(255, 255, 255, 0);
	fbo.end();
}

//--------------------------------------------------------------
void ofApp::update(){
	capture_screen();
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	// INITIALIZE A SENDER
	if (!bInitialized) {
		// Optionally set for DirectX 9 instead of default DirectX 11 functions
		//spoutsender->SetDX9(true);

		bInitialized = spoutsender->CreateSender(sendername, capture_dst_width, capture_dst_height);
		// Detect texture share compatibility for status display
		bMemoryShare = spoutsender->GetMemoryShareMode();
	}

	if (bInitialized) {

		// Draw the captured desktop into fbo, then bind the sendertexture to the fbo, and copy the contents of the fbo into the sendertexture		
		fbo.begin();
		capturedTexture.draw(0, 0);
		glBindTexture(GL_TEXTURE_2D, sendertexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, capture_dst_width, capture_dst_height);
		glBindTexture(GL_TEXTURE_2D, 0);
		fbo.end();

		//fit fbo to window bounds and position in middle
		if (ofGetWindowWidth() > 0 && ofGetWindowHeight() > 0 && (ofGetWindowWidth() / ofGetWindowHeight() <= 1280.0f / 720.0f)) {
			fbo.draw(0, (ofGetWindowHeight() / 2) - (ofGetWindowWidth()/1.77778) / 2, ofGetWindowWidth(), ofGetWindowWidth() / 1.77778);
		}
		else {
			fbo.draw((ofGetWindowWidth()/2)-(ofGetWindowHeight()*1.77778)/2, 0, ofGetWindowHeight()*1.77778, ofGetWindowHeight());
		}		
	
		//
		// Send the texture out for all receivers to use
		//
		// Notes :
		// (1)	If a host calls SendTexture with a framebuffer object bound,
		//		include the FBO id in the SendTexture call so that the binding is restored
		//		afterwards because Spout makes use of its own FBO for intermediate rendering.
		// (2)	openGL/DirectX coordinates make our texture come out inverted so the texture
		//		is inverted	when transferring it to the shared texture. You can specify false
		//		to disable this default then the result comes out apparently inverted.
		//
		spoutsender->SendTexture(sendertexture, GL_TEXTURE_2D, capture_dst_width, capture_dst_height, false, fbo.getId());
		
	}


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_LEFT) {

		ofSetWindowPosition(ofGetWindowPositionX() - 1, ofGetWindowPositionY());
	}
	if (key == OF_KEY_RIGHT) {

		ofSetWindowPosition(ofGetWindowPositionX() + 1, ofGetWindowPositionY());
	}
	if (key == OF_KEY_UP) {

		ofSetWindowPosition(ofGetWindowPositionX(), ofGetWindowPositionY() - 1);
	}
	if (key == OF_KEY_DOWN) {

		ofSetWindowPosition(ofGetWindowPositionX(), ofGetWindowPositionY() + 1);
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	// Update the sender texture to receive the new dimensions
	InitGLtexture(sendertexture, capture_src_width, capture_src_height);
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

bool ofApp::InitGLtexture(GLuint &texID, unsigned int width, unsigned int height)
{
	if (texID != 0) glDeleteTextures(1, &texID);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}


void ofApp::capture_screen() {
	
	//hSourceDC = GetDC(NULL);//GetDC( NULL ); for entire desktop; line moved to setup function
	hCaptureDC = CreateCompatibleDC(hSourceDC);
	hCaptureBitmap = CreateCompatibleBitmap(hSourceDC, capture_dst_width, capture_dst_height);

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = capture_dst_width;
	bmi.bmiHeader.biHeight = capture_dst_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	SelectObject(hCaptureDC, hCaptureBitmap);

	// flip right-side up:
	StretchBlt(hCaptureDC, 0, capture_dst_height, capture_dst_width, -capture_dst_height, hSourceDC, 0, 0, capture_src_width, capture_src_height, SRCCOPY);
	// keep upside-down:
	///StretchBlt( hCaptureDC, 0, 0, capture_dst_width, capture_dst_height, hSourceDC, 0, 0, capture_src_width, capture_src_height, SRCCOPY );
	//ReleaseDC( window_handle, hSourceDC );

	if (GetDIBits(hCaptureDC, hCaptureBitmap, 0, capture_dst_height, (void*)capturedPixelData, &bmi, DIB_RGB_COLORS) == 0)
	{
		cout << "Error: Failed to copy the pixels" << flush;
	}

	capturedTexture.loadData(capturedPixelData, capture_dst_width, capture_dst_height, GL_BGR);
	
	// after the recording is done, release the desktop context you got..
	
	DeleteDC(hCaptureDC);
	DeleteObject( hCaptureBitmap );
}
//--------------------------------------------------------------
void ofApp::exit() {
	//release our source DC that was initialised in the setup function
	ReleaseDC(NULL, hSourceDC);


	// Release the sender - this can be used for repeated 
	// Create and Release of a sender within the program
	spoutsender->ReleaseSender();

	// At program termination, clean up everything
	delete spoutsender;
	spoutsender = NULL;


}

