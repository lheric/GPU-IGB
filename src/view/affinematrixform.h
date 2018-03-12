#ifndef AFFINEMATRIXFORM_H
#define AFFINEMATRIXFORM_H

#include <QWidget>

namespace Ui {
class AffineMatrixForm;
}

class AffineMatrixForm : public QWidget
{
    Q_OBJECT

public:
    explicit AffineMatrixForm(QWidget *parent = 0);
    ~AffineMatrixForm();

private slots:
    void on_tableWidget_cellChanged(int row, int column);

private:
    Ui::AffineMatrixForm *ui;
};

#endif // AFFINEMATRIXFORM_H
