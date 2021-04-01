/*
 * Holds the gaze data and can save it to file.
 * 18. February 2017, Severin Tobler
 */
#ifndef ANNOTATIONDATAMANAGER_H
#define ANNOTATIONDATAMANAGER_H

#include <QObject>
#include <QMetaType>
#include <QFile>
#include <QSize>


struct CsvData {
    unsigned int frame;         // frame index
    double x, y;                // gaze coordinates relative to image size (between 0 and 1 = gazed on video)
    std::int64_t timestamp;     // timestamp in milliseconds
    bool visible;               // user indicated object is gazed on (space bar pressed = 1)
    bool isFixated;
};
Q_DECLARE_METATYPE(CsvData)


class AnnotationDataManager : public QObject {
    Q_OBJECT
public:
    AnnotationDataManager();
    ~AnnotationDataManager();

    void addVideoData(QString name, QSize size);
    bool saveData(QString path);
    void deleteData();
    bool newContentSinceSave() const;
    QString getCurrentFile() const;
    std::vector<CsvData> getData();
    std::vector<CsvData> getData(unsigned int frame_num);

public slots:
    void newData(const CsvData &data);

signals:
    void modified();

private:
    void writeCsvHeader(QFile* file) const;
    void writeCsvData(QFile* file, const CsvData &data) const;

    QString vidName;
    QSize vidSize;
    std::vector<CsvData> annotationData; //Stores all data for csv file
    QString currentFile;
    bool hasNewContent;
};

#endif // ANNOTATIONDATAMANAGER_H
