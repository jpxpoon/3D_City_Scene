//header file

#ifndef _DRAW_H_
#define _DRAW_H_


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>

/* window struct */
struct glutWindow
{
    int width;
    int height;
    
    float v_angle;
    float near;
    float far;
} ;

/* Curve Class */
class curve
{
public:
    float x, y, z, w;
    curve() : x(0.0f),y(0.0f),z(0.0f),w(1.0f) { }
    curve(float x, float y, float z) : x(x),y(y),z(z),w(1.0f) { }
    curve(float x, float y, float z, float w) : x(x),y(y),z(z),w(w) { }
    
    float operator[](int i) const
    {
        switch(i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        } // switch
    } // overload []
    
    float & operator[](int i)
    {
        switch(i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        } // switch
    } // overload []
    
};


/* Draw Class */
class Draw
{
private:
    static void render();
    static void renderBuilding();
    static void update();
    static void reshape();
    static void keyboard(int key, int, int);
    static void keyboardUp(int key, int, int);
    static void mouse(int button, int state, int x, int y);
    static void mouseMotion(int x, int y);
    static void init();
    static void makeShaders();
    
    Draw(int argc, char **argv);
    
public:
    static Draw& set(int argc, char **argv)
    {
        static Draw gw(argc, argv);
        return gw;
    } // set
    
    static void run();
};

#endif