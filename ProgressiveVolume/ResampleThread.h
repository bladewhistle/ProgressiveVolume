#ifndef RESAMPLETHREAD_H
#define RESAMPLETHREAD_H

#include <QThread>
#include <QMutex>

class vtkImageData;

struct ResampleInfo
{
	vtkImageData* data;
	float factor;
};

class ResampleThread : public QThread
{
	Q_OBJECT

public:
	ResampleThread(QObject *parent);
	~ResampleThread();

	virtual void run();
	void startResample( vtkImageData* data, float factor);
	void stop();
signals:
	void resampleDataReady(vtkImageData* data);
private:
	QList<ResampleInfo> _dataList;
	QMutex _mutex;
	bool _stopThread;
	
};

#endif // RESAMPLETHREAD_H
