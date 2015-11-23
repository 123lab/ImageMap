#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <vector>
#include <QGLWidget>

class QWheelEvent;

/* Opengl widget, derived from QGLWidget */
class SceneWidget : public QGLWidget
{
    Q_OBJECT

public:
    SceneWidget(QGLWidget *parent = 0);
    ~SceneWidget();

    /* Read image file and generate texture in opengl */
    GLuint loadTextures(const char * filename);

protected:
    /* OpenGL related callback */
    void initializeGL();
    void resizeGL(int w, int height);
    void paintGL();

    //====================================================================

    /* Timer event */
    void timerEvent(QTimerEvent *);

    //====================================================================

    /* Mouse and keyboard event callback */
    // Mouse Wheel
    void wheelEvent(QWheelEvent *);

    /* Mouse press and move event */
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    // Key pressed event
    void keyPressEvent(QKeyEvent *);

private:
    /* Texture ID */
    GLuint textureMap;
    GLuint textureVehicle;

    /* Scale parameters */
    GLdouble scaling;
    /* Transform parameters */
    GLdouble transform[2], mousePos[2];

    /* Vehicle paths */
    GLuint uiCurrentPos;
    std::vector<QPointF> vehiclePaths;
};

#endif // MYGLWIDGET_H
