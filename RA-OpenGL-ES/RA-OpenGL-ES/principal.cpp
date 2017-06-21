#include "principal.h"
#include "ui_principal.h"
#include <QKeyEvent>
#include <QDebug>

Principal::Principal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Principal)
{
    ui->setupUi(this);

    // Para estirar la columna 0, que es la escena
    ui->principalLayout->setColumnStretch( 0, 1 );

    this->setFont( QFont( "Tahoma", 8 ) );

//    connect(ui->scene, SIGNAL(message(QString)), ui->teMessages, SLOT(append(QString)));

//    connect(ui->rotateCheckBox, SIGNAL(clicked(bool)), ui->rotationVelocitySlider, SLOT(setEnabled(bool)));
}

Principal::~Principal()
{
    delete ui;
}

void Principal::keyPressEvent( QKeyEvent * e )
{
    float desplazamiento = 0.1;
    switch( e->key() )  {
    case Qt::Key_Escape:
        this->close();
        break;

    case Qt::Key_W:
        ui->openGLWidget->z += desplazamiento;
        break;

    case Qt::Key_S:
        ui->openGLWidget->z -= desplazamiento;
        break;

    case Qt::Key_A:
        ui->openGLWidget->x += desplazamiento;
        break;

    case Qt::Key_D:
        ui->openGLWidget->x -= desplazamiento;
        break;

    case Qt::Key_R:
        ui->openGLWidget->y += desplazamiento;
        break;

    case Qt::Key_F:
        ui->openGLWidget->y -= desplazamiento;
        break;

    case Qt::Key_T:
        ui->openGLWidget->angulo += 30 * desplazamiento;
        break;

    case Qt::Key_G:
        ui->openGLWidget->angulo -= 30 * desplazamiento;
        break;


    default:;
    }

    qDebug() << ui->openGLWidget->x << ui->openGLWidget->y << ui->openGLWidget->z;
}



