#ifndef FIBERCONFIGPANEL_H
#define FIBERCONFIGPANEL_H

#include <QWidget>
#include "gitlview.h"

class FiberRenderer;

namespace Ui {
class FiberConfigPanel;
}

class FiberConfigPanel : public QWidget, public GitlView
{
    Q_OBJECT

public:
    explicit FiberConfigPanel(FiberRenderer* pcFiberRenderer, QWidget *parent = 0);
    ~FiberConfigPanel();

public:
    void onUpdateTrkFile(GitlUpdateUIEvt &rcEvt);

    void onUpdateROIList(GitlUpdateUIEvt &rcEvt);
private slots:
    void on_openFiber_clicked();

    void on_samplingSlider_valueChanged(int value);

    void on_ROIFromCobombox_activated(const QString &arg1);

    void on_ROIToCobombox_activated(const QString &arg1);

private:
    Ui::FiberConfigPanel *ui;

    FiberRenderer* m_pcFiberRenderer;
};

#endif // FIBERCONFIGPANEL_H
