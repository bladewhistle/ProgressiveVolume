#include "DownloadIOThread.h"
#include "ICPHttpClient.h"
#include <QFile>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkImageData.h>
#include <vtkImageResize.h>
#include <vtkImageShiftScale.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageFlip.h>


DownloadIOThread::DownloadIOThread(QObject *parent)
    : QThread(parent)
{
    _stopThread = false;

    _serverUrl = "http://127.0.0.1:8080";
}


DownloadIOThread::~DownloadIOThread(void)
{
}

void DownloadIOThread::run()
{
    while(!_stopThread) {
        bool hasRequest = false;
        QString path;
        QMutexLocker locker(&_mutex);
        if(_downLoadList.count() == 0)
            hasRequest = false;
        else {
            path = _downLoadList.front();
            _downLoadList.pop_front();
            hasRequest = true;
        }
        locker.unlock();

        if(hasRequest) {
            bool small = path.indexOf("jpg") != -1;
            int start = path.lastIndexOf("_") + 1;
            int end = path.lastIndexOf(".");
            int slice = path.mid(start, end - start).toInt();
            vtkImageData* data = NULL;
            if(small) {
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

            int sleepTime = qrand() % (small ? 14: 36);
            QThread::msleep(small? 30: 70 + sleepTime);
            emit dataReady(slice, data, small);
        } else
            QThread::msleep(10);
    }
}

void DownloadIOThread::startFetch(unsigned int slice, bool small)
{
    QString ext = small ? "jpg" : "mha";
    QString path = QString("/%1/image_11-17-2015_%2.%1").arg(ext).arg(slice);

    auto func1 = [this](ICPHttpResult& result) {
        QString url = result.getUrl();
        QString destFileName = url.mid(url.lastIndexOf("/") + 1);
        QString destPathFileName = result.getResult().left(result.getResult().lastIndexOf("/") + 1) + destFileName;
        QFile::remove(destPathFileName);

        QFile::rename(result.getResult(), destPathFileName);
        addToList(destPathFileName);
    };

    ICPHttpClient* client = new ICPHttpClient(_serverUrl + path);
    client->setDownloadPath("../../download");
    client->setFinishFuc(func1);
    client->get();
}

void DownloadIOThread::stop()
{
    _stopThread = true;
}

void DownloadIOThread::addToList(QString path)
{
    QMutexLocker locker(&_mutex);
    _downLoadList.append(path);
    locker.unlock();
}
