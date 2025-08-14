#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <QWidget>
#include<GL/gl.h>
#include<GL/glu.h>
#include<QOpenGLWidget>
#include"QOpenGLFunctions"

class myopenGL : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit myopenGL(QOpenGLWidget *parent = nullptr);
public:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL()  Q_DECL_OVERRIDE;  //绘制QPenGL窗口,有更新发生，函数就会被调用
    void resizeGL(int width , int height) Q_DECL_OVERRIDE; //处理窗口大小变化，w和h是新状态下的宽和高，完成后自动刷新屏幕
signals:

public slots:
};

#endif // MYOPENGL_H
