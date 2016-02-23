#pragma once
#include <QThread>
#include "ICPHttpResult.h"
#include <QList>
#include <QMutex>

class vtkImageData;
class DownloadIOThread :
    public QThread
{
    Q_OBJECT

public:
    DownloadIOThread(QObject *parent);
    virtual ~DownloadIOThread(void);

    virtual void run();
    void startFetch(unsigned int slice, bool small);
    void stop();
    QString serverUrl() const
    {
        return _serverUrl;
    }
    void setServerUrl(QString value)
    {
        _serverUrl = value;
    }


signals:
    void dataReady(unsigned int slice, vtkImageData* data, bool small);

protected:
    void addToList(QString path);

private:
    QList<QString> _downLoadList;
    QString _serverUrl;
    QMutex _mutex;
    bool _stopThread;
};

