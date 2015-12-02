#include <iostream>
#include <string>

#include "gbufferedimage.h"
#include "console.h"
#include "point.h"
#include "queue.h"
using namespace std;

GBufferedImage* image;
const int BACKGROUND_COLOR = 0xdfdfdf;
const int WHITE_COLOR = 0xffffff;


/**
  *This function takes a point of graphic object
  * as parameter and adds all neighboring points,
  * which are not the background, in a vector of points of the object
  * ---------------------
 * @brief findAllPixelObject
 * @param x
 * @param y
 * @return
 */
Vector <Point> findAllPixelObject (int x, int y){
    Vector <Point> result;
    Queue <Point> points;
    Point point(x,y);
    points.enqueue(point);

    while(!points.isEmpty()){
        Point newPoint = points.dequeue();
        result.add(newPoint);
        for(int x = newPoint.getX() - 1; x <= newPoint.getX() + 1; x++){
            for(int y = newPoint.getY()- 1; y <= newPoint.getY() + 1; y++){

                //if the neighboring pixel is outside the object
                if ((x < 0 || x > image->getWidth()) || (y < 0 || y > image->getHeight())) {
                    continue;
                }

                //if the neighboring pixel belongs to the object
                if (image->getRGB(x,y) < BACKGROUND_COLOR){
                    Point point(x,y);
                    points.enqueue(point);
                    image->setRGB(point.getX(),point.getY(),WHITE_COLOR);
                }
            }
        }
    }

    return result;
}


/**
 * The search function of objects in the image.
 * Scans the image line by line. If the found point belongs to the object,
 * searches all the points belonging to the same object using the method findAllPixelObject.
 * -------------------
 * @brief findObjects
 * @return
 */
int findObjects(){

    Vector <Vector<Point>> setObjects;  //vector of graphical objects

    for (int y = 0; y < image->getHeight(); y++){
        for (int x = 0; x < image->getWidth(); x++){
            if (image->getRGB(x,y) < BACKGROUND_COLOR){  //if the pixel is not very white, so it belongs to the object
                image->setRGB(x,y,WHITE_COLOR);  //mark pixel as posted
                Vector <Point> object = findAllPixelObject(x,y); //creat the vector of points of a graphic object
                setObjects.add(object);
            }
        }
    }
    int col = setObjects.size();
    return col;
}


int main() {
    GWindow window;
    image = new GBufferedImage(1000,1000,0);
    image->load("1.jpg");

    window.setCanvasSize(image->getWidth(),image->getHeight());
    window.add(image);
    int res = findObjects();
    cout << "The number of objects in the picture : " << res << endl;
    return 0;
}
