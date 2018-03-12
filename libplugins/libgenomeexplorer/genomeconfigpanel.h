#ifndef GENOMECONFIGPANEL_H
#define GENOMECONFIGPANEL_H

#include <QWidget>
#include "gitlview.h"

class GenomeExplorer;

namespace Ui {
class GenomeConfigPanel;
}

class GenomeConfigPanel : public QWidget, public GitlView
{
    Q_OBJECT

public:
    explicit GenomeConfigPanel(GenomeExplorer* pcFiberRenderer, QWidget *parent = 0);
    ~GenomeConfigPanel();

protected:
    void initServerList();

private slots:

    void on_connServerBtn_clicked();

    void on_serverListComboBox_activated(const QString &arg1);

    void on_openSNPSubjectBtn_clicked();

    void on_openVoxelSubjectBtn_clicked();

    void on_readyBtn_clicked();

    void on_VEGASRadioButton_toggled(bool checked);

    void on_opaqueModeGroupBox_toggled(bool bChecked);

    void on_openGeneSNPBtn_clicked();

    void on_openLDMatrixBtn_clicked();

    void on_brainExpOpacitySlider_actionTriggered(int action);

private:
    Ui::GenomeConfigPanel *ui;

    GenomeExplorer* m_pcGenomeExplorer;
};

#endif // GENOMECONFIGPANEL_H
