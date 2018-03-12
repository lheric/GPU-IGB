#ifndef LABELVOLUME_H
#define LABELVOLUME_H

#include <gitldef.h>
#include <QObject>
#include <vtkSmartPointer.h>
#include <vtkNIFTIImageReader.h>
#include <vtkImageFlip.h>
#include <vtkImageData.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellLocator.h>
#include <vtkClipVolume.h>
#include <vtkPointData.h>
#include <vtkCell.h>
#include <QDebug>
/**
 * @brief The LabelVolume class
 * gray matter volume with label
 */
class LabelVolume : public QObject
{
    Q_OBJECT
public:
    explicit LabelVolume(QObject *parent = 0);
    ~LabelVolume();

    /**
     * @brief init create and connect pipeline
     */
    void init();

    /**
     * @brief setFlipImage flip the Z axe or not
     * @param bFlip
     */
    void setFlipImage(bool bFlip);


    /**
     * @brief openNifti load volume from nifti
     * @param strFilename nifti file path
     */
    bool openNifti(const QString& strFilename );

    /// path of the image file
    ADD_CLASS_FIELD(QString, strFilepath, getFilepath, setFilepath)

    /// reader
    ADD_CLASS_FIELD(vtkSmartPointer<vtkNIFTIImageReader>, pcReader, getReader, setReader)

    /// filp z axes for registration to fiber (registration from the right hand to left hand coordinate system)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageFlip>, pcFilpFilter, getFlipFilter, setFlipFilter)

    /// clip out the part with 0 label (not part of the brain)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkClipVolume>, pcClipper, getClipper, setClipper)

    ///
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageData>, pcUnclippedVolume, getUnclippedVolume, setUnclippedVolume) ///

    /// brain volume with label
    ADD_CLASS_FIELD(vtkSmartPointer<vtkUnstructuredGrid>, pcVolume, getVolume, setVolume)

    /// cell locator for label query to build labelled surface later
    ADD_CLASS_FIELD(vtkSmartPointer<vtkCellLocator>, pcLocator, getLocator, setLocator)

    /// min ROI label
    ADD_CLASS_FIELD(int, iLabelMin, getLabelMin, setLabelMin)

    /// max ROI label
    ADD_CLASS_FIELD(int, iLabelMax, getLabelMax, setLabelMax)

signals:
    void updated();

public slots:
};

#endif // LABELVOLUME_H
