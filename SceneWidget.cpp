//=============================================================================
// MyGLWidget.cpp
//
// Usage : Mouse wheel to zoom in and out.
//         Press left mouse button and then the mouse, you can drag the map freely.
//
//         'ESC' key to exit the program, 'R' key to reset the scene
//=============================================================================

#include "SceneWidget.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <fstream>
#include <sstream>

//=============================================================================

SceneWidget::SceneWidget(QGLWidget *parent)
    : QGLWidget(parent)
{
    /* Default values */
    scaling = 1.0;
    uiCurrentPos = 0;
    transform[0] = transform[1] = 0.0;

    /* Set window position and size */
    setGeometry(260, 100, 920,600);
}

SceneWidget::~SceneWidget()
{

}

void SceneWidget::initializeGL()
{
    /* Background color */
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* OpenGL blender function */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* Disable blend by defualt */
    glDisable(GL_BLEND);

    /* Load map and vehicle texture from images */
    // Only support the image that can be loaded by QImage (bmp, jpg, png recomended)
    textureMap = loadTextures("../res/map.png");
    textureVehicle = loadTextures("../res/vehicle.png");

    /* Enable texture*/
    glEnable(GL_TEXTURE_2D);

    //====================================================================
    /* Read path of the vehicle from txt file*/
    std::string str;
    qreal pointx, pointy;
    std::ifstream fin("../res/path.txt");
    while(std::getline(fin, str)){
        std::stringstream sin(str);
        sin>>pointx>>pointy;
        /* Save read data */
        vehiclePaths.push_back(QPointF(pointx, pointy));
    }

    //====================================================================

    /* Repaint every 100 milliseconds */
    startTimer(100);
}

void SceneWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    /* Ortho projection matrix */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    /* Model view matrix */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SceneWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Translation and rotation */
    glTranslatef(transform[0], transform[1], 0.0);
    glScalef(scaling, scaling, 1.0);

    /////////////////////////////////////////////////////////////////
    /* Draw map with texture*/
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f), glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();

    //=========================================================================
    /* Draw vehicle identify icon */
    glPushMatrix();
    // Translate the vehicle
    glTranslatef(vehiclePaths[uiCurrentPos].x(), vehiclePaths[uiCurrentPos].y(), 0.0f);
    glScalef(0.4f, 0.8f, 1.0f);
    glEnable(GL_BLEND);

    /* Flicker drawing the vehicle */
    static int iDrawIndex = 0;
    glColor4f(1.0f, 1.0f, 1.0f, (iDrawIndex++ == 10)?(iDrawIndex = 0, 1.0f):(iDrawIndex / 10.0f ));

    glBindTexture(GL_TEXTURE_2D, textureVehicle);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f, -0.1f, 0.0f);
    glTexCoord2f(1.0f, 0.0f), glVertex3f( 0.1f, -0.1f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.1f,  0.1f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.1f,  0.1f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

void SceneWidget::timerEvent(QTimerEvent *)
{
    // Change the vehicle's position
    if(uiCurrentPos < vehiclePaths.size()-1)
        uiCurrentPos++;

    /* Repaint OpenGL scene through calling paintGL */
    updateGL();
}
#include <QDebug>
void SceneWidget::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        scaling += 0.02;
    else if(e->delta() < 0)
        scaling -= 0.02;
}

void SceneWidget::mousePressEvent(QMouseEvent *e)
{
    /* Records the position of the mouse pressed location */
    mousePos[0] = e->pos().x();
    mousePos[1] = e->pos().y();
}

void SceneWidget::mouseReleaseEvent(QMouseEvent *)
{
    /* Reset mouse cursor */
    setCursor(Qt::ArrowCursor);
}

void SceneWidget::mouseMoveEvent(QMouseEvent *e)
{
    static GLdouble sFactor = 0.001;

    if(e->buttons() & Qt::LeftButton){

        /* Mouse cursor */
        setCursor(Qt::OpenHandCursor);

        /* Translate the Quards in the scene */
        transform[0] += (e->pos().x() - mousePos[0]) * sFactor;
        transform[1] -= (e->pos().y() - mousePos[1]) * sFactor;

        /* Update mouse position */
        mousePos[0] = e->pos().x();
        mousePos[1] = e->pos().y();
    }
}

void SceneWidget::keyPressEvent(QKeyEvent *e)
{
    /* ESC to exit the program */
    if(e->key() == Qt::Key_Escape)
        close();

    /* Reset the scene */
    if(e->key() == Qt::Key_R){
        transform[0] = transform[1] = 0.0;
        scaling = 1.0;
        uiCurrentPos = 0;
    }
}

/* Image load and texture generation function */
GLuint SceneWidget::loadTextures(const char *filename)
{
    QImage t, b;

    if (!b.load(filename)) {
        b = QImage(16, 16, QImage::Format_ARGB32);
        b.fill(Qt::green);
    }
    /* Format converting to suit OpenGL */
    t = QGLWidget::convertToGLFormat(b);

    GLuint TextureID;
    /* Generate opengl texture */
    glGenTextures(1, &TextureID);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width(), t.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, t.bits());

    return TextureID;
}
