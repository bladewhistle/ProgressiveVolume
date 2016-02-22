#ifndef PROGRESSIVEENGINE_H
#define PROGRESSIVEENGINE_H

#include <QObject>
#include <QtCore>
#include "FetchIOThread.h"
#include "ResampleThread.h"
#include "DataBuildTheard.h"
#include <vtkImageData.h>


struct SliceInfo {
    vtkSmartPointer<vtkImageData> smallData;
    vtkSmartPointer<vtkImageData> allData;
};

class VolumeRaycastingWidget;
class ProgressiveEngine : public QObject
{
    Q_OBJECT

public:
    ProgressiveEngine(QObject *parent);
    ~ProgressiveEngine();

    void start();
    void prepare();
    unsigned int getSliceNum() const
    {
        return _sliceNum;
    }
    void setSliceNum(unsigned int sliceNum)
    {
        _sliceNum = sliceNum;
    }
    void setWidget(VolumeRaycastingWidget* val)
    {
        _widget = val;
    }
    bool isProgressive() const
    {
        return _progressive;
    }
    void setProgressive(bool value)
    {
        _progressive = value;
    }

public slots:
    void onDataReady(unsigned int slice, vtkImageData* data, bool small);
    void onAllDataReady(vtkImageData* data);
    void onResampleDataReady(vtkImageData* data);

protected:
    void startStep(unsigned int step);
    vtkImageData* createData();

    void getImagesProgressForCurrentStep(int& all, int& complete, bool small);
    bool resampleReadyForCurrentStep();
    void generateData(bool skipData = false);

protected:
    QVector<SliceInfo> _slicesInfo;
    QVector<StepInfo> _stepsInfo;
    QMap<unsigned int,  vtkSmartPointer<vtkImageData> > _currentStepDataMap;
    unsigned int _currentStep;
    unsigned int _sliceNum;
    FetchIOThread _fetchIOThread;
    ResampleThread _resampleThread;
    VolumeRaycastingWidget* _widget;
    bool _progressive;
};

#endif // PROGRESSIVEENGINE_H
