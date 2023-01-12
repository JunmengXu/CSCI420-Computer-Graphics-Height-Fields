/*
  CSCI 420 Computer Graphics
  Assignment 1: Height Fields
  <Junmeng Xu>
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>

/*
// Use <string> for construct the file name of screenshot
//#include <string>
//using std::string;
//using std::to_string;
//
//int fileIndex = 0;
*/

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
enum display_set { POINT, LINE, TRIANGLE };
enum bpp_set { ONE, THREE };

CONTROLSTATE g_ControlState = ROTATE;
display_set g_DisplayState = POINT;
bpp_set g_DisplayBpp = ONE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* store another image of equal size */
Pic * g_pHeightData2;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
    int i, j;
    Pic *in = NULL;

    if (filename == NULL)
    return;

    /* Allocate a picture buffer */
    in = pic_alloc(640, 480, 3, NULL);

    printf("File to save to: %s\n", filename);

    for (i=479; i>=0; i--) {
        glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                     &in->pix[i*in->nx*in->bpp]);
    }

    if (jpeg_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");

    pic_free(in);
}

void myinit()
{
    /* setup gl view here */
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // enable depth buffering
    glEnable(GL_DEPTH_TEST);
    // interpolate colors during rasterization
    glShadeModel(GL_SMOOTH);
}

/*
 Method_2_POINT: Basic function to render the height field as points
 Iterate over each pixel and draw a point for each pixel using GL_POINTS
 */
void displayPoint()
{
    glBegin(GL_POINTS);

    float maxX = g_pHeightData->nx;
    float maxY = g_pHeightData->ny;

    for(int i=0;i<maxY;i++){
        for(int j=0;j<maxX;j++){
            unsigned char heightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
            float h = heightVal - '0' + 48;
            float x = -0.8 + 1.5 * (j/maxX);
            float y = 0.8 - 1.5 * (i/maxY);
            float z = -0.3 * (h/255);

            glColor3f(h/255, h/255, 1);
            glVertex3f(x, y, z);
        }
    }

    glEnd();
}

/*
 Method_2_LINE: Basic function to render the height field as lines, the result is like a net
 Iterate over each pixel and draw two lines for each pixel with pixel under it and its right pixel using GL_LINES
 */
void displayLine()
{
    glBegin(GL_LINES);

    float maxX = g_pHeightData->nx;
    float maxY = g_pHeightData->ny;

    for(int i=0;i<maxY;i++){
        for(int j=0;j<maxX;j++){
            unsigned char heightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
            float h = heightVal - '0' + 48;
            float x = -0.8 + 1.5 * (j/maxX);
            float y = 0.8 - 1.5 * (i/maxY);
            float z = -0.3 * (h/255);

            if(i<maxY-1){
                unsigned char dheightVal = PIC_PIXEL(g_pHeightData, j, i+1, 0);
                float dh = dheightVal - '0' + 48;
                float dx = -0.8 + 1.5 * (j/maxX);
                float dy = 0.8 - 1.5 * ((i+1)/maxY);
                float dz = -0.3 * (dh/255);

                glColor3f(h/255, h/255, 1);
                glVertex3f(x, y, z);
                glColor3f(dh/255, dh/255, 1);
                glVertex3f(dx, dy, dz);
            }

            if(j<maxX-1){
                unsigned char rheightVal = PIC_PIXEL(g_pHeightData, j+1, i, 0);
                float rh = rheightVal - '0' + 48;
                float rx = -0.8 + 1.5 * ((j+1)/maxX);
                float ry = 0.8 - 1.5 * (i/maxY);
                float rz = -0.3 * (rh/255);

                glColor3f(h/255, h/255, 1);
                glVertex3f(x, y, z);
                glColor3f(rh/255, rh/255, 1);
                glVertex3f(rx, ry, rz);
            }
        }
    }

    glEnd();
}

/*
 Method_2_TRIANGLE: Basic function to render the height field as triangles
 Iterate over each pixel and the pixel under it layer by layer apart from the bottom layer using GL_TRIANGLE_STRIP
 */
void displayTran()
{
      float maxX = g_pHeightData->nx;
      float maxY = g_pHeightData->ny;
      
      for(int i=0;i<maxY-1;i++){
          glBegin(GL_TRIANGLE_STRIP);
          
          for(int j=0;j<maxX;j++){
              unsigned char theightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
              float th = theightVal - '0' + 48;
              float tx = -0.8 + 1.5 * (j/maxX);
              float ty = 0.8 - 1.5 * (i/maxY);
              float tz = -0.3 * (th/255);
              
              unsigned char bheightVal = PIC_PIXEL(g_pHeightData, j, i+1, 0);
              float bh = bheightVal - '0' + 48;
              float bx = -0.8 + 1.5 * (j/maxX);
              float by = 0.8 - 1.5 * ((i+1)/maxY);
              float bz = -0.3 * (bh/255);
              
              glColor3f(th/255, th/255, 1);
              glVertex3f(tx, ty, tz);
              
              glColor3f(bh/255, bh/255, 1);
              glVertex3f(bx, by, bz);
          }
          
          glEnd();
      }
}

/*
 Method_1_GENERAL: Function to render the height field as triangles.
 It is similar to the above one
 but it uses glPolygonMode to render the height field as points, lines ("wireframe for triangles"), and solid triangles.
 */
void displayPolygonMode()
{
    switch(g_DisplayState)
    {
        case POINT:
            /* First Parameters: Specifies the polygons that mode applies to. GL_FRONT_AND_BACK for front- and back-facing polygons.
             Second Parameters: Specifies how polygons will be rasterized.
             */
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
            
        case LINE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        
        case TRIANGLE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
    }
    
    float maxX = g_pHeightData->nx;
    float maxY = g_pHeightData->ny;

    for(int i=0;i<maxY-1;i++){
        glBegin(GL_TRIANGLE_STRIP);
      
        for(int j=0;j<maxX;j++){
            // read x, y, z(height) dimension of current pixel
            unsigned char theightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
            float th = theightVal - '0' + 48;
            float tx = -0.8 + 1.5 * (j/maxX);
            float ty = 0.8 - 1.5 * (i/maxY);
            float tz = -0.3 * (th/255);

            // read x, y, z(height) dimension of the pixel below the current pixel
            unsigned char bheightVal = PIC_PIXEL(g_pHeightData, j, i+1, 0);
            float bh = bheightVal - '0' + 48;
            float bx = -0.8 + 1.5 * (j/maxX);
            float by = 0.8 - 1.5 * ((i+1)/maxY);
            float bz = -0.3 * (bh/255);

            glColor3f(th/255, th/255, 1);
            glVertex3f(tx, ty, tz);

            glColor3f(bh/255, bh/255, 1);
            glVertex3f(bx, by, bz);
        }
        
        glEnd();
    }
}

/*
 ***EXTRAS***
 {Color the vertices based on color values taken from another image of equal size.
 However, your code should still support also smooth color gradients as per the core requirements.}
 Function to render the height field as triangles.
 It uses glPolygonMode to render the height field as points, lines ("wireframe for triangles"), and solid triangles.
 Also the vertices are colored based on color values taken from another image of equal size.
 */
void displayPolygonModeExtraforColor()
{
    switch(g_DisplayState)
    {
        case POINT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
            
        case LINE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        
        case TRIANGLE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
    }
    
    float maxX = g_pHeightData->nx;
    float maxY = g_pHeightData->ny;
    float maxX2 = g_pHeightData2->nx;
    float maxY2 = g_pHeightData2->ny;

    for(int i=0;i<maxY-1;i++){
        glBegin(GL_TRIANGLE_STRIP);
      
        for(int j=0;j<maxX;j++){
            // read x, y, z(height) dimension from the first image
            unsigned char theightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
            float th = theightVal - '0' + 48;
            float tx = -0.8 + 1.5 * (j/maxX);
            float ty = 0.8 - 1.5 * (i/maxY);
            float tz = -0.3 * (th/255);

            unsigned char bheightVal = PIC_PIXEL(g_pHeightData, j, i+1, 0);
            float bh = bheightVal - '0' + 48;
            float bx = -0.8 + 1.5 * (j/maxX);
            float by = 0.8 - 1.5 * ((i+1)/maxY);
            float bz = -0.3 * (bh/255);
            
            // read height dimension from the second image to render color
            unsigned char theightVal2 = PIC_PIXEL(g_pHeightData2, j, i, 0);
            float th2 = theightVal2 - '0' + 48;
            
            unsigned char bheightVal2 = PIC_PIXEL(g_pHeightData2, j, i, 0);
            float bh2 = bheightVal2 - '0' + 48;

            glColor3f(th2/255, th2/255, 1);
            glVertex3f(tx, ty, tz);

            glColor3f(bh2/255, bh2/255, 1);
            glVertex3f(bx, by, bz);
        }
        
        glEnd();
    }
}

/*
 ***EXTRAS***
 {Support color (bpp=3) in input images.}
 */
void displayPolygonModeExtraforBpp()
{
    switch(g_DisplayState)
    {
        case POINT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
            
        case LINE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        
        case TRIANGLE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
    }
    
    float maxX = g_pHeightData->nx;
    float maxY = g_pHeightData->ny;

    for(int i=0;i<maxY-1;i++){
        glBegin(GL_TRIANGLE_STRIP);
      
        for(int j=0;j<maxX;j++){
            // read red channel chan of pixel (j,i) of picture when chan=0
            unsigned char theightValRed = PIC_PIXEL(g_pHeightData, j, i, 0);
            // read green channel chan of pixel (j,i) of picture when chan=1
            unsigned char theightValGreen = PIC_PIXEL(g_pHeightData, j, i, 1);
            // read blue channel chan of pixel (j,i) of picture when chan=2
            unsigned char theightValBlue = PIC_PIXEL(g_pHeightData, j, i, 2);
            float thr = theightValRed - '0' + 48;
            float thg = theightValGreen - '0' + 48;
            float thb = theightValBlue - '0' + 48;
            float tx = -0.8 + 1.5 * (j/maxX);
            float ty = 0.8 - 1.5 * (i/maxY);
            float tz = -0.3 * (0.3*thr/255 + 0.3*thg/255 + 0.3*thb/255);

            unsigned char bheightValRed = PIC_PIXEL(g_pHeightData, j, i+1, 0);
            unsigned char bheightValGreen = PIC_PIXEL(g_pHeightData, j, i+1, 1);
            unsigned char bheightValBlue = PIC_PIXEL(g_pHeightData, j, i+1, 2);
            float bhr = bheightValRed - '0' + 48;
            float bhg = bheightValGreen - '0' + 48;
            float bhb = bheightValBlue - '0' + 48;
            float bx = -0.8 + 1.5 * (j/maxX);
            float by = 0.8 - 1.5 * ((i+1)/maxY);
            float bz = -0.3 * (0.3*bhr/255 + 0.3*bhg/255 + 0.3*bhb/255);

            /* Render color with blue theme */
            //glColor3f((thr+thg+thb)/765, (thr+thg+thb)/765, 1);
            /* render rgb color */
            glColor3f(thr/255, thg/255, thb/255);
            glVertex3f(tx, ty, tz);

            /* Render color with blue theme */
            //glColor3f((bhr+bhg+bhb)/765, (bhr+bhg+bhb)/765, 1);
            /* render rgb color */
            glColor3f(bhr/255, bhg/255, bhb/255);
            glVertex3f(bx, by, bz);
        }
        
        glEnd();
    }
}

/*
 Save screenshots by defining filename with index
 */
void saveScreenshots()
{
    char myFilenm[2048];
    for (int i=0;i<1000;i++)
    {
        sprintf(myFilenm, "screenshots/anim.%04d.jpg", i);
        saveScreenshot(myFilenm);
    }
    
    // string filename = "screenshots/pic";
    // filename.append(to_string(fileIndex));
    // filename.append(".jpg");
    // char *fileName = (char*)filename.c_str();
    // saveScreenshot(fileName);
    // fileIndex++;
}

void display()
{
    // clear buffers, use depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    // reset transformation
    glLoadIdentity();

    // render as a perspective view
    gluPerspective(60.0, 1.0 * 64 / 48, -0.01, 1000.0);

    // precede it with the rotation/translation/scaling
    glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
    glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);
    glRotatef(g_vLandRotate[0], 1, 0, 0);
    glRotatef(g_vLandRotate[1], 0, 1, 0);
    glRotatef(g_vLandRotate[2], 0, 0, 1);

    
    if(g_DisplayBpp == ONE)
    {
        /*
         Method 1 to draw the height field as points, lines ("wireframe for triangles"), and solid triangles.
         */
        displayPolygonMode();
        
        /*
         Method 2 (Alternative) to draw the height field as points, lines, and solid triangles.
         Also set key to control the state of display.
         */
        /*
         switch(g_DisplayState)
         {
             case POINT:
                 displayPoint();
                 break;

             case LINE:
                 displayLine();
                 break;

             case TRIANGLE:
                 displayTran();
                 break;
         }
         */
        
        /*
         ***Extra***
         {Color the vertices based on color values taken from another image of equal size.
         However, your code should still support also smooth color gradients as per the core requirements.}
         */
         //displayPolygonModeExtraforColor();
    }else
    {
        /*
         ***Extra***
         {Support color (bpp=3) in input images.}
         */
        displayPolygonModeExtraforBpp();
    }

    // double buffer flush
    glutSwapBuffers();
}


void menufunc(int value)
{
    switch (value)
    {
        case 0:
            exit(0);
            break;
    }
}

void doIdle()
{
    /* do some stuff... */
    display();
    /* make the screen update */
    glutPostRedisplay();
}

/* converts mouse drags into information about
 rotation/translation/scaling
 */
void mousedrag(int x, int y)
{
    int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};

    switch (g_ControlState)
    {
        case TRANSLATE:
            if (g_iLeftMouseButton)
            {
                g_vLandTranslate[0] += vMouseDelta[0]*0.01;
                g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandTranslate[2] += vMouseDelta[1]*0.01;
            }
            break;
        case ROTATE:
            if (g_iLeftMouseButton)
            {
                g_vLandRotate[0] += vMouseDelta[1];
                g_vLandRotate[1] += vMouseDelta[0];
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandRotate[2] += vMouseDelta[1];
            }
            break;
        case SCALE:
            if (g_iLeftMouseButton)
            {
                g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
                g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
            }
            break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
    }

    // Move this part to keybutton part due to the use of MAC
    //  switch(glutGetModifiers())
    //  {
    //    case GLUT_ACTIVE_CTRL:
    //      g_ControlState = TRANSLATE;
    //      break;
    //    case GLUT_ACTIVE_SHIFT:
    //      g_ControlState = SCALE;
    //      break;
    //    default:
    //      g_ControlState = ROTATE;
    //      break;
    //  }

    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

/*
 Because of issues of keyboard mapping on MAC for CTRL or ALT keys for g_ControlState switch,
 use glutKeyboardFunc and use other keys (such as ‘z’, ‘x’, ‘c’, 'a', 's', 'd', 'q') to rebind the control.
 */
void keybutton(unsigned char key, int x, int y)
{
//    saveScreenshots();
    switch (key){
        // 'z', 'x', 'c' keys are used to control: translate, scale, or rotate graphics.
        case 'z':
            g_ControlState = TRANSLATE;
            break;

        case 'x':
            g_ControlState = SCALE;
            break;

        case 'c':
            g_ControlState = ROTATE;
            break;
        
        // 'a', 's', 'd' keys are used to change the method of rendering the height field
        case 'a':
            g_DisplayState = POINT;
            break;
        
        case 's':
            g_DisplayState = LINE;
            break;
        
        case 'd':
            g_DisplayState = TRIANGLE;
            break;

        // 'q' key is used to change support color (bpp=1) and (bpp=3) in input images
        case 'q':
            if(g_DisplayBpp == ONE)
            {
                g_DisplayBpp = THREE;
            }else
            {
                g_DisplayBpp = ONE;
            }
            break;
            
        case 'w':
            saveScreenshot("SantaMonicaMountains-256-HF.jpg");
            break;
    default:
         break;
    }
}

int main (int argc, char ** argv)
{
    if (argc<2)
    {
        printf ("usage: %s heightfield.jpg\n", argv[0]);
        exit(1);
    }

    g_pHeightData = jpeg_read(argv[1], NULL);
    if (!g_pHeightData)
    {
        printf ("error reading %s.\n", argv[1]);
        exit(1);
    }
    
    if (argc==3)
    {
        g_pHeightData2 = jpeg_read(argv[2], NULL);
        if (!g_pHeightData2)
        {
            printf ("error reading the second image %s.\n", argv[2]);
            exit(1);
        }
        float maxX = g_pHeightData->nx;
        float maxY = g_pHeightData->ny;
        float maxX2 = g_pHeightData2->nx;
        float maxY2 = g_pHeightData2->ny;
        if(maxX != maxX2 || maxY != maxY2)
        {
            printf ("error: the two images have different size.\n");
            exit(1);
        }
    }

    glutInit(&argc,argv);

    /*
    create a window here..should be double buffered and use depth testing

    the code past here will segfault if you don't have a window set up....
    replace the exit once you add those calls.
    */
    
    // request double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    // set window size
    glutInitWindowSize(640, 480);

    // set window position
    glutInitWindowPosition(0, 0);

    // creates a window
    glutCreateWindow("Height Field");

    /* tells glut to use a particular display function to redraw */
    glutDisplayFunc(display);

    /* allow the user to quit using the right mouse button menu */
    g_iMenuId = glutCreateMenu(menufunc);
    glutSetMenu(g_iMenuId);
    glutAddMenuEntry("Quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* replace with any animate code */
    glutIdleFunc(doIdle);

    /* callback for mouse drags */
    glutMotionFunc(mousedrag);
    /* callback for idle mouse movement */
    glutPassiveMotionFunc(mouseidle);
    /* callback for mouse button changes */
    glutMouseFunc(mousebutton);
    /* Due to issues of keyboard mapping on MAC, I use glutKeyboardFunc for g_ControlState switch */
    glutKeyboardFunc(keybutton);

    /* do initialization */
    myinit();

    glutMainLoop();

    return(0);
}
