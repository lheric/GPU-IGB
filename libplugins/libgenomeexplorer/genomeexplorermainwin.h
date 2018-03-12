#ifndef GENOMEEXPLORERMAINWIN_H
#define GENOMEEXPLORERMAINWIN_H


#include <vtkImageData.h>
#include <QWidget>
#include "gitlview.h"
#include "genomechart.h"
#include "heatmap.h"



namespace Ui {
class GenomeExplorerMainwin;
}

class GenomeExplorerMainwin : public QWidget, public GitlView
{
    Q_OBJECT

public:
    explicit GenomeExplorerMainwin(QWidget *parent = 0);
    ~GenomeExplorerMainwin();

    void init();

    /*!
     * \brief setBGData Set the GM intensity volume data
     */
    void setBGData(vtkImageData* pcImageData);

    ADD_CLASS_FIELD_NOSETTER(GenomeChart, cGenomeChart, getGenomeChart)
    ADD_CLASS_FIELD_NOSETTER(Heatmap, cHeatmap, getHeatmap)




private:
    Ui::GenomeExplorerMainwin *ui;
};

#endif // GENOMEEXPLORERMAINWIN_H
