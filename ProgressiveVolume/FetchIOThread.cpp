#include "FetchIOThread.h"

//#include <vtkGDCMImageReader.h>
#include <vtkMetaImageReader.h>
#include <vtkJPEGReader.h>
#include <vtkImageData.h>
#include <vtkImageResize.h>
#include <vtkImageShiftScale.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageFlip.h>

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
        SliceFetchInfo slice;
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
            QString ext = slice.small ? "jpg" : "mha";
            QString path = QString("../../%1/image_11-17-2015_%2.%1").arg(ext).arg(slice.slice);
            vtkImageData* data = NULL;
            if(slice.small) {
                vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
                reader->SetFileName(path.toUtf8().constData());
                reader->Update();

                data = reader->GetOutput();
                int dimensions[3];
                data->GetDimensions(dimensions);
                if(dimensions[0] == 256) {
                    vtkSmartPointer<vtkImageFlip> flip = vtkSmartPointer<vtkImageFlip>::New();
                    flip->SetFilteredAxis(1);
                    flip->SetInputData(reader->GetOutput());
                    flip->Update();

                    vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
                    extract->SetComponents(0);
                    extract->SetInputData(flip->GetOutput());
                    extract->Update();

                    vtkSmartPointer<vtkImageResize> resize = vtkSmartPointer<vtkImageResize>::New();
                    resize->SetInputData(extract->GetOutput());
                    resize->SetOutputDimensions(512, 512, 1);
                    resize->Update();

                    vtkSmartPointer<vtkImageShiftScale> shift = vtkSmartPointer<vtkImageShiftScale>::New();
                    double scale = 4096.0/ 255;
                    shift->SetShift(-255.0 / 4);
                    shift->SetScale(scale);
                    shift->SetOutputScalarTypeToShort();
                    shift->SetInputData(0, resize->GetOutput());
                    shift->Update();

                    /* vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
                    writer->SetInputData(shift->GetOutput());
                    writer->SetFileName("d:\\1.mhd");
                    writer->Write();*/

                    data = shift->GetOutput();

                    data->Register(NULL);

                    int dimensions[3];
                    data->GetDimensions(dimensions);
                    int i = 0;
                }

            } else {
                vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
                reader->SetFileName(path.toUtf8().constData());
                reader->Update();

                data = reader->GetOutput();
                data->Register(NULL);
            }


            QThread::msleep(30);
            emit dataReady(slice.slice, data, slice.small);
        } else
            QThread::msleep(50);
    }
}

void FetchIOThread::startFetch(unsigned int slice, bool small)
{
    QMutexLocker locker(&_mutex);
    SliceFetchInfo info;
    info.slice = slice;
    info.small = small;
    _sliceList.push_back(info);
    locker.unlock();
}

void FetchIOThread::stop()
{
    _stopThread = true;
}
