#include "FetchIOThread.h"

//#include <vtkGDCMImageReader.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>
FetchIOThread::FetchIOThread(QObject *parent)
    : QThread(parent)
{
    _stopThread = false;
}

FetchIOThread::~FetchIOThread()
{

}

void FetchIOThread::run()
{
    while(!_stopThread) {
        bool hasRequest = false;
        unsigned int slice = 0;
        QMutexLocker locker(&_mutex);
        if(_sliceList.count() == 0)
            hasRequest = false;
        else {
            slice = _sliceList.front();
            _sliceList.pop_front();
            hasRequest = true;
        }
        locker.unlock();

        if(hasRequest) {
            QString path = QString("../../mha/image_11-17-2015_%1.mha").arg(slice);

            vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
            reader->SetFileName(path.toUtf8().constData());
            reader->Update();

            vtkImageData* data = reader->GetOutput();
            data->Register(NULL);

            QThread::msleep(30);
            emit dataReady(slice, data, true);
        } else
            QThread::msleep(50);
    }
}

void FetchIOThread::startFetch(unsigned int slice, bool small)
{
    QMutexLocker locker(&_mutex);
    _sliceList.push_back(slice);
    locker.unlock();
}

void FetchIOThread::stop()
{
    _stopThread = true;
}
