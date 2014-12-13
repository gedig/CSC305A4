#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtGlobal>
#include <QtGui>
#include <QtOpenGL>
#include "pointlight.h"
#include "sphere.h"

#define VIEW_DISTANCE 7000


//This is our OpenGL Component we built it on top of QGLWidget
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    //Constructor for GLWidget
    GLWidget(QWidget *parent = 0);

    //Destructor for GLWidget
    ~GLWidget();

    void openImage(QString fileBuf);
    void openScene(QString fileBuf);
    void saveImage( QString fileBuf);
    void makeImage();

protected:
    //Initialize the OpenGL Graphics Engine
    void initializeGL();

    //All our painting stuff are here
    void paintGL();

    //When user resizes main window, the scrollArea will be resized and it will call this function from
    //its attached GLWidget
    void resizeGL(int w, int h);


private:
    void clear();
    void displayImage();
    void prepareImageDisplay(QImage* myimage); // converts from Qt to opengl format
    int intersectSpheres(QVector3D initialPosition, QVector3D direction, QVector3D& intersectionPoint);
    QColor rayColor(QVector3D rayOrigin, QVector3D rayDirection, int timesCalled);

    int renderWidth, renderHeight;
    QProgressBar* pbar;
    QImage glimage, qtimage;  // paintGL will display the gl formatted image
    // keep the qtimage around for saving (one is a copy of the other)

    // Scene Attributes
    float cameraPos;
    int maxRayRecursion;
    int antialiasing;
    QColor ambientColour;
    float tileSize;
    Material floorMainMaterial;
    Material floorSecondaryMaterial;

    // Lists of Scene Objects
    QVector<PointLight> pointLights;
    QVector<Sphere> spheres;
    QHash<QString, Material> materials;
};


#endif
