#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QWidget>

namespace Ui  {
    class Principal;
}

class Principal : public QWidget  {
    Q_OBJECT

public:
    explicit Principal(QWidget *parent = 0);
    ~Principal();

    Ui::Principal *ui;  // Lo hice publico para poder acceder al ui desde Scene
protected:
    void keyPressEvent(QKeyEvent *e);

};

#endif // PRINCIPAL_H
