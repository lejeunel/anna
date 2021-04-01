#include "annotationdatamanager.h"
#include <QDateTime>

/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
AnnotationDataManager::AnnotationDataManager() : hasNewContent(false) {
    qRegisterMetaType<CsvData>();  // not sure if that's the right place to call this
}

AnnotationDataManager::~AnnotationDataManager() {
}

void AnnotationDataManager::addVideoData(QString name, QSize size) {
    vidName = name;
    vidSize = size;
}

bool AnnotationDataManager::saveData(QString path) {
    QFile* tracking_data = new QFile(path);
    if(!tracking_data->open(QFile::WriteOnly | QFile::Text))
        return false;

    writeCsvHeader(tracking_data);
    for(const CsvData& data : annotationData)
        writeCsvData(tracking_data, data);

    tracking_data->close();
    currentFile = path;
    hasNewContent = false;
    return true;
}

void AnnotationDataManager::deleteData() {
    vidSize.setHeight(0);
    vidSize.setWidth(0);
    hasNewContent = false;
    currentFile = QString();
    vidName = QString();
    annotationData.clear();
}

bool AnnotationDataManager::newContentSinceSave() const {
    return hasNewContent;
}

QString AnnotationDataManager::getCurrentFile() const {
    return currentFile;
}

std::vector<CsvData> AnnotationDataManager::getData(){

    return annotationData;
}

std::vector<CsvData> AnnotationDataManager::getData(unsigned int frame_num){

    std::vector<CsvData> out;

    for(const CsvData& data : annotationData)
        if(data.frame == frame_num)
            out.push_back(data);
    return out;
}

/*
 * ==============================================================================
 *                              public slots
 * ==============================================================================
 */
void AnnotationDataManager::newData(const CsvData &data) {
    annotationData.push_back(data);
    hasNewContent = true;
    emit modified();
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
void AnnotationDataManager::writeCsvHeader(QFile* file) const {
    // data, time, video resolution
    QDateTime clock = QDateTime::currentDateTime();
    file->write(QString("AcquisitionDate;%1\n").arg(clock.date().toString()).toStdString().c_str());
    file->write(QString("AcquisitionTime;%1\n").arg(clock.time().toString()).toStdString().c_str());
    file->write(QString("VideoName;%1\n").arg(vidName).toStdString().c_str());
    file->write(QString("VideoWidth;%1\n").arg(vidSize.width()).toStdString().c_str());
    file->write(QString("VideoHeight;%1\n\n").arg(vidSize.height()).toStdString().c_str());

    file->write("frame;timestamp;visible;x;y;isFixated\n");
}

void AnnotationDataManager::writeCsvData(QFile* file, const CsvData &data) const {
    // frame index, timestamp, visible, x, y, isFixated
    file->write(QString("%1;").arg(data.frame).toStdString().c_str());
    file->write(QString("%1;").arg(data.timestamp).toStdString().c_str());
    file->write(QString("%1;").arg(data.visible).toStdString().c_str());
    file->write(QString("%1;").arg(data.x).toStdString().c_str());
    file->write(QString("%1;").arg(data.y).toStdString().c_str());
    file->write(QString("%1\n").arg(data.isFixated).toStdString().c_str());
}






