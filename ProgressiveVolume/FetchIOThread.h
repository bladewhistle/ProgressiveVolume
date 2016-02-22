#ifndef FETCHIOTHREAD_H
#define FETCHIOTHREAD_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <vtkSmartPointer.h>

typedef struct {
    unsigned int slice;
    bool small;
} SliceFetchInfo;


class vtkImageData;

class FetchIOThread : public QThread
{
    Q_OBJECT

public:
    FetchIOThread(QObject *parent);
    ~FetchIOThread();

    virtual void run();
    void startFetch(unsigned int slice, bool small);
    void stop();
signals:
    void dataReady(unsigned int slice, vtkImageData* data, bool small);
private:
    QList<SliceFetchInfo> _sliceList;
    QMutex _mutex;
    bool _stopThread;
};

#endif // FETCHIOTHREAD_H
