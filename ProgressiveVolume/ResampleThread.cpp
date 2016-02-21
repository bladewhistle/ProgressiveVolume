#include "ResampleThread.h"
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkImageResample.h>

ResampleThread::ResampleThread(QObject *parent)
    : QThread(parent)
{
    _stopThread = false;
}

ResampleThread::~ResampleThread()
{

}

void ResampleThread::run()
{
    while(!_stopThread) {
        bool hasRequest = false;
        ResampleInfo resampleInfo;
        QMutexLocker locker(&_mutex);
        if(_dataList.count() == 0)
            hasRequest = false;
        else {
            resampleInfo = _dataList.front();
            _dataList.pop_front();
            hasRequest = true;
        }
        locker.unlock();

        if(hasRequest) {
            vtkSmartPointer<vtkImageResample> imageResample = vtkSmartPointer<vtkImageResample>::New();
            imageResample->SetInputData(resampleInfo.data);
            imageResample->SetAxisMagnificationFactor(2, resampleInfo.factor);
            imageResample->Update();
            vtkImageData* result = imageResample->GetOutput();
            result->Register(NULL);
            emit resampleDataReady(result);
        } else
            QThread::msleep(50);
    }
}

void ResampleThread::startResample(vtkImageData* data, float factor)
{
    ResampleInfo resampleInfo;
    resampleInfo.data = data;
    resampleInfo.factor = factor;

    QMutexLocker locker(&_mutex);
    _dataList.push_back(resampleInfo);
    locker.unlock();
}

void ResampleThread::stop()
{
    _stopThread = true;
}
