
AUTHOR
------
CSCI 420 Computer Graphics
Assignment 1: Height Fields
<Junmeng Xu>
<USC ID 8912001452>


CONTENTS OF THIS FILE
---------------------
 * Introduction
 * Environment
 * Basic functions
 * Program's features
 * Extra credit
 * Operation


INTRODUCTION
------------
In this project, I implemented Basic Requirements and Animation Requirements, and also implemented two tasks for extra credit.


ENVIRONMENT
-----------
This program is developed in Mac OS X. I installed XCode and the command line tools and compiled the pic library before compiling the starter code. Under Mac OS X, I got an error like this 'error: X11/Xlib.h: No such file or directory', so I installed Xquartz.


BASIC FUNCTIONS
---------------
* This program handles at least a 256x256 image for height field at interactive frame rates (window size of 640x480).
* Render the height field as points, lines ("wireframe for triangles"), and solid triangles. Users can switch between the three because of the binding of functions with keys.
* This program renders height field as a perspective view, utilizing GL's depth buffer for hidden surface removal.
* Use input from the mouse to spin the heightfield around using glRotate.
* Use input from the mouse to move the heightfield around using glTranslate.
* Use input from the mouse to change the dimensions of the heightfield using glScale
* Color the vertices using some smooth gradient.


PROGRAM'S FEATURES
------------------
* Because of issues of keyboard mapping on MAC for CTRL or ALT keys for g_ControlState switch, the program used glutKeyboardFunc and use other keys (‘z’, ‘x’, ‘c’) to rebind the control of translation/scaling/rotation.
* Optimized my program by minimizing the number of calls to glBegin(), and used GL_TRIANGLE_STRIP.
* In terms of rendering the height field as points, lines, and solid triangles, I implemented this by two different methods. One is to use glBegin(GL_POINTS), glBegin(GL_LINES) and glBegin(GL_TRIANGLE_STRIP) respectively to render the height field (shown as displayPoint(), displayLine() and displayTran()) . The other method is to use glBegin(GL_TRIANGLE_STRIP) and glPolygonMode(GL_FRONT_AND_BACK, GL_POINT/GL_LINE/GL_FILL) to do it (shown as displayPolygonMode()).
* Keys 'a', 's', 'd' are used to change the method of rendering the height field (points, lines ("wireframe for triangles"), and solid triangles)
* Saved all screenshots through 'sprintf(myFilenm, "anim.%04d.jpg", i)' to name filenames.
* Used command 'ffmpeg -r 15 -f image2 -i anim.%04d.jpg heightfield.mp4' to make the video clip with a frame rate of 15 frames per second. However, the video effect was a little stuttering due to the incoherence of the screenshots, so I used Screen recording software to make the video. But I want to say that I have tried to use ffmpeg to synthesize video from pictures.


EXTRA CREDIT
------------
* Support color (bpp=3) in input images.
  ** Read red/green/blue channel chan for each pixel of picture by setting chan=0 or 1 or 2.
  ** Height value (z) is determined equally by the height value of red, green, and blue.
  ** Render RGB color for the height field default, but I also coded that render color with a blue theme

* Color the vertices based on color values taken from another image of equal size. My code still support also smooth color gradients as per the core requirements.
  ** If argv[2] exists, then it would be the name of the second image.
  ** Run ' ./assign1 GrandTeton-256.jpg OhioPyle-256.jpg ' to read two images with the same size.
  ** If the two images have different size, then the program would print an error log to hint the user.
  ** displayPolygonModeExtraforColor() is developed for this and for each pixel in the first image, the function color it based on color values of the pixel with the same position taken from the second image.


OPERATION
------------
* Mouse
  ** LeftMouseButton: drag to rotate/translate/scale x and y dimensions
  ** MiddleMouseButton: drag to rotate/translate/scale z dimensions
  ** RightMouseButton: open menu and you can choose to quit the running program
* Keyboard
  ** Z: switch the controlState to TRANSLATE
  ** X: switch the controlState to SCALE
  ** C: switch the controlState to ROTATE
  ** A: render the height field as POINTs
  ** S: render the height field as LINEs
  ** D: render the height field as SOLID TRIANGLEs
  ** Q: switch the mode of supporting color (bpp=1) and (bpp=3) in input images
