#ifndef DATABUILDTHEARD_H
#define DATABUILDTHEARD_H

#include <QThread>
#include <QVector>
#include <vtkSmartPointer.h>

class vtkImageData;

struct StepInfo {
    unsigned short int sliceThickness;
    bool useSmall;
};

class DataBuildTheard : public QThread
{
    Q_OBJECT

public:
    DataBuildTheard(QObject *parent);
    ~DataBuildTheard();

    virtual void run();
    void setCurrentStepInfo(StepInfo stepInfo)
    {
        _currentStepInfo = stepInfo;
    }

    void setSliceNum(unsigned int sliceNum)
    {
        _sliceNum = sliceNum;
    }

    void addData(vtkImageData* data)
    {
        _dataArray.append(data);
    }

    static void insertCTData(vtkImageData* srcData, vtkImageData* destData, int z);

signals:
    void allDataReady(vtkImageData* data);
private:
    StepInfo _currentStepInfo;
    QVector<vtkImageData*> _dataArray;
    unsigned int _sliceNum;
};

#endif // DATABUILDTHEARD_H
