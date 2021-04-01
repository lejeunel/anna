/*
 * Reads frames from image volumes. Uses ITK for image reading.
 * Currently supports dicom, tiff and png formats.
 * 03. April 2017, Severin Tobler
 */
#ifndef VOLUMEREADER_H
#define VOLUMEREADER_H

#include "framereaderinterface.h"
#include <QFileInfo>
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkPNGImageIOFactory.h"
#include "itkTIFFImageIOFactory.h"
#include "itkGDCMImageIOFactory.h"

class VolumeReader : public FrameReaderInterface {
public:
    VolumeReader(AnnotationDataManager * a);
    bool load(QString filePathName) override;
    bool isOpened() const override;
    qint64 getCurrentFrameNumber() const override;
    QSize getFrameSize() const override;
    QString getName() const override;
    void setFrame(qint64 frameNumber) override;

public slots:
    void nextFrame() override;
    void previousFrame() override;

private:
    typedef itk::RGBPixel< unsigned char >    PixelType;
    typedef itk::Image< PixelType, 3 >   ImageType3D;

    qint64 getNbFrames() const;
    QImage readFrame();

    ImageType3D::Pointer volume;
    QFileInfo fileInfo;
    bool videoOpen;
    qint64 currentFrame;
};

#endif // VOLUMEREADER_H
