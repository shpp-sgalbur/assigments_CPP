

#include <iostream>
#include "console.h"
#include <stdlib.h>
#include "gbufferedimage.h"
#include <math.h>
#include <complex>

using namespace std;

/* -------- The Complex class -----------
 * ---------------------------------------
 * Class representing a complex number
 */
class Complex
{
public:
    double re;//the real part of a complex number
    double im;//imaginary part of a complex number

    /**the class constructor
     */
    Complex(double are, double aim) {
        this->re = are, this->im = aim;
    }

    /**the copy constructor
     */
    Complex (const Complex &c)
     {
       re = c.re;
       im = c.im;
     }

    // destructor
    ~Complex ()
    {
    }

    /**overloading operator +
     */
    Complex operator+(const Complex &v)
    {
        return Complex(re + v.re, im + v.im);
    }

    /**squaring a complex number
     */
    Complex complSqr(const Complex &v)
    {
        return Complex(v.re * v.re - v.im * v.im, 2 * v.re * v.im);
    }

    /**The absolute value (or modulus or magnitude) of a complex number
     */
    double complRadius(const Complex &v)
    {
        return sqrt(v.re * v.re + v.im * v.im);
    }

};
/*-------- End complex class ----------------*/




/* ---------- Declarations constants ---------------*/

//the size of the graphics window (px)
double const GW_WIDTH = 1000;
double const GW_HEIGHT = 600;

// Representing radius value "2" on the complex area
double const RADIUS = GW_WIDTH * 0.3;

//The coordinates of the center of the complex plane
double const OX = 3 * GW_WIDTH / 4;
double const OY = GW_HEIGHT / 2;

// Limit for iterations quantity
int const LIMIT_ITERATION = 100;

/*---------------------------------------------------*/

/**Converts coordinates of the pixels  to  the complex number
*/
Complex  convertToComplex(double x, double y){
       x = (x - OX)/RADIUS ;
       y = (y - OY)/RADIUS;
       return Complex(x, y);
    }

/*Check whether the complex number in the Mandelbrot set
*/
bool isMandelbrot( Complex c){
    Complex zz = Complex(0,0);
    for(int i = 0;i < LIMIT_ITERATION; i++){
        zz = zz + c;
        zz = zz.complSqr(zz);
        if (zz.complRadius(zz) > RADIUS) return false;
    }
    return true;
}

int main() {

    //create the graphic window
    GWindow gw;
    gw.setSize(GW_WIDTH, GW_HEIGHT);

    //add object to the graphics window
    GBufferedImage* img;
    img = new GBufferedImage(GW_WIDTH, GW_HEIGHT,0xffffff);
    gw.add(img, 0, 0);

    img->setVisible(false);

    /* paint the pixel if it belongs to the Mandelbrot set
    */
    for(int row = 0; row < GW_HEIGHT; row++){
        for(int col = 0; col < GW_WIDTH; col++){            
            Complex c = convertToComplex(col, row);//transform the pixel coordinates of a complex number
            if (isMandelbrot(c)){
                img->setRGB(col, row, 0);//paint the pixel
            }
        }
    }

    img->setVisible(true);

   return 0;
}


