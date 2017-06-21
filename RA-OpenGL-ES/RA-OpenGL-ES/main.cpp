#include <QApplication>
#include "principal.h"
//#include <QSurfaceFormat>

int main( int argc, char **argv )  {

//    Q_INIT_RESOURCE(textures);

    QApplication app( argc, argv );

//    QSurfaceFormat format;
//    format.setDepthBufferSize(24);
//    QSurfaceFormat::setDefaultFormat(format);


    Principal principal;
    principal.show();



    return app.exec();
}


