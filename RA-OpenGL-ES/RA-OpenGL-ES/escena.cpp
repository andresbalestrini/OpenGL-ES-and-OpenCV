#include "escena.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#include <QMouseEvent>
#include <QKeyEvent>

#include <QMessageBox>

#include <QDebug>
#include <QApplication>

#include <QDir>

Escena::Escena(QWidget *parent) : QOpenGLWidget(parent),

                                  videoCapture ( new cv::VideoCapture ),

                                  sceneTimer ( new QTimer ),
                                  milisecondsTimer( 15 ),
                                  markerDetector( new MarkerDetector ),
                                  cameraParameters( new CameraParameters )
{
    if ( ! videoCapture->open( CV_CAP_ANY ) )  {
        QMessageBox::critical(this, "No se pudo iniciar camara", "Problema con videoCapture->open");
    }

    cameraParameters->readFromXMLFile( "../files/CameraParameters.yml" );

    if ( ! cameraParameters->isValid() )
        QMessageBox::critical(this, "Error con YML", "No es valido");

    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_updateScene() ) );
    sceneTimer->start( milisecondsTimer );

    x = y = 0;
    z = 0;
    angulo = 0;

    vImageBuffer = new QVector< QOpenGLBuffer * >();
    vImageTexture = new QVector< QOpenGLTexture * >();


}

Escena::~Escena()  {

    makeCurrent();

    cam_buffer->destroy();

    delete program;

    doneCurrent();

    videoCapture->release();
}

/*
 * Carga todas las texturas y imagenes en las carpeta 'Textures' y las almacena una por una en el vector
 * 'vImageTextures'. En vImageBuffer almacena las coordenadas (vertices y indices)
 * correspondientes a cada una de ellas
*/
void Escena::loadTextures()
{
    QDir directory( "../Textures" );

    QStringList fileFilter;
    fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
    QStringList imageFiles = directory.entryList( fileFilter );

    qDebug() << imageFiles;

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        QOpenGLBuffer * imageBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );

        imageBuffer->create();

        static const int coords[1][4][3] = { { { +1, -1, 0 }, { -1, -1, 0 }, { -1, 0, 0 }, { +1, 0, 0 } } };

        QVector<GLfloat> vertData;
        for (int i = 0; i < 1; ++i) {
            for (int j = 0; j < 4; ++j) {
                // vertex position
                vertData.append( coords[i][j][0]);
                vertData.append( coords[i][j][1]);
                vertData.append( coords[ i ][ j ][ 2 ]);

                // texture coordinate
                if ( j==0 )  {  vertData.append(1);  vertData.append(0);  }
                if ( j==1 )  {  vertData.append(0);  vertData.append(0);  }
                if ( j==2 )  {  vertData.append(0);  vertData.append(1);  }
                if ( j==3 )  {  vertData.append(1);  vertData.append(1);  }
            }
        }

        if ( ! imageBuffer->bind() )
            QMessageBox::critical(this, "False", "vbo2 bind()");

        imageBuffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );


        QOpenGLTexture * imageTexture = new QOpenGLTexture( QImage( "../Textures/" + imageFiles.at( i ) ) );
        imageTexture->setMinificationFilter( QOpenGLTexture::Nearest );
        imageTexture->setMagnificationFilter( QOpenGLTexture::Linear );


    vImageBuffer->append( imageBuffer );
    vImageTexture->append( imageTexture );

    }
}

/*
 * 'loadTextures' --> carga todas las texturas con sus coordenadas correspondientes dejandolas disponibles
 * en el vector de texturas 'vImageTexture' y en el vector de coordenadas 'vImageBuffer'
 *
 * 'crearProgram' --> genera el SharedProgram que genera y controla las tuberias con el
 * 'vShared y el fShared
 *
 * 'crearCam' --> genera los datos y los almacena en 'cam_buffer' para que se levante
 * la imagen de la camara
*/
void Escena::initializeGL()  {

    initializeOpenGLFunctions();

    glClearColor( 1, 1, 1, 1 );

    this->loadTextures();

    this->crearProgram();

    this->crearCam();
//    this->crearImage();

    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
//    glEnable(GL_CULL_FACE);  // Hace que no se vea el interior de una caja


    geometries = new GeometryEngine;

}

void Escena::resizeGL( int w, int h )  {

    glViewport( 0, 0, w, h );

}

void Escena::paintGL()
{
    // primero dibujo en el plano que contendra la imagen de la camara
    this->drawCamera();

    if( detectedMarkers.size() > 0 )  {

        vImageTexture->at( 2 )->bind();
        vImageBuffer->at( 2 )->bind();

        double projectionMatrix[16];
        cameraParameters->glGetProjectionMatrix( cv::Size2i( 640, 480 ),
                                                 cv::Size2i( 640, 480 ),
                                                 projectionMatrix, 0.05, 10 );

        for( unsigned int i = 0 ; i < detectedMarkers.size() ; i++ )
        {
            double modelview_matrix[ 16 ];
            detectedMarkers.operator []( i ).glGetModelViewMatrix( modelview_matrix );

            QMatrix4x4 projection;
            QMatrix4x4 modelView;

            projection( 0, 0 ) = projectionMatrix[ 0 ];
            projection( 1, 0 ) = projectionMatrix[ 1 ];
            projection( 2, 0 ) = projectionMatrix[ 2 ];
            projection( 3, 0 ) = projectionMatrix[ 3 ];
            projection( 0, 1 ) = projectionMatrix[ 4 ];
            projection( 1, 1 ) = projectionMatrix[ 5 ];
            projection( 2, 1 ) = projectionMatrix[ 6 ];
            projection( 3, 1 ) = projectionMatrix[ 7 ];
            projection( 0, 2 ) = projectionMatrix[ 8 ];
            projection( 1, 2 ) = projectionMatrix[ 9 ];
            projection( 2, 2 ) = projectionMatrix[ 10 ];
            projection( 3, 2 ) = projectionMatrix[ 11 ];
            projection( 0, 3 ) = projectionMatrix[ 12 ];
            projection( 1, 3 ) = projectionMatrix[ 13 ];
            projection( 2, 3 ) = projectionMatrix[ 14 ];
            projection( 3, 3 ) = projectionMatrix[ 15 ];

            modelView( 0, 0 ) = modelview_matrix[ 0 ];
            modelView( 1, 0 ) = modelview_matrix[ 1 ];
            modelView( 2, 0 ) = modelview_matrix[ 2 ];
            modelView( 3, 0 ) = modelview_matrix[ 3 ];
            modelView( 0, 1 ) = modelview_matrix[ 4 ];
            modelView( 1, 1 ) = modelview_matrix[ 5 ];
            modelView( 2, 1 ) = modelview_matrix[ 6 ];
            modelView( 3, 1 ) = modelview_matrix[ 7 ];
            modelView( 0, 2 ) = modelview_matrix[ 8 ];
            modelView( 1, 2 ) = modelview_matrix[ 9 ];
            modelView( 2, 2 ) = modelview_matrix[ 10 ];
            modelView( 3, 2 ) = modelview_matrix[ 11 ];
            modelView( 0, 3 ) = modelview_matrix[ 12 ];
            modelView( 1, 3 ) = modelview_matrix[ 13 ];
            modelView( 2, 3 ) = modelview_matrix[ 14 ];
            modelView( 3, 3 ) = modelview_matrix[ 15 ];

            modelView.scale( 1.25 );
            modelView.translate( x, y, z );
            modelView.rotate( angulo, 0, 0, 1 );

            program->setUniformValue( "projection", projection * modelView );

            // Draw cube geometry
//            geometries->drawCubeGeometry( program );
            geometries->drawSheetGeometry( program );


        }
    }
}


void Escena::slot_updateScene()  {

    if ( ! videoCapture->isOpened() )
        return;

    videoCapture->operator >>( matCamera );

    this->process( matCamera );

    update();
}

void Escena::crearProgram()
{
    program = new QOpenGLShaderProgram;

    // Compile vertex shader
    if ( ! program->addShaderFromSourceFile( QOpenGLShader::Vertex, ":/vshader.glsl" ) )
        QMessageBox::critical( this, "No se cargo vshader.glsl", "Falla" );

    // Compile fragment shader
    if ( ! program->addShaderFromSourceFile( QOpenGLShader::Fragment, ":/fshader.glsl" ) )
        QMessageBox::critical( this, "No se cargo fshader.glsl", "Falla" );

    // Link shader pipeline
    if ( ! program->link() )
        QMessageBox::critical( this, "No se Linkeo", "Falla" );

    // Bind shader pipeline for use
    if ( ! program->bind() )
        QMessageBox::critical( this, "No se bindeo", "Falla" );

}


/*
 * genera las coordenadas (vertices y indices) donde se pegara la imagen de la camara y
 * los almacena en 'cam_buffer' para que se levante la imagen de la camara
*/
void Escena::crearCam()
{
    cam_buffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
    cam_buffer->create();

    static const int coords[1][4][3] = { { { +1, -1, -999 }, { -1, -1, -999 }, { -1, +1, -999 }, { +1, +1, -999 } } };

    QVector<GLfloat> vertData;
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[ i ][ j ][ 0 ]);
            vertData.append(coords[ i ][ j ][ 1 ]);
            vertData.append(coords[ i ][ j ][ 2 ]);

            // texture coordinate
            if ( j==0 )  {  vertData.append( 1 );  vertData.append( 0 );  }
            if ( j==1 )  {  vertData.append( 0 );  vertData.append( 0 );  }
            if ( j==2 )  {  vertData.append( 0 );  vertData.append( 1 );  }
            if ( j==3 )  {  vertData.append( 1 );  vertData.append( 1 );  }
        }
    }

    if ( ! cam_buffer->bind() )
        QMessageBox::critical(this, "False", "vbo2 bind()");

    cam_buffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );

    cam_texture = new QOpenGLTexture( QOpenGLTexture::Target2D );
    cam_texture->setMinificationFilter( QOpenGLTexture::Nearest );
    cam_texture->setMagnificationFilter( QOpenGLTexture::Linear );
    cam_texture->setFormat( QOpenGLTexture::RGBA8_UNorm );

    // Estos valores hay que sacarlo directamente de los Mat que entrega el OpenCV
    cam_texture->setSize( 640, 480 );
}

void Escena::crearImage()
{
    image_buffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
    image_buffer->create();

    static const int coords[1][4][3] = { { { +1, -1, -10 }, { -1, -1, -10 }, { -1, 0, -10 }, { +1, 0, -10 } } };

    QVector<GLfloat> vertData;
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[i][j][0]);
            vertData.append(coords[i][j][1]);
            vertData.append(coords[i][j][2]);

            // texture coordinate
            if ( j==0 )  {  vertData.append(1);  vertData.append(0);  }
            if ( j==1 )  {  vertData.append(0);  vertData.append(0);  }
            if ( j==2 )  {  vertData.append(0);  vertData.append(1);  }
            if ( j==3 )  {  vertData.append(1);  vertData.append(1);  }
        }
    }

    if ( ! image_buffer->bind() )
        QMessageBox::critical(this, "False", "vbo2 bind()");

    image_buffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );

//    image_texture = new QOpenGLTexture( QOpenGLTexture::Target2D );
    image_texture = new QOpenGLTexture( QImage( ":/images/side1.png" ) );
    image_texture->setMinificationFilter( QOpenGLTexture::Nearest );
    image_texture->setMagnificationFilter( QOpenGLTexture::Linear );
//    image_texture->setFormat( QOpenGLTexture::RGBA8_UNorm );

}

/*
 * 'cam_buffer' contiene las coodenadas (vertices y indices) donde se dibujara
 * la imagen de la camara, es por esto que hacemos bind() para utilizar y indicar que vamos
 * a dibujar sobre ese plano
 *
 * 'projection' sera nuestra matriz de projeccion que definimos como orthonormal
*/
void Escena::drawCamera()
{    
    cam_buffer->bind();

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.ortho( -1.0f, +1.0f, +1.0f, -1.0f, -10.0f, 1000.0f );

    program->setUniformValue( "projection", projection );
    program->enableAttributeArray( PROGRAM_VERTEX_ATTRIBUTE );
    program->enableAttributeArray( PROGRAM_TEXCOORD_ATTRIBUTE );
    program->setAttributeBuffer( PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof( GLfloat ) );
    program->setAttributeBuffer( PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof( GLfloat ), 2, 5 * sizeof( GLfloat ) );

    if ( matCamera.rows > 0 )  {
        cv::Mat temp( matCamera.cols, matCamera.rows, matCamera.type() );
        cvtColor( matCamera, temp, CV_BGR2RGB );

//        cam_texture->setSize( matCamera.cols, matCamera.rows );

        if ( ! cam_texture->isStorageAllocated() )  {
            cam_texture->allocateStorage();
        }

        cam_texture->setData( QOpenGLTexture::RGB, QOpenGLTexture::UInt8, temp.data );

        cam_texture->bind();

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

/*
 * Detecta el marker y dibuja el contorno en el mismo
 */
void Escena::process( Mat & frame )
{
    flip( frame, frame, 1 );

    cameraParameters->resize( frame.size() );
    markerDetector->detect( frame, detectedMarkers, *cameraParameters, 0.57f );

    // Descripcion del marker
    for( unsigned int i = 0; i < detectedMarkers.size(); i++ )
        detectedMarkers.at( i ).draw( frame, Scalar( 255, 0, 255 ), 1 );
}

