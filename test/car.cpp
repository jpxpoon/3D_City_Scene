//  Assignment_4

#include <math.h>
#include <cstdlib>
#include "car.h"

#define DIMX 64
#define DIMZ 64
#define SIZE 32

Car::Car()
{
    // Car and Wheel Position
	x=0;
	y=0.75;
	z=0;
	any=0;
	dany=0;
	velocita = 100;	// Initial speed
	vf=100;
	dvf=10;
    
	xwheel[0]=2.4;
	ywheel[0]=0.2;
	zwheel[0]=-5.2;
	xwheel[1]=-2.4;
	ywheel[1]=0.2;
	zwheel[1]=-5.2;
	xwheel[2]=2.4;
	ywheel[2]=0.2;
	zwheel[2]=4.3;
	xwheel[3]=-2.4;
	ywheel[3]=0.2;
	zwheel[3]=4.3;
	anruote=0;	// Wheel Angle
	gira=0;		// Angle for car turning
    
    // Camera
    frame=0;
	anc=-20;
	gira=false;	// True when the car is steering
	vel=0;	// >0 if the speed car change
	cameratype=0;
    
    // Initial Camera Position
	Iox=x+50*cos((any+(rand()%90-rand()%90))*3.141/180);
	Ioy=(float)(1+rand()%10);
	Ioz=z+50*sin((any+(rand()%90-rand()%90))*3.141/180);
} // car constructor


