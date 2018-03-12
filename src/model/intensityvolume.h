#ifndef INTENSITYVOLUME_H
#define INTENSITYVOLUME_H

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
#include <vtkPointDataToCellData.h>
#include <vtkImageGradient.h>

/**
 * @brief The ProbabilityVolumn class
 * for gray matter iso surface (read from NIFTI file [*.nii] with probabilities)
 */
class IntensityVolume : public QObject
{
    Q_OBJECT
public:
    explicit IntensityVolume(QObject *parent = 0);
    ~IntensityVolume();

    /**
     * @brief init create and connect pipeline
     */
    void init();

    /**
     * @brief openNifti load volume from nifti
     * @param strFilename nifti file path
     */
    bool openNifti(const QString& strFilename );

    /**
     * @brief setFilpImage flip its upside down or not
     * @param bFlip
     */
    void setFilpImage(bool bFlip);

    /// path of the image file
    ADD_CLASS_FIELD(QString, strFilepath, getFilepath, setFilepath)

    /// reader
    ADD_CLASS_FIELD(vtkSmartPointer<vtkNIFTIImageReader>, pcReader, getReader, setReader)

    /// filp z axes for registration to fiber (registration from the right hand to left hand coordinate system)
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageFlip>, pcFilpFilter, getFlipFilter, setFlipFilter)

    /// volume read
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageData>, pcVolume, getVolume, setVolume)

    /// gradient filter
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageGradient>, pcGradientFilter, getGradientFilter, setGradientFilter)

    /// volume gradient
    ADD_CLASS_FIELD(vtkSmartPointer<vtkImageData>, pcGradient, getGradient, setGradient)

//    ADD_CLASS_FIELD(vtkSmartPointer<vtkContourFilter>, pcContourFilter, getContourFilter, setContourFilter)
//    ADD_CLASS_FIELD(vtkSmartPointer<vtkPolyDataMapper>, pcMapper, getMapper, setMapper)
//    ADD_CLASS_FIELD(vtkSmartPointer<vtkLookupTable>, pcLUT, getLUT, setLUT)    /// color look up table for labels
//    ADD_CLASS_FIELD(vtkSmartPointer<vtkActor>, pcActors, getActors, setActors)
signals:
    void updated();

public slots:
};

#endif // INTENSITYVOLUME_H
