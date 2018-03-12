#ifndef BRAINEXPLORERWIDGET_H
#define BRAINEXPLORERWIDGET_H

#include <QStackedWidget>

namespace Ui {
class BrainExplorerWidget;
}

class BrainExplorerWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit BrainExplorerWidget(QWidget *parent = 0);
    ~BrainExplorerWidget();

private:
    Ui::BrainExplorerWidget *ui;
};

#endif // BRAINEXPLORERWIDGET_H
