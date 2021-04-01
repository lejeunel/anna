#include "volumereader.h"
#include <QDebug>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
VolumeReader::VolumeReader(AnnotationDataManager * a) {

    setAnnotationDataManager(a);
    itk::TIFFImageIOFactory::RegisterOneFactory();
    itk::GDCMImageIOFactory::RegisterOneFactory();  // DICOM V3.0 and ACR/NEMA 1&2
    itk::PNGImageIOFactory::RegisterOneFactory();
    videoOpen = false;
}

bool VolumeReader::load(QString filePathName) {
    qDebug() << "Opening video (VolumeReader): " << filePathName;
    fileInfo.setFile(filePathName);

    typedef itk::ImageFileReader< ImageType3D >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();;
    reader->SetFileName(filePathName.toStdString());
    try {
        reader->Update();
        volume = reader->GetOutput();
        videoOpen = true;
    }
    catch ( itk::ExceptionObject & exp ) {
        videoOpen = false;
//        qDebug() << "error opening video: " << exp.GetDescription();
        return false;
    }
    emit nbFramesChanged(getNbFrames());
    currentFrame = -1;
    nextFrame();
    return true;
}

bool VolumeReader::isOpened() const {
    return videoOpen;
}

qint64 VolumeReader::getCurrentFrameNumber() const {
    return currentFrame;
}

QSize VolumeReader::getFrameSize() const {
    // Index[0] = col, Index[1] = row
    ImageType3D::RegionType::SizeType size = volume->GetLargestPossibleRegion().GetSize();
    return QSize(size[0], size[1]);
}

QString VolumeReader::getName() const {
    return fileInfo.fileName();
}

void VolumeReader::setFrame(qint64 frameNumber) {
    if(videoOpen && frameNumber>=0 && frameNumber<getNbFrames()) {
        currentFrame = frameNumber;
        emit frameChanged(drawAllPoints(readFrame()), frameNumber);
    }
    else
        qDebug() << "requested frame outside range... do nothing";
}

/*
 * ==============================================================================
 *                              public slots
 * ==============================================================================
 */
void VolumeReader::nextFrame() {
    if(!videoOpen)
        return;
    qint64 nextFrameNumber = getCurrentFrameNumber()+1;
    setFrame(nextFrameNumber);
}

void VolumeReader::previousFrame() {
    if(!videoOpen)
        return;
    qint64 previousFrameNumber = getCurrentFrameNumber()-1;
    setFrame(previousFrameNumber);
}

/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
qint64 VolumeReader::getNbFrames() const {
    // Index[2] = slice
    return volume->GetLargestPossibleRegion().GetSize()[2];
}

QImage VolumeReader::readFrame() {
    if (!volume)
        return QImage();

    ImageType3D::RegionType frameRegion = volume->GetLargestPossibleRegion();
    frameRegion.SetIndex(2, currentFrame);
    frameRegion.SetSize(2, 1);

    QImage frame(frameRegion.GetSize(0), frameRegion.GetSize(1), QImage::Format_RGB32);
    itk::ImageRegionConstIteratorWithIndex< ImageType3D > it(volume, frameRegion);
    for(it.GoToBegin(); !it.IsAtEnd(); ++it) {
        const ImageType3D::PixelType pixel = it.Get();
        const ImageType3D::IndexType index = it.GetIndex();
        //frame.setPixelColor(index[0], index[1], QColor(pixel[0], pixel[1], pixel[2]));
        frame.setPixel(QPoint(index[0], index[1]), QRgb(pixel[0], pixel[1], pixel[2]));
    }
    return frame;
}

