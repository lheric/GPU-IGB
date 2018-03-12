#ifndef INTERACTORSTYLEWIDGET_H
#define INTERACTORSTYLEWIDGET_H

#include <QWidget>

namespace Ui {
class InteractorStyleWidget;
}

class InteractorStyleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InteractorStyleWidget(QWidget *parent = 0);
    ~InteractorStyleWidget();

private slots:
    void on_moveCamBtn_clicked();

    void on_moveObjBtn_clicked();

private:
    Ui::InteractorStyleWidget *ui;
};

#endif // INTERACTORSTYLEWIDGET_H
