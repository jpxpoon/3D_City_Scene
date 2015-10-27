// Assignment 4

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "draw.h"
#include "car.h"
#include "MilkshapeModel.h"
#include "Model.h"
#include "bitmap/bitmap.h"
//#include "shader_utils.hpp"

#define PI 3.1415926

using namespace std;

int m_pos_attribute_location;
int m_normal_attribute_location;
unsigned int m_frame;
unsigned int m_program;
unsigned int m_vertexsh;
unsigned int m_fragmentsh;

float M[16];    // Matrix
int n_point = 20;
int n_strip = 30;
float strip = 10.0f;

const float degree = 3.141592654f / 180.0f; // The value of 1 degree in radians

/* Camera */
bool mouseDown = false;
float x_rot = 0.0f, y_rot = 0.0f, z_rot = 0.0f;
float camera_x = 50.0f, camera_y = 50.0f, camera_z = 500.0f;
float camXSpeed = 0.0f, camYSpeed = 0.0f, camZSpeed = 0.0f;
float modelx_rot = 0.0f, modely_rot = 0.0f;
GLfloat movementSpeedFactor = 25.0f;

/* Keyboard */
bool holdingForward     = false;
bool holdingBackward    = false;
bool holdingLeftStrafe  = false;
bool holdingRightStrafe = false;

/* Car */
Model *truck = NULL;
Model *Auto = NULL;
Model *Wheel = NULL;
Car car[5];
bool car1 = false, car2 = false, car3 = false;

static unsigned int m_texture[10];
glutWindow w;

/* Color */
float WHITE[] = {1, 1, 1};
float RED[] = {1, 0, 0};
float GREEN[] = {0, 1, 0};
float MAGENTA[] = {1, 0, 1};
float BLUE[] = {0, 0, 1};


void moveCamera()
{
    camera_x += camXSpeed;
    camera_y += camYSpeed;
    camera_z += camZSpeed;
}


float toRads(const float &theAngleInDegrees)
{
    return theAngleInDegrees * degree;
} // get function


void calculateCameraMovement()
{
    // Break up our movement into components along the X, Y and Z axis
    float camMovementXComponent = 0.0f;
    float camMovementYComponent = 0.0f;
    float camMovementZComponent = 0.0f;
    
    if (holdingForward == true)
    {
        // Control X-Axis movement
        float pitchFactor = cos(toRads(x_rot));
        camMovementXComponent += ( movementSpeedFactor * float(sin(toRads(y_rot))) ) * pitchFactor;
        
        // Control Y-Axis movement
        camMovementYComponent += movementSpeedFactor * float(sin(toRads(x_rot))) * -1.0f;
        
        // Control Z-Axis movement
        float yawFactor = float(cos(toRads(x_rot)));
        camMovementZComponent += ( movementSpeedFactor * float(cos(toRads(y_rot))) * -1.0f ) * yawFactor;
    }
    
    if (holdingBackward == true)
    {
        // Control X-Axis movement
        float pitchFactor = cos(toRads(x_rot));
        camMovementXComponent += ( movementSpeedFactor * float(sin(toRads(y_rot))) * -1.0f) * pitchFactor;
        
        // Control Y-Axis movement
        camMovementYComponent += movementSpeedFactor * float(sin(toRads(x_rot)));
        
        // Control Z-Axis movement
        float yawFactor = float(cos(toRads(x_rot)));
        camMovementZComponent += ( movementSpeedFactor * float(cos(toRads(y_rot))) ) * yawFactor;
    }
    
    if (holdingLeftStrafe == true)
    {
        // Calculate our Y-Axis rotation in radians once here because we use it twice
        float yRotRad = toRads(y_rot);
        
        camMovementXComponent += -movementSpeedFactor * float(cos(yRotRad));
        camMovementZComponent += -movementSpeedFactor * float(sin(yRotRad));
    }
    
    if (holdingRightStrafe == true)
    {
        // Calculate our Y-Axis rotation in radians once here because we use it twice
        float yRotRad = toRads(y_rot);
        
        camMovementXComponent += movementSpeedFactor * float(cos(yRotRad));
        camMovementZComponent += movementSpeedFactor * float(sin(yRotRad));
    }
    
    // After combining our movements for any & all keys pressed, assign them to our camera speed along the given axis
    camXSpeed = camMovementXComponent;
    camYSpeed = camMovementYComponent;
    camZSpeed = camMovementZComponent;
    
    // Cap the speeds to our movementSpeedFactor (otherwise going forward and strafing at an angle is twice as fast as just going forward!)
    // X Speed cap
    if (camXSpeed > movementSpeedFactor)
    {
        //cout << "high capping X speed to: " << movementSpeedFactor << endl;
        camXSpeed = movementSpeedFactor;
    }
    if (camXSpeed < -movementSpeedFactor)
    {
        //cout << "low capping X speed to: " << movementSpeedFactor << endl;
        camXSpeed = -movementSpeedFactor;
    }
    
    // Y Speed cap
    if (camYSpeed > movementSpeedFactor)
    {
        //cout << "low capping Y speed to: " << movementSpeedFactor << endl;
        camYSpeed = movementSpeedFactor;
    }
    if (camYSpeed < -movementSpeedFactor)
    {
        //cout << "high capping Y speed to: " << movementSpeedFactor << endl;
        camYSpeed = -movementSpeedFactor;
    }
    
    // Z Speed cap
    if (camZSpeed > movementSpeedFactor)
    {
        //cout << "high capping Z speed to: " << movementSpeedFactor << endl;
        camZSpeed = movementSpeedFactor;
    }
    if (camZSpeed < -movementSpeedFactor)
    {
        //cout << "low capping Z speed to: " << movementSpeedFactor << endl;
        camZSpeed = -movementSpeedFactor;
    }
} // camera movement


void carMove(int n, Model *m, Model *w, char type)
{
    // Car and Wheel angle
	car[n].any+=car[n].dany;
	car[n].dany=car[n].anruote/30;
	car[n].anruote/=1.01;
	if(fabs(car[n].anruote)<2)
	{
		car[n].gira=false;
		car[n].anruote=0;
	}
	
	// set car direction
    if(type == 'w')
    {
        car[n].x += car[n].velocita*cos((car[n].any+90)*3.141/180)/30;
        car[n].z += -car[n].velocita*sin((car[n].any+90)*3.141/180)/30;
    } // move forward
    else if(type == 's')
    {
        car[n].x += car[n].velocita*cos((car[n].any+90)*3.141/180)/30;
        car[n].z += car[n].velocita*sin((car[n].any+90)*3.141/180)/30;
    } // move back
    else if(type == 'a')
    {
        car[n].z += car[n].velocita*cos((car[n].any+90)*3.141/180)/30;
        car[n].x += -car[n].velocita*sin((car[n].any+90)*3.141/180)/30;
    } // move left
    else if(type == 'd')
    {
        car[n].z += car[n].velocita*cos((car[n].any+90)*3.141/180)/30;
        car[n].x += car[n].velocita*sin((car[n].any+90)*3.141/180)/30;
    } // move right
    
    
	// Clipping
	if( car[n].x < -600 )
		car[n].x = 600;
	if( car[n].x > 600 )
		car[n].x = -600;
	if( car[n].z <- 600 )
		car[n].z = 600;
	if( car[n].z > 600 )
		car[n].z = -600;
	
	// Draw The Car
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(car[n].x, car[n].y, car[n].z);
	glRotatef(car[n].any, 0, 1, 0);
    if(type == 's')
    {
        glPushMatrix();
            glRotatef(180, 0, 1, 0);
            m->draw();
        glPopMatrix();
    } // back
    else if(type == 'a')
    {
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            m->draw();
        glPopMatrix();
    } // left
    else if(type == 'd')
    {
        glPushMatrix();
            glRotatef(-90, 0, 1, 0);
            m->draw();
        glPopMatrix();
    } // right
    else
        m->draw();

    
    if(w)
    {
        // Draw the Wheels
        for(int t=0;t<4;t++)
        {
            glPushMatrix();
            if(type == 'w' || type == 's')
                glTranslatef(car[n].xwheel[t], car[n].ywheel[t]+(float)(rand()%10)/100, car[n].zwheel[t]);
            else
            {
                glTranslatef(car[n].zwheel[t], car[n].ywheel[t]+(float)(rand()%10)/100, car[n].xwheel[t]);
                glRotatef(90, 0, 1, 0);
            } // left or right
            
            if(t==0 || t==1)
                glRotatef(car[n].anruote,0,1,0);
            if(t==1 || t==3)
            {
                glRotatef(180,0,1,0);
                glRotatef(-car[n].gira,1,0,0);
            }
            if(t==0 || t==2)
                glRotatef(car[n].gira,1,0,0);
            
            w->draw();
            glPopMatrix();
            car[n].gira -= car[n].velocita/3;
        }
    } // if have wheel
	glPopMatrix();
    
    car[n].frame++;
} // car move


void renderModel()
{
    
    Model *m = NULL;
    m = new MilkshapeModel();
    
    //building
    m->loadModelData("texture/building.ms3d");
    m->reloadTextures();
    glPushMatrix();
    glTranslated(300, 0, -420);
        m->draw();
    glPopMatrix();
    
    //figure
    m->loadModelData("texture/dwarf/dwarf1.ms3d");
    m->reloadTextures();
    glPushMatrix();
        glTranslated(50, 10, -500);
        glRotatef(180, 0, 1, 0);
        glScalef(0.7f, 0.7f, 0.7f);
        m->draw();
    glPopMatrix();
    
    //figure
    m->loadModelData("texture/dwarf/dwarf2.ms3d");
    m->reloadTextures();
    glPushMatrix();
        glTranslated(120, 10, -500);
        glRotatef(180, 0, 1, 0);
        glScalef(0.7f, 0.7f, 0.7f);
        m->draw();
    glPopMatrix();
    
    //car model
    m->loadModelData("texture/f360/f360.ms3d");
    m->reloadTextures();
    glPushMatrix();
        glTranslated(220, 50, -340);
        glPushMatrix();
            glRotatef(modelx_rot, 0, 1, 0);
            glScalef(0.3f, 0.3f, 0.3f);
            m->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(150, 0, 0);
            glRotatef(-modelx_rot, 0, 1, 0);
            glScalef(0.3f, 0.3f, 0.3f);
            m->draw();
        glPopMatrix();
    glPopMatrix();
    
    //helicopter model
    m->loadModelData("texture/ch53/ch-53.ms3d");
    m->reloadTextures();
    glPushMatrix();
        glTranslated(290, 30, -420);
        glRotatef(modelx_rot, 1, 0, 1);
        glRotatef(modely_rot, 0, 1, 0);
        glScalef(5.0f, 5.0f, 5.0f);
        m->draw();
    glPopMatrix();
    
    //human model
    m->loadModelData("texture/figure/figure.ms3d");
    m->reloadTextures();
    for(int i = 0; i < 200; i+=50)
    {
        glPushMatrix();
            glRotatef(i/5, 0, 1, 0);
            glTranslated(-100+i, 0, -50-i);
            glScalef(2.0f, 2.0f, 2.0f);
            m->draw();
        glPopMatrix();
        glPushMatrix();
            glRotatef(-(i/5)-45, 0, 1, 0);
            glTranslated(200+i, 0, i);
            glScalef(2.0f, 2.0f, 2.0f);
            m->draw();
        glPopMatrix();
        glPushMatrix();
            glRotatef(-(i/5)+45, 0, 1, 0);
            glTranslated(150+i, 0, -i);
            glScalef(2.0f, 2.0f, 2.0f);
            m->draw();
        glPopMatrix();
    } // draw each figure
    
    
    /* Tree */
    Model *tree = NULL;
    tree = new MilkshapeModel();
    
    tree->loadModelData("texture/tree.ms3d");
    tree->reloadTextures();

    /* Horizontal */
    for(int x = -550; x <= -340; x += 90)
    {
        glPushMatrix();
            glTranslated(x, 0, 30);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, -5);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, 30-300);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, -5-300);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
    } // draw horizotal tree
    
    for(int x = -280; x <= 70; x += 90)
    {
        glPushMatrix();
            glTranslated(x, 0, 30);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, -5);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, 30-300);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
            glPushMatrix();
            glTranslated(x, 0, -5-300);
            glScaled(2, 2, 2);
        tree->draw();
        glPopMatrix();
    } // draw horizotal tree
    
    for(int x = 120; x <= 550; x += 90)
    {
        glPushMatrix();
            glTranslated(x, 0, 30);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, -5);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, 30-300);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(x, 0, -5-300);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
    } // draw horizotal tree
    
    /* Vertical */
    for(int z = -550; z <= -300; z += 90)
    {
        glPushMatrix();
            glTranslated(70, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(70-400, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110-400, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
    } // draw vertical tree
    
    for(int z = -350; z <= -45; z += 90)
    {
        glPushMatrix();
            glTranslated(70, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(70-400, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110-400, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
    } // draw vertical tree
    
    for(int z = -40; z <= 550; z += 90)
    {
        glPushMatrix();
            glTranslated(70, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(70-400, 0, z);
            glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
        glPushMatrix();
            glTranslated(110-400, 0, z);
                glScaled(2, 2, 2);
            tree->draw();
        glPopMatrix();
    } // draw vertical tree
    /* Tree */
    
    /* Car */
    glTranslated(94, 0, 0);
    
    truck = new MilkshapeModel();
    Auto = new MilkshapeModel();
    Wheel = new MilkshapeModel();									// Memory To Hold The Model
    truck->loadModelData("texture/ninja/ninja.ms3d");
	Auto->loadModelData( "texture/auto.ms3d" );
    Wheel->loadModelData( "texture/ruota.ms3d" );
    truck->reloadTextures();
    Auto->reloadTextures();
    Wheel->reloadTextures();
	//Auto->draw();
    //Wheel->draw();
    
    glPushMatrix();
        carMove(0, Auto, Wheel, 'w');
    glPopMatrix();
    glPushMatrix();
        glTranslated(-13, 0, 0);
        carMove(1, Auto, Wheel, 's');
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -295);
        carMove(2, Auto, Wheel, 'd');
    glPopMatrix();
    glPushMatrix();
        glTranslated(-13, 0, -282);
        carMove(3, Auto, Wheel, 'a');
    glPopMatrix();
    
} // draw car


void Draw::renderBuilding()
{
    /* Base */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, m_texture[2]);
    glTranslated(-200, 0, -200);
    //glScalef(10.0f, 10.0f, 10.f);
    //bottom
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 0, 50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(50, 0, 50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(50, 0, -50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-50, 0, -50);
    glEnd();
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 15, -50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(50, 15, 50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-50, 15, 50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-50, 15, -50);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 0, 50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(-50, 15, 50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-50, 15, -50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-50, 0, -50);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, 50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(50, 15, 50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(50, 15, -50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(50, 0, -50);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, 50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(50, 15, 50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-50, 15, 50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-50, 0, 50);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, -50);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(50, 15, -50);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-50, 15, -50);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-50, 0, -50);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    /* Top */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, m_texture[3]);
    glTranslated(-220, 15, -200);
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-25, 70, 25);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(15, 70, 25);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(15, 70, -25);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-25, 70, -25);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-25, 0, 25);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(-25, 70, 25);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-25, 70, -25);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-25, 0, -25);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(15, 0, 25);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(15, 70, 25);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(15, 70, -25);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(15, 0, -25);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(15, 0, 25);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(15, 70, 25);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-25, 70, 25);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-25, 0, 25);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(15, 0, -25);
        glTexCoord2d( 0.0, 2.0 );   glVertex3f(15, 70, -25);
        glTexCoord2d( 2.0, 2.0 );   glVertex3f(-25, 70, -25);
        glTexCoord2d( 2.0, 0.0 );   glVertex3f(-25, 0, -25);
    glEnd();
    
    //next
    glTranslated(40, -3, 0);
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-25, 150, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(25, 150, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(25, 150, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-25, 150, -25);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-25, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-25, 150, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-25, 150, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-25, 0, -25);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(25, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(25, 150, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(25, 150, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(25, 0, -25);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(25, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(25, 150, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-25, 150, 25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-25, 0, 25);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(25, 0, -25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(25, 150, -25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-25, 150, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-25, 0, -25);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //next 4 building
    glBindTexture(GL_TEXTURE_2D, m_texture[7]);
    glTranslated(475, -10, -290);
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 200, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(50, 200, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(50, 200, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 200, -25);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-50, 200, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 200, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -25);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(50, 200, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(50, 200, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(50, 0, -25);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, 25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(50, 200, 25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 200, 25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, 25);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(50, 0, -25);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(50, 200, -25);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 200, -25);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -25);
    glEnd();
    
    //next 2
    glTranslated(-45, 0, 60);
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 150, -20);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -20);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-10, 0, -20);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, 50);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, -20);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -20);
    glEnd();
    
    //next 3
    glTranslated(145, 0, 0);
    //top
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 150, -20);
    glEnd();
    //left
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-50, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -20);
    glEnd();
    //right
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-10, 0, -20);
    glEnd();
    //front
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, 50);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, 50);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, 50);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, 50);
    glEnd();
    //back
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3f(-10, 0, -20);
        glTexCoord2d( 0.0, 4.0 );   glVertex3f(-10, 150, -20);
        glTexCoord2d( 4.0, 4.0 );   glVertex3f(-50, 150, -20);
        glTexCoord2d( 4.0, 0.0 );   glVertex3f(-50, 0, -20);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    glPopMatrix();

} // draw buidling


void renderRoads(float start, float end, char type)
{
    if(type == 'v')
    {
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(0.0f, 0.1f, start);
            glTexCoord2d( 0.0, 1.0 );   glVertex3d(25.0f, 0.1f, start);
            glTexCoord2d( 1.0, 1.0 );   glVertex3d(25.0f, 0.1f, end);
            glTexCoord2d( 1.0, 0.0 );   glVertex3d(0.0f, 0.1f, end);
        glEnd();
    }
    else if (type == 'h')
    {
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(start, 0.1f, 25.0f);
            glTexCoord2d( 0.0, 1.0 );   glVertex3d(start, 0.1f, 0.0f);
            glTexCoord2d( 1.0, 1.0 );   glVertex3d(end, 0.1f, 0.0f);
            glTexCoord2d( 1.0, 0.0 );   glVertex3d(end, 0.1f, 25.0f);
        glEnd();
    }
} // draw roads


void Draw::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    calculateCameraMovement();
    moveCamera();
    if(car1)
    {
        glRotatef(car[0].any, 0, 1, 0);
        glTranslated(-car[0].x-43, -car[0].y+40, -car[0].z+450);
    } // car1 view
    if(car3)
    {
        glRotatef(car[2].any+90, 0, 1, 0);
        glTranslated(-car[2].x+10, -car[2].y+40, -car[2].z+796);
    } // car3 view
    
    
    glRotatef(x_rot, 1.0f, 0.0f, 0.0f);
    glRotatef(y_rot, 0.0f, 1.0f, 0.0f);
    glTranslatef(-camera_x, -camera_y, -camera_z);
    
    //Light
    glEnable(GL_LIGHTING);
    GLfloat lightPosition[] = {600, 40, 70, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);

    /* Draw Ground */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, m_texture[0]);
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3d(-600.0f, 0.0f, -600.0f);
        glTexCoord2d( 0.0, 30.0 );   glVertex3d(-600.0f, 0.0f, 600.0f);
        glTexCoord2d( 30.0, 30.0 );   glVertex3d(600.0f, 0.0f, 600.0f);
        glTexCoord2d( 30.0, 0.0 );   glVertex3d(600.0f, 0.0f, -600.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // draw road
    glBindTexture(GL_TEXTURE_2D, m_texture[4]);
    // horizontal
    glPushMatrix();
        renderRoads(-600.0f, -325.0f, 'h');
        renderRoads(-300.0f, 75.0f, 'h');
        renderRoads(100.0f, 600.0f, 'h');
        glTranslated(0, 0, -300);
        renderRoads(-600.0f, -325.0f, 'h');
        renderRoads(-300.0f, 75.0f, 'h');
        renderRoads(100.0f, 600.0f, 'h');
    glPopMatrix();
    // vertical
    glPushMatrix();
        glTranslated(75, 0, 0);
        renderRoads(25.0f, 600.0f, 'v');
        renderRoads(0.0f, -275.0f, 'v');
        renderRoads(-300.0f, -600.0f, 'v');
        glTranslated(-400, 0, 0);
        renderRoads(25.0f, 600.0f, 'v');
        renderRoads(0.0f, -275.0f, 'v');
        renderRoads(-300.0f, -600.0f, 'v');
    glPopMatrix();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    /* Sky */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, m_texture[1]);
    glBegin(GL_QUADS);
    //glColor3f(0.0f, 0.0f, 1.0f);
        glTexCoord2d( 0.0, 0.0 );   glVertex3d(-600.0f, 0.0f, -600.0f);
        glTexCoord2d( 0.0, 10.0 );   glVertex3d(-600.0f, 600.0f, -600.0f);
        glTexCoord2d( 10.0, 10.0 );   glVertex3d(600.0f, 600.0f, -600.0f);
        glTexCoord2d( 10.0, 0.0 );   glVertex3d(600.0f, 0.0f, -600.0f);
        glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    renderBuilding();
    
    /* draw pool */
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, m_texture[5]);
        glTranslated(0, 0.1, 300);
        //bottom
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(-200.0f, 5.0f, -150.0f);
            glTexCoord2d( 0.0, 3.0 );   glVertex3d(-200.0f, 5.0f, 0.0f);
            glTexCoord2d( 3.0, 3.0 );   glVertex3d(0.0f, 5.0f, 0.0f);
            glTexCoord2d( 3.0, 0.0 );   glVertex3d(0.0f, 5.0f, -150.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, m_texture[6]);
        //left
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(-200.0f, 10.0f, -150.0f);
            glTexCoord2d( 0.0, 3.0 );   glVertex3d(-200.0f, 10.0f, 0.0f);
            glTexCoord2d( 3.0, 3.0 );   glVertex3d(-200.0f, 0.0f, 0.0f);
            glTexCoord2d( 3.0, 0.0 );   glVertex3d(-200.0f, 0.0f, -150.0f);
        glEnd();
        //right
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(0.0f, 10.0f, -150.0f);
            glTexCoord2d( 0.0, 3.0 );   glVertex3d(0.0f, 10.0f, 0.0f);
            glTexCoord2d( 3.0, 3.0 );   glVertex3d(0.0f, 0.0f, 0.0f);
            glTexCoord2d( 3.0, 0.0 );   glVertex3d(0.0f, 0.0f, -150.0f);
        glEnd();
        //front
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(-0.0f, 0.0f, 0.0f);
            glTexCoord2d( 0.0, 3.0 );   glVertex3d(-200.0f, 0.0f, 0.0f);
            glTexCoord2d( 3.0, 3.0 );   glVertex3d(-200.0f, 10.0f, 0.0f);
            glTexCoord2d( 3.0, 0.0 );   glVertex3d(0.0f, 10.0f, 0.0f);
        glEnd();
        //back
        glBegin(GL_QUADS);
            glTexCoord2d( 0.0, 0.0 );   glVertex3d(0.0f, 0.0f, -150.0f);
            glTexCoord2d( 0.0, 3.0 );   glVertex3d(-200.0f, 0.0f, -150.0f);
            glTexCoord2d( 3.0, 3.0 );   glVertex3d(-200.0f, 10.0f, -150.0f);
            glTexCoord2d( 3.0, 0.0 );   glVertex3d(0.0f, 10.0f, -150.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    /* draw grass */
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, m_texture[8]);
    glTranslated(350, -4.9, -70);
    glBegin(GL_QUADS);
        glTexCoord2d( 0.0, 0.0 );   glVertex3d(-200.0f, 5.0f, -150.0f);
        glTexCoord2d( 0.0, 3.0 );   glVertex3d(-200.0f, 5.0f, 0.0f);
        glTexCoord2d( 3.0, 3.0 );   glVertex3d(0.0f, 5.0f, 0.0f);
        glTexCoord2d( 3.0, 0.0 );   glVertex3d(0.0f, 5.0f, -150.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
    /* draw object */
    //tespot
    glPushMatrix();
        glTranslated(170, 0, 150);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, RED);
        glutSolidCube(20);
        glTranslated(0, 25, 0);
        glRotatef(modelx_rot/2, 1.0f, 0.0f, 1.0f);
        glRotatef(modely_rot/2, 0.0f, 1.0f, 1.0f);
        glutSolidTeapot(10);
    glPopMatrix();
    //torus
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, GREEN);
        glTranslated(-75, 0, -180);
        glutSolidCube(20);
        glTranslated(0, 25, 0);
        glRotatef(modelx_rot/2, 1.0f, 0.0f, 1.0f);
        glRotatef(modely_rot/2, 0.0f, 1.0f, 0.0f);
        glutSolidTorus(3, 10, 30, 30);
    glPopMatrix();
    //cone
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, MAGENTA);
        glTranslated(300, 0, -200);
        glutSolidCube(20);
        glTranslated(0, 30, 0);
        glRotated(90, 1, 0, 0);
        glRotatef(modelx_rot/2, 0, 0, 1);
        glutSolidCone(10, 15, 30, 30);
    glPopMatrix();
    //base
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, BLUE);
        glTranslated(50, 0, -500);
        glutSolidCube(20);
        glTranslated(70, 0, 0);
        glutSolidCube(20);
    glPopMatrix();
    
    glPushMatrix();
        renderModel();
    glPopMatrix();

    glDisable(GL_LIGHTING);

    glFlush();

    glutSwapBuffers();
} // render


void Draw::keyboard(int key, int, int)
{
    switch(key)
    {
        case 'q':
            exit(0);                                    // exit
        case 27:
            exit(0);
        case GLUT_KEY_DOWN:
            holdingBackward = true;
            break;
        case GLUT_KEY_UP:
            holdingForward = true;
            break;
        case GLUT_KEY_LEFT:
            holdingLeftStrafe = true;
            break;
        case GLUT_KEY_RIGHT:
            holdingRightStrafe = true;
            break;
        case '+':
            camera_y += 10.0f;
            break;
        case '-':
            camera_y -= 10.0f;
            break;
        case '1':
            camera_x = 50.0f, camera_y = 50.0f, camera_z = 500.0f;
            camXSpeed = 0.0f, camYSpeed = 0.0f, camZSpeed = 0.0f;
            x_rot = 0.0f, y_rot = 0.0f;
            if(car1)
                car1 = false;
            else
                car1 = true;
            break;
        case '2':
            camera_x = 50.0f, camera_y = 50.0f, camera_z = 500.0f;
            camXSpeed = 0.0f, camYSpeed = 0.0f, camZSpeed = 0.0f;
            x_rot = 0.0f, y_rot = 0.0f;
            if(car3)
                car3 = false;
            else
                car3 = true;
        default:
            break;
    } // switch key
} // keyboard


void Draw::keyboardUp(int key, int x, int y)
{
    
    switch (key)
    {
        case GLUT_KEY_DOWN:
            holdingBackward = false;
            break;
        case GLUT_KEY_UP:
            holdingForward = false;
            break;
        case GLUT_KEY_LEFT:
            holdingLeftStrafe = false;
            break;
        case GLUT_KEY_RIGHT:
            holdingRightStrafe = false;
        default:
            break;
    }
}


void Draw::mouse(int button, int state, int x, int y)
{    
    if(button == GLUT_LEFT_BUTTON)
        mouseDown = true;
    else
        mouseDown = false;
} // mouse function


void Draw::mouseMotion(int x, int y)
{
    if(mouseDown)
    {
        GLfloat vertMouseSensitivity  = 50.0f;
        GLfloat horizMouseSensitivity = 50.0f;
                
        int horizMovement = x - w.width/2;
        int vertMovement  = y - w.height/2;
        
        x_rot += vertMovement / vertMouseSensitivity;
        y_rot += horizMovement / horizMouseSensitivity;
        
        // Limit loking up to vertically up
        if (x_rot < -90.0f)
            x_rot = -90.0f;
        
        // Limit looking down to vertically down
        if (x_rot > 90.0f)
            x_rot = 90.0f;
        
        // Looking left and right.
        if (y_rot < -180.0f)
            y_rot += 360.0f;
        
        if (y_rot > 180.0f)
            y_rot -= 360.0f;
    }
} // mouse motion


void Draw::reshape()
{
    glutPostRedisplay();
} // reshape


void Draw::update()
{
    if(modelx_rot < 360 && modely_rot < 360)
    {
        modelx_rot += 5.0;
        modely_rot += 5.0;
    }
    else
    {
        modelx_rot = -360.0f;
        modely_rot = -360.0f;
    }
    glutPostRedisplay();
} // update


void Draw::run()
{
    glutMainLoop();
} // start


Draw::Draw(int argc, char ** argv)
{
    w.width = 1024;
    w.height = 768;
    w.v_angle = 45;
    w.near = 1.0f;
    w.far = 3000.0f;
    
    glutInitWindowSize(w.width, w.height);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignemnt 4 - City Scene");
    
    glutDisplayFunc(render);        // call render function
    glutIdleFunc(update);           // call ideal function
    //glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);   // call keyboard function
    glutSpecialUpFunc(keyboardUp);
    glutMouseFunc(mouse);           // call mouse function
    glutMotionFunc(mouseMotion);    // call mouse moition function
    
    init();
} // constructor


void loadTexture(int n, const char* filename)
{
    CBitmap image;
    image.Load(filename);               //read bitmap image
    
    glGenTextures(1, &m_texture[n]);               //allocate 1 texture
    glBindTexture(GL_TEXTURE_2D, m_texture[n]);    //bind this texture to be active
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetBits());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //specify minificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //specify magnificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    //specify texture coordinate treatment
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);    //specify texture coordinate treatment
    
    glEnable(GL_TEXTURE_2D);    //enable texturing
} // load texture


void Draw::init()
{
    glViewport(0, 0, w.width, w.height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float) w.width / w.height;
    gluPerspective(w.v_angle/2.0f, aspect, w.near, w.far);
    
    glEnable(GL_DEPTH_TEST);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
    glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // background color
    glClearDepth(1.0f);     // depth buffer
    
    //makeShaders();
    loadTexture(0, "texture/floor/images-2.bmp");
    loadTexture(1, "texture/sky/bluesky.bmp");
    loadTexture(2, "texture/building/rockwall.bmp");
    loadTexture(3, "texture/building/images.bmp");
    loadTexture(4, "texture/floor/road.bmp");
    loadTexture(5, "texture/floor/water2.bmp");
    loadTexture(6, "texture/building/psioldwood.bmp");
    loadTexture(7, "texture/building/building.bmp");
    loadTexture(8, "texture/floor/grass.bmp");
    
} // initializes

