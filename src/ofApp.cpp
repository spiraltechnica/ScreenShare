#include "ofApp.h"
#include <Windows.h>
using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
	inputImg = cv::Mat(ofGetScreenHeight(), ofGetScreenWidth(), CV_8UC4);
	correctedColorImg = cv::Mat(ofGetScreenHeight(), ofGetScreenWidth(), CV_8UC4);
	p.allocate(ofGetScreenWidth(), ofGetScreenHeight(), ofImageType::OF_IMAGE_COLOR_ALPHA);
	
	
	ofEnableAlphaBlending();
	ofBackground(0, 0, 0, 255);

	ofSetVerticalSync(true);							// lock to monitor
	//ofEnableNormalizedTexCoords();						// explicitly normalize tex coords for ofBox

	bInitialized = false;							// Spout sender initialization


	strcpy_s(sendername, "ScreenShare");			// Set the sender name
	ofSetWindowTitle(sendername);						// show it on the title bar
	//ofSetWindowShape(g_Width, g_Height);				// Set the window size
	// 3D drawing setup for a sender
	glEnable(GL_DEPTH_TEST);							// enable depth comparisons and update the depth buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	ofDisableArbTex();									// needed for textures to work


	spoutsender = new SpoutSender;						// Create a new Spout sender
	sendertexture = 0;									// Initially there is no local OpenGL texture ID
	//InitGLtexture(sendertexture, ofGetWindowWidth(), ofGetWindowHeight());	// Create an OpenGL texture for data transfers

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
		spoutsender->SetDX9(true);

		bInitialized = spoutsender->CreateSender(sendername, ofGetWindowWidth(), ofGetWindowHeight());
		// Detect texture share compatibility for status display
		bMemoryShare = spoutsender->GetMemoryShareMode();
	}


	p.draw(0, 0);



	if (bInitialized) {

		// Grab the screen into the local spout texture
		glBindTexture(GL_TEXTURE_2D, sendertexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		glBindTexture(GL_TEXTURE_2D, 0);

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
		spoutsender->SendTexture(sendertexture, GL_TEXTURE_2D, ofGetWindowWidth(), ofGetWindowHeight());

		// Show what it is sending
		/*	ofSetColor(255);
		if(!bMemoryShare) sprintf_s(str, "Sending as : [%s]", sendername);
		else sprintf_s(str, "Memoryshare sender");
		ofDrawBitmapString(str, 20, 20);

		// Show fps
		sprintf_s(str, "fps: %3.3d", (int)ofGetFrameRate());
		ofDrawBitmapString(str, ofGetWidth()-120, 20);*/
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
	InitGLtexture(sendertexture, ofGetWindowWidth(), ofGetWindowHeight());
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::capture_screen()
{
	HDC hDC = GetDC(NULL); //Get full screen  
	HDC hMemDC = CreateCompatibleDC(hDC);
	SIZE size;
	size.cx = GetSystemMetrics(SM_CXSCREEN);
	size.cy = GetSystemMetrics(SM_CYSCREEN);

	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, size.cx, size.cy);
	if (hBitmap)
	{
		HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
		BitBlt(hMemDC, 0, 0, size.cx, size.cy, hDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOld);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hDC);

		GetBitmapBits(hBitmap, ofGetScreenHeight()*ofGetScreenWidth() * 4, inputImg.data);

		cv::cvtColor(inputImg, correctedColorImg, CV_BGR2RGBA);
		 
		ofxCv::toOf(correctedColorImg, p);
		p.update();

		DeleteObject(hBitmap);
	}
}

void ofApp::capture_screen2()
{

	HDC screen_dc = GetDC(0); //Get full screen
	HDC shot_dc = CreateCompatibleDC(screen_dc);
	HBITMAP shot_bitmap = CreateCompatibleBitmap(screen_dc, ofGetScreenWidth(), ofGetScreenHeight());
	HGDIOBJ old_obj = SelectObject(shot_dc, shot_bitmap);

	if (!BitBlt(shot_dc, 0, 0, ofGetScreenWidth(), ofGetScreenHeight(), screen_dc, 0, 0, SRCCOPY)) {
		printf("error: BitBlt failed\n");
	}

	GetBitmapBits(shot_bitmap, ofGetScreenHeight()*ofGetScreenWidth() * 4, inputImg.data);

	cv::cvtColor(inputImg, correctedColorImg, CV_BGR2RGBA);

	ofxCv::toOf(correctedColorImg, p);
	p.update();
	

	DeleteDC(shot_dc);
	DeleteDC(screen_dc);
	DeleteObject(shot_bitmap);
	SelectObject(shot_dc, old_obj);
	
}

//not working..find out why...
void ofApp::capture_screen3() {
	HDC hdc = GetDC(NULL); // get the desktop device context
	HDC hDest = CreateCompatibleDC(hdc); // create a device context to use yourself

	// get the height and width of the screen
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	// create a bitmap
	HBITMAP hbDesktop = CreateCompatibleBitmap( hdc, width, height);

	// use the previously created device context with the bitmap
	SelectObject(hDest, hbDesktop);

	// copy from the desktop device context to the bitmap device context
	// call this once per 'frame'
	BitBlt(hDest, 0,0, width, height, hdc, 0, 0, SRCCOPY);

	GetBitmapBits(hbDesktop, ofGetScreenHeight()*ofGetScreenWidth() * 4, inputImg.data);

	cv::cvtColor(inputImg, correctedColorImg, CV_BGR2RGBA);

	ofxCv::toOf(correctedColorImg, p);
	p.update();
	// after the recording is done, release the desktop context you got..
	ReleaseDC(NULL, hdc);

	// ..and delete the context you created
	DeleteDC(hDest);

}

bool ofApp::InitGLtexture(GLuint &texID, unsigned int width, unsigned int height)
{
	if (texID != 0) glDeleteTextures(1, &texID);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

//--------------------------------------------------------------
void ofApp::exit() {


	// Release the sender - this can be used for repeated 
	// Create and Release of a sender within the program
	spoutsender->ReleaseSender();

	// At program termination, clean up everything
	delete spoutsender;
	spoutsender = NULL;


}

