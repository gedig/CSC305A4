#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtGlobal>
#include <QtGui>
#include <QtOpenGL>
#include "pointlight.h"
#include "sphere.h"


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
    int renderWidth, renderHeight;
    void displayImage();
    QProgressBar* pbar;
    void prepareImageDisplay(QImage* myimage); // converts from Qt to opengl format
    QImage glimage, qtimage;  // paintGL will display the gl formatted image
    // keep the qtimage around for saving (one is a copy of the other)

    float cameraPos;
    QVector<PointLight> pointLights;
    QVector<Sphere> spheres;
};


#endif
