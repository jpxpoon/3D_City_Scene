//  Assignment_4


#ifndef _CAR_H_
#define _CAR_H_

#include <iostream>

/* Car Camera */
class Camera
{
public:
	int frame;
	float xc;
	float yc;
	float zc;
	float anc;
	int gira;
	int vel;
	float Iox;
	float Ioy;
	float Ioz;
	int cameratype;
} ;


/* Car */
class Car : public Camera
{
public:
	// Car
	float x;
	float y;
	float z;
	float any;
	float dany;
	float velocita;
	float vf;
	float dvf;
    
	// Ruote
	float xwheel[4];
	float ywheel[4];
	float zwheel[4];
	float anruote;
	float gira;
    
    Car();
};


#endif
