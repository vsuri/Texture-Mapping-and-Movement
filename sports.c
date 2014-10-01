/*
 Vinay Suri
 Sports; with textures
 
 Controls:
 - To move person 1: ^-w <-a v-s >-d
 - Record functions:
 z - Start
 x - Stop
 c - Load
 v - Reset
 b - Playback
 n - Save
 - Toggle Syncronization: m
 
 */

#include <GL/glut.h>
#include <stdio.h>



#define TORSO_HEIGHT 2.5
#define UPPER_ARM_HEIGHT 1.5
#define LOWER_ARM_HEIGHT 1.0
#define UPPER_LEG_RADIUS  0.25
#define LOWER_LEG_RADIUS  0.25
#define LOWER_LEG_HEIGHT 1.5
#define UPPER_LEG_HEIGHT 1.5
#define UPPER_LEG_RADIUS  0.25
#define TORSO_RADIUS 0.5
#define UPPER_ARM_RADIUS  0.25
#define LOWER_ARM_RADIUS  0.25
#define HEAD_HEIGHT 0.75
#define HEAD_RADIUS 0.5

#define BALL_RADIUS 0.5
#define BALL_HEIGHT 0.5

#define NET_HEIGHT 10
#define NET_RADIUS .125

// Recording

#define MAXEVENTS 1000
#define RECORDSIZE 8

// event_buffer contains the saved user inputs. Each event is of RECORDSIZE.
// E.g., if you only need to store the object_id and one transformation value (such as angle) per
// event, then RECORDSIZE is 2.

int event_buffer[MAXEVENTS*RECORDSIZE];
int sync_buffer[MAXEVENTS*RECORDSIZE];
// event_ptr is for recording into the event_buffer array.
int event_ptr = 0;

// playback_ptr is for reading/playing back from the event_buffer array.
int playback_ptr=0;

//recordMode and playbackMode are flags used for recording and playback.
int recordMode = 0;
int playbackMode = 0;

//Recorded events are saved to <first nameLastInitial>
FILE *jFile = NULL;
char *fileName = "vinays.txt";

int sync = -1;


int max_w=500;
int max_h=500;

// Used to move Person 1
float x=0.0;
float y=-10.0;

// Used to move ball
float ball_x=-5.0;
float ball_y=0.0;


typedef float point[3];

static GLfloat theta[11] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,
    180.0,0.0,180.0,0.0}; /* initial joint angles */

static GLfloat theta2[11] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,
    180.0,0.0,180.0,0.0}; /* initial joint angles */
static GLint axis = -1;
static GLint sync_axis = -1;

/* Define the three parts */
/* Note use of push/pop to return modelview matrix
 to its state before functions were entered and use
 rotation, translation, and scaling to create instances
 of symbols (cube and cylinder */

GLUquadricObj *t, *h, *lua, *lla, *rua, *rla, *lll, *rll, *rul, *lul;
GLUquadricObj *ball;

GLfloat mat_specular[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_diffuse[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_ambient[]={0.0, 0.0, 1.0, 1.0};
GLfloat mat_shininess={100.0};

double size=1.0;

GLuint texture[4];

GLuint LoadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;
   
    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );
   
    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to itís array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
   
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   
    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture )
{
    glDeleteTextures( 1, &texture );
}

void square (void) {
    glPushMatrix();
   
    glBindTexture( GL_TEXTURE_2D, texture[0] ); //bind our texture to our shape
   
    glBegin (GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, -10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, -10.0);
    glEnd();
   
    glBindTexture( GL_TEXTURE_2D, texture[2] ); //bind our texture to our shape
   
    glBegin (GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5.0,-5.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-10, 10, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-10, -10, 0.0);
    glEnd();
   
    glBindTexture( GL_TEXTURE_2D, texture[3] ); //bind our texture to our shape
    glBegin (GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(5.0,-5.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(5, 5, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 10, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -10, 0.0);
    glEnd();
   
    glBindTexture( GL_TEXTURE_2D, texture[1] ); //bind our texture to our shape
    glBegin (GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5.0,-5.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(5, -5, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, -10, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-10, -10, 0.0);
   
    glEnd();
   
    glPopMatrix();
}


void drawball()
{
    glPushMatrix();
    glTranslatef(0.0, 0.5*BALL_HEIGHT,0.0);
    glScalef(BALL_RADIUS, BALL_HEIGHT, BALL_RADIUS);
    gluSphere(ball,1.0,10,10);
    glPopMatrix();
}

//////  Person 1 functions  //////

void torso()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(t,TORSO_RADIUS, TORSO_RADIUS, TORSO_HEIGHT,10,10);
    glPopMatrix();
}

void head()
{
    glPushMatrix();
    glTranslatef(0.0, 0.5*HEAD_HEIGHT,0.0);
    glScalef(HEAD_RADIUS, HEAD_HEIGHT, HEAD_RADIUS);
    gluSphere(h,1.0,10,10);
    glPopMatrix();
}

void left_upper_arm()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(lua,UPPER_ARM_RADIUS, UPPER_ARM_RADIUS, UPPER_ARM_HEIGHT,10,10);
    glPopMatrix();
}

void left_lower_arm()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(lla,LOWER_ARM_RADIUS, LOWER_ARM_RADIUS, LOWER_ARM_HEIGHT,10,10);
    glPopMatrix();
}

void right_upper_arm()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(rua,UPPER_ARM_RADIUS, UPPER_ARM_RADIUS, UPPER_ARM_HEIGHT,10,10);
    glPopMatrix();
}

void right_lower_arm()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(rla,LOWER_ARM_RADIUS, LOWER_ARM_RADIUS, LOWER_ARM_HEIGHT,10,10);
    glPopMatrix();
}

void left_upper_leg()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(lul,UPPER_LEG_RADIUS, UPPER_LEG_RADIUS, UPPER_LEG_HEIGHT,10,10);
    glPopMatrix();
}

void left_lower_leg()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(lll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS, LOWER_LEG_HEIGHT,10,10);
    glPopMatrix();
}

void right_upper_leg()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(rul,UPPER_LEG_RADIUS, UPPER_LEG_RADIUS, UPPER_LEG_HEIGHT,10,10);
    glPopMatrix();
}

void right_lower_leg()
{
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(rll,LOWER_LEG_RADIUS, LOWER_LEG_RADIUS, LOWER_LEG_HEIGHT,10,10);
    glPopMatrix();
}

void drawObjects(GLenum mode)
{
    // Person 1 :
    //glPopMatrix();
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);  


    glLoadIdentity();
    //glColor3f(0.0, 0.0, 0.0);
   
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
   
    glEnable( GL_TEXTURE_2D );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   
    gluLookAt(0.0,-7.0,8.0,0.0,-9.0,4.0,0.0,2.0,0.0);
    glTranslatef(0.0, 0.0, 10);

    square();
    glScalef(.5,.5,.5);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glInitNames();
    glPushName(0);

    glTranslatef(x, y, 0.0);
   
   
    if(mode == GL_SELECT ) glLoadName(1);
    glColor3f(1.0, 1.0, 1.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    torso();
    glPushMatrix();
   
    if(mode == GL_SELECT ) glLoadName(2);
    glTranslatef(0.0, TORSO_HEIGHT+0.5*HEAD_HEIGHT, 0.0);
    ///glRotatef(theta[1], 1.0, 0.0, 0.0);
    ///glRotatef(theta[2], 0.0, 1.0, 0.0);
    glTranslatef(0.0, -0.5*HEAD_HEIGHT, 0.0);
    head();
   
    if(mode == GL_SELECT ) glLoadName(3);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-(TORSO_RADIUS+UPPER_ARM_RADIUS), 0.9*TORSO_HEIGHT, 0.0);
    glRotatef(theta[3], 1.0, 0.0, 0.0);
    left_upper_arm();
   
    if(mode == GL_SELECT ) glLoadName(4);
    glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
    glRotatef(theta[4], 1.0, 0.0, 0.0);
    left_lower_arm();
   
    if(mode == GL_SELECT ) glLoadName(5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(TORSO_RADIUS+UPPER_ARM_RADIUS, 0.9*TORSO_HEIGHT, 0.0);
    glRotatef(theta[5], 1.0, 0.0, 0.0);
    right_upper_arm();
   
    if(mode == GL_SELECT ) glLoadName(6);
    glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
    glRotatef(theta[6], 1.0, 0.0, 0.0);
    right_lower_arm();
   
    if(mode == GL_SELECT ) glLoadName(7);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-(TORSO_RADIUS+UPPER_LEG_RADIUS), 0.1*UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[7], 1.0, 0.0, 0.0);
    left_upper_leg();
   
    if(mode == GL_SELECT ) glLoadName(8);
    glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[8], 1.0, 0.0, 0.0);
    left_lower_leg();
   
    if(mode == GL_SELECT ) glLoadName(9);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(TORSO_RADIUS+UPPER_LEG_RADIUS, 0.1*UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[9], 1.0, 0.0, 0.0);
    right_upper_leg();
   
    if(mode == GL_SELECT ) glLoadName(10);
    glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
    glRotatef(theta[10], 1.0, 0.0, 0.0);
    right_lower_leg();
   
    glPopMatrix();



    glFlush();
    //glutSwapBuffers();
   
   
   
    /// To completely detach old person
    glPopMatrix();
    glLoadIdentity();
    //BALL
    glPushMatrix();
    glTranslatef(ball_x, ball_y, 0.0);
    drawball();
    glPopMatrix();


    glFlush();
    glutSwapBuffers();
   
   
}



void display(void)
{
   
    /* Accumulate ModelView Matrix as we traverse tree */
    if(recordMode == 1)
    {
        printf("Adding axis = %d && theta = %d to event buffer\n", axis, (int)theta[axis]);
        // Need to add all changeable variables to the event buffer.


        event_buffer[event_ptr++] = axis;
        event_buffer[event_ptr++] = theta[axis];
	//Capture sync events as well
	if(sync==1&&axis>2) {
		sync_buffer[event_ptr]=1;
		event_buffer[event_ptr++]=sync_axis;
		event_buffer[event_ptr++] = theta[sync_axis];
	}else sync_buffer[event_ptr]=0;
       
        event_buffer[event_ptr++] = x;
        event_buffer[event_ptr++] = y;
       
       	event_buffer[event_ptr++] = ball_x;
    	event_buffer[event_ptr++] = ball_y;
	
    }
    glClear(GL_COLOR_BUFFER_BIT);
    drawObjects(GL_RENDER);
    // glLoadIdentity();
    glFlush();
}


void processHits (GLint hits, GLuint buffer[])
{
   
    unsigned int i, j;
    GLuint names, *ptr;
   
    printf ("hits = %d\n", hits);
    ptr = (GLuint *) buffer;
    for (i = 0; i < hits; i++) {    /*  for each hit  */
        names = *ptr;
        ptr+=3;
        printf ("ptr = %d\n", *ptr);
        //printf ("names = %d\n", names);
       
        for (j = 0; j < names; j++) { /*  for each name */
           
            //1- torso, 2- head, 3- left upper arm, 4- left lower arm, 5- right upper arm, 6- right lower arm, 7- left upper leg, 8- left lower let, 9- right upper leg, 10- right lower leg
           
           
            if(*ptr == 1) {
                printf ("torso\n");
                //    bounce = !bounce;
                axis = 1;
            }
            else if (*ptr == 2)
            {
                printf ("head\n");
               
                axis = 2;
               
            }
            else if (*ptr == 3)
            {
                printf ("left upper arm\n");
               
                axis = 3;
               
            }
            else if (*ptr == 4)
            {
                printf ("left lower arm\n");
               
                axis = 4;
               
            }
            else if (*ptr == 5)
            {
                printf ("right upper arm\n");
               
                axis = 5;
               
            }
            else if (*ptr == 6)
            {
                printf ("right lower arm\n");
               
                axis = 6;
               
            }
            else if (*ptr == 7)
            {
                printf ("left upper leg\n");
               
                axis = 7;
               
            }
            else if (*ptr == 8)
            {
                printf ("left lower leg\n");
               
                axis = 8;
               
            }
            else if (*ptr == 9)
            {
                printf ("right upper leg\n");
               
                axis = 9;
               
            }
            else if (*ptr == 10)
            {
                printf ("right lower leg\n");
               
                axis = 10;
               
            }
           
            ptr++;
        }
    }
}

#define SIZE 512


void mouse(int btn, int state, int x2, int y2)
{
    int prevAxis;
   
    GLuint selectBuf[SIZE];
    GLint hits;
    GLint viewport[4];
    prevAxis=axis;
    if ((btn == GLUT_LEFT_BUTTON || btn == GLUT_RIGHT_BUTTON) && state == GLUT_DOWN)
    {
        glGetIntegerv (GL_VIEWPORT, viewport);
       
        glSelectBuffer (SIZE, selectBuf);
        glRenderMode(GL_SELECT);
       
        //glInitNames();
        //glPushName(0);
       
        glMatrixMode (GL_PROJECTION);
        glPushMatrix ();
        glLoadIdentity ();
        /*  create 5x5 pixel picking region near cursor location    */
        gluPickMatrix ((GLdouble) x2, (GLdouble) (viewport[3] - y2),
                       10.0, 10.0, viewport);
        glOrtho (-10.0, 10.0, -5.0, 15.0, -10, 10);
       
        //glOrtho(-1.0,1.0,-0.5,-1.5,-1,1);
        //glOrtho(0.0f, max_w, max_h, 0.0f, 0.0f, 1.0f);
       
       
        drawObjects(GL_SELECT);
       
        glMatrixMode (GL_PROJECTION);
        glPopMatrix ();
        glFlush ();
       
        hits = glRenderMode (GL_RENDER);
        processHits (hits, selectBuf);
       
        glutPostRedisplay();
    }
   
    //gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y),5.0, 5.0, viewport);
    //1- torso, 2- head, 3- left upper arm, 4- left lower arm, 5- right upper arm, 6- right lower arm, 7- left upper leg, 8- left lower let, 9- right upper leg, 10- right lower leg
    if (axis > -1 && prevAxis == axis && axis <=10) {
       

           
            if(sync==1&&axis==3) sync_axis=5;
            if(sync==1&&axis==4) sync_axis=6;
            if(sync==1&&axis==5) sync_axis=3;
            if(sync==1&&axis==6) sync_axis=4;
            if(sync==1&&axis==7) sync_axis=9;
            if(sync==1&&axis==8) sync_axis=10;
            if(sync==1&&axis==9) sync_axis=7;
            if(sync==1&&axis==10) sync_axis=8;


        if (btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            theta[axis] +=5.0;
            if(theta[axis] < 360.0) theta[axis] += 360.0;
           
            // Syncronization mappings
	    if (sync==1 && axis>2) {
		theta[sync_axis] +=5.0;
            	if(theta[sync_axis] < 360.0) theta[sync_axis] += 360.0;
           
	    }
           
        }
        if (btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            theta[axis] -=5.0;
            if(theta[axis] < 360.0) theta[axis] += 360.0;
           
            // Syncronization mappings

	    if (sync==1 && axis>2) {
		theta[sync_axis] -=5.0;
            	if(theta[sync_axis] < 360.0) theta[sync_axis] += 360.0;
           
	    }
           
        }
       
        display();
    }

   
}



void menu(int id)
{
   
    /* menu selects which angle to change or whether to quit */
   
    if(id == 1 ) axis=0;
    if(id == 2) axis=1;
    if(id == 3 ) axis=2;
    if(id ==4 ) exit(0);
}

void
myReshape(int w, int h)
{
    glViewport(0, -10, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-10.0, 10.0, -5.0 * (GLfloat) h / (GLfloat) w,
                15.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);
    else
        glOrtho(-10.0 * (GLfloat) w / (GLfloat) h,
                10.0 * (GLfloat) w / (GLfloat) h, -5.0, 15.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



void myinit()
{
    //glClearColor(1.0, 1.0, 1.0, 1.0);
    //glColor3f(1.0, 0.0, 0.0);
    //p=gluNewQuadric();
    //gluQuadricDrawStyle(p, GLU_LINE); /* render it as wireframe */
    /* allocate quadrics with filled drawing style */
   
    //light 1
    GLfloat light_ambient1[]={0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse1[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular1[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_position1[]={10.0, 10.0, 10.0, 0.0};
   
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
   
    //light 2
    GLfloat light_ambient2[]={0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse2[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular2[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_position2[]={10.0, 0.0, 10.0, 0.0};
   
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
   
    //light 3
    GLfloat light_ambient3[]={0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse3[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular3[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_position3[]={0.0, -10.0, 10.0, 0.0};
   
    glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse3);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular3);
   
    //light 4
    GLfloat light_ambient4[]={0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse4[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular4[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light_position4[]={-10.0, 10.0, 10.0, 0.0};
   
    glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
    glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient4);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse4);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular4);
   
   
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
   
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
   
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
   
    //glClearColor(0.75, 0.75, 0.75, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
   
    h=gluNewQuadric();
    gluQuadricDrawStyle(h, GLU_FILL);
    t=gluNewQuadric();
    gluQuadricDrawStyle(t, GLU_FILL);
    lua=gluNewQuadric();
    gluQuadricDrawStyle(lua, GLU_FILL);
    lla=gluNewQuadric();
    gluQuadricDrawStyle(lla, GLU_FILL);
    rua=gluNewQuadric();
    gluQuadricDrawStyle(rua, GLU_FILL);
    rla=gluNewQuadric();
    gluQuadricDrawStyle(rla, GLU_FILL);
    lul=gluNewQuadric();
    gluQuadricDrawStyle(lul, GLU_FILL);
    lll=gluNewQuadric();
    gluQuadricDrawStyle(lll, GLU_FILL);
    rul=gluNewQuadric();
    gluQuadricDrawStyle(rul, GLU_FILL);
    rll=gluNewQuadric();
    gluQuadricDrawStyle(rll, GLU_FILL);


    ball=gluNewQuadric();
    gluQuadricDrawStyle(ball, GLU_FILL);

   
}

//The timer function
void timerFunc(int val)
{
    // Check if playback_ptr has reached the last event
    printf("playback_ptr = %d\n", playback_ptr);
    if(playback_ptr<event_ptr) {
       
        // Need to account for all changeable variables to the event buffer.
        /*
        
         axis
         theta
        
         // Used to move Person 1
         float x
         float y
        
         // Used to move Person 2
         float x1
         float y1
        
         // Used to move ball
         float ball_x
         float ball_y
        
        
         */
       
        axis = event_buffer[playback_ptr++];
       
        theta[axis] = event_buffer[playback_ptr++];

	if(sync_buffer[playback_ptr] == 1)
	{
		//sync=1;
		sync_axis=event_buffer[playback_ptr++];
		theta[sync_axis]=event_buffer[playback_ptr++];
	}
	//else sync=0;
       
        x = event_buffer[playback_ptr++];
        y = event_buffer[playback_ptr++];

       	ball_x = event_buffer[playback_ptr++];
    	ball_y = event_buffer[playback_ptr++];
	
       
        // Update the screen with the current transformation retrieved from the event_buffer.
        display();
       
        // Call TimerFunc to read another event
        glutTimerFunc(50, timerFunc, 1);
    }
    else {
        playback_ptr=0;
    }
}

void reset()
{
    // Used to move Person 1
    x=0.0;
    y=0.0;
   
    static GLfloat newtheta[11] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        180.0,0.0,180.0,0.0}; /* initial joint angles */
    int i=0;
    for (i = 0; i < 11; i++)
    {
        theta[i] = newtheta[i];
    }
    axis = -1;
   
    display();
}

// Keyboard callback function


void moveKeyboard (unsigned char key, int xmouse, int ymouse)
{
    switch (key){
            //  Move Person
        case 'a':
            x-=1;
            break;
        case 'd':
            x+=1;
            break;
        case 'w':
            y+=1;
            break;
        case 's':
            y-=1;
            break;
	    //  Move ball
        case 'j':
            ball_x-=1;
            break;
        case 'l':
            ball_x+=1;
            break;
        case 'i':
            ball_y+=1;
            break;
        case 'k':
            ball_y-=1;
            break;
            /// RECORDING STUFF ///
           
        case 'z': //begin recording. Set recordMode
            recordMode = 1;
            playbackMode = 0;
            printf("Recording...  Press 'x' to end\n");
            event_ptr=0;
            break;
           
        case 'x': //stop recording. Reset recordMode.
            if(recordMode == 1) {
                recordMode = 0;
                printf("Recording stopped.  Press 'n' to save to playback file.\n");
            }
            break;
           
        case 'c': //Load file. Reset everything and load the contents of the file into the buffer.
            recordMode = 0;
            playbackMode = 0;
            event_ptr=0;
            playback_ptr=0;
            reset();
            printf("Loading file %s\n", fileName);
            jFile = fopen(fileName, "r");
            if ( jFile == NULL ) {
                printf("Warning: Could not open %s\n", fileName);
                playbackMode = 0;
            }
            else {
                //printf("%d", &event_buffer[event_ptr]);
               
                // Store the events to event_buffer
                while((fscanf(jFile, "%d ", &event_buffer[event_ptr])) != EOF) {
                    // printf("got here");
                    printf("event ptr = %d\n", event_ptr);
                    event_ptr++;
                }
                fclose(jFile);
                playbackMode = 1;
                printf("Successfully loaded %s\n", fileName);
            }
            break;
           
        case 'v': //Reset everything.
            recordMode = 0;
            playbackMode = 0;
            event_ptr=0;
            playback_ptr=0;
            reset();
            break;
           
        case 'b': //Playback
	    printf("in playback function");
            if(playbackMode==1) {
                printf("Starting playback");
                reset();
                glutTimerFunc(4,timerFunc,1);
            }
            break;
           
        case 'n': //Save file.
            recordMode = 0;
            playbackMode = 0;
           
            jFile = fopen(fileName, "w");
            if (jFile == NULL) {
                printf("Warning: Could not open %s\n", fileName);
            }
            else {
                int j = 0;
                printf("event_ptr = %d", event_ptr);
                for(j=0;j<event_ptr;j++){
                    printf("Event buffer = %d\n",event_buffer[j]);
                    fprintf(jFile, "%d ", event_buffer[j]);
                }
                fclose(jFile);
                printf("\nEvents saved in %s\n", fileName);
            }
            playback_ptr=0;
            break;
           
            /// start/stop syncronized movement
        case 'm':
            sync=sync*-1;
            break;
           
        default:
            break;
    }
    glutPostRedisplay();
}

void
main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(max_w, max_h);
    glutCreateWindow("sports");
    myinit();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(moveKeyboard);
    // glutSpecialKeys(processSpecialKeys);
    glutCreateMenu(menu);
    glutAddMenuEntry("torso", 0);
    glutAddMenuEntry("head1", 1);
    glutAddMenuEntry("head2", 2);
    glutAddMenuEntry("right_upper_arm", 3);
    glutAddMenuEntry("right_lower_arm", 4);
    glutAddMenuEntry("left_upper_arm", 5);
    glutAddMenuEntry("left_lower_arm", 6);
    glutAddMenuEntry("right_upper_leg", 7);
    glutAddMenuEntry("right_lower_leg", 8);
    glutAddMenuEntry("left_upper_leg", 9);
    glutAddMenuEntry("left_lower_leg", 10);
    glutAddMenuEntry("quit", 11);
   // glutAttachMenu(GLUT_LEFT_BUTTON);
   
   
    //Load our texture
    texture[3] = LoadTexture( "sky.bmp", 512, 256 );      //back
    texture[1] = LoadTexture( "Grass.bmp", 256, 256 );      //floor
    texture[2] = LoadTexture( "sky.bmp", 512, 256 );      //left
    texture[0] = LoadTexture( "sky.bmp", 512, 256 );      //right
   
    reset();
    glutMainLoop();
   
}
