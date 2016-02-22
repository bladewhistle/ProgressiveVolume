#include "ProgressiveEngine.h"
//#include "vtkImageData.h"
#include "VolumeRaycastingWidget.h"
#include <QDebug>
#include <QApplication>

ProgressiveEngine::ProgressiveEngine(QObject *parent):
    QObject(parent), _fetchIOThread(this), _resampleThread(this)
{
    _progressive = false;
    _currentStep = 0;
    connect(&_fetchIOThread, SIGNAL(dataReady(unsigned int, vtkImageData*, bool)), this,
            SLOT(onDataReady(unsigned int, vtkImageData*, bool)));
    connect(&_resampleThread, SIGNAL(resampleDataReady(vtkImageData*)), this,
            SLOT(onResampleDataReady(vtkImageData*)));
}

ProgressiveEngine::~ProgressiveEngine()
{
    _resampleThread.stop();
    _fetchIOThread.stop();

    _resampleThread.wait();
    _fetchIOThread.wait();
}


void ProgressiveEngine::prepare()
{
    StepInfo step;
    step.sliceThickness = 20;
    step.useSmall = true;
    _stepsInfo.append(step);

    step.sliceThickness = 8;
    step.useSmall = true;
    _stepsInfo.append(step);

    /* step.sliceThickness = 5;
    step.useSmall = true;
    _stepsInfo.append(step);*/

    step.sliceThickness = 4;
    step.useSmall = true;
    _stepsInfo.append(step);

    step.sliceThickness = 2;
    step.useSmall = false;
    _stepsInfo.append(step);

    step.sliceThickness = 1;
    step.useSmall = false;
    _stepsInfo.append(step);

    _stepsInfo.reserve(_sliceNum);

    for(int i = 0; i < _sliceNum; i++) {
        SliceInfo info;
        _slicesInfo.append(info);
    }

    vtkImageData* allData = createData();

    //short* ptDSrcData = (short*)allData->GetScalarPointer();
    ////// write necessary values
    //*++ptDSrcData = 0;
    //*++ptDSrcData = 5680;
    //*++ptDSrcData = -2048;
    _widget->initialize(allData);
    _widget->setBlendTypeToBone();
    //_widget->initialize(allData);
    _widget->setCameraToFront();
}

void ProgressiveEngine::startStep(unsigned int step)
{
    qDebug() << "\n start step " << step << "\n";
    _currentStepDataMap.clear();

    _currentStep = step;
    StepInfo& stepInfo = _stepsInfo[_currentStep];
    for(int i = 0; i < _sliceNum; i += stepInfo.sliceThickness) {
        SliceInfo& sliceInfo = _slicesInfo[i];
        vtkImageData* data = stepInfo.useSmall ? sliceInfo.smallData : sliceInfo.allData;
        if(data == NULL) {
            _fetchIOThread.startFetch(i, stepInfo.useSmall);
            //qDebug() << "start fetch slice" << i;
        }
    }
}

void ProgressiveEngine::start()
{
    _currentStep = 0;
    _fetchIOThread.start();
    _resampleThread.start();
    //generateData(true);
    startStep(_currentStep);
}

void ProgressiveEngine::onDataReady(unsigned int slice, vtkImageData* data, bool small)
{
    static int count = 0;
    count++;

    qDebug() << "slice fetch complete" << slice << "with request spacing " << _stepsInfo[_currentStep].sliceThickness;
    if(small)
        _slicesInfo[slice].smallData = data;
    else
        _slicesInfo[slice].allData = data;

    int all, complete;
    getImagesProgressForCurrentStep(all, complete, small);
    _widget->setProgress(QString("Step %1: %2/%3").arg(_currentStep + 1).arg(complete).arg(all));
    if(_progressive) {
        if(resampleReadyForCurrentStep()) {
            DataBuildTheard::insertCTData(data, _widget->getInput(), (_sliceNum - slice) / _stepsInfo[_currentStep].sliceThickness - 1);
            //_widget->setBlendTypeToBone();
            /*if(count == 1) {
            _widget->setBlendTypeToBone();
            }*/
            _widget->getInput()->Modified();
            if(count % (_sliceNum / _stepsInfo[_currentStep].sliceThickness / 15) == 0)
                _widget->render();
        }
    }

    if(complete == all) {
        if(!_progressive) {
            generateData();
            _widget->getInput()->Modified();
            //_widget->setBlendTypeToBone();
            _widget->render();

            if(_currentStep < _stepsInfo.count() - 1)
                startStep(++_currentStep);
            else {
                qDebug() << "\n all step complete ";
                _widget->setProgress("All steps complete");
                _widget->render();
                //_fetchIOThread.stop();
                //_resampleThread.stop();
            }
        } else {
            vtkImageData* currentData = _widget->getInput();
            double spacing[3];
            currentData->GetSpacing(spacing);
            if(spacing[2] > _stepsInfo[_currentStep].sliceThickness) {
                // if the resample data not arrived, need manual generate current step all display data
                qDebug() << "\n generate data when step complete, since resample data not arrived.";
                generateData();
                _widget->getInput()->Modified();
                //_widget->setBlendTypeToBone();
                _widget->render();
            }

            if(_currentStep < _stepsInfo.count() - 1) {
                //_currentStep++;
                startStep(++_currentStep);
                _resampleThread.startResample(_widget->getInput(), 1.0 * _stepsInfo[_currentStep - 1].sliceThickness / _stepsInfo[_currentStep].sliceThickness);
                _widget->setState("Resample...");
                _widget->render();
                //_widget->render();
            } else {
                qDebug() << "\n all step complete ";
                _widget->setProgress("All steps complete");
                _widget->render();
                //_fetchIOThread.stop();
                //_resampleThread.stop();
            }
        }
    } else {

    }
}


vtkImageData* ProgressiveEngine::createData()
{
    vtkImageData* allData = vtkImageData::New();

    allData->SetDimensions(512, 512, _sliceNum / _stepsInfo[_currentStep].sliceThickness);
    allData->SetSpacing(0.671, 0.671, _stepsInfo[_currentStep].sliceThickness);
    //allData->SetNumberOfScalarComponents(1);
    //allData->SetScalarTypeToShort();
    allData->AllocateScalars(VTK_SHORT, 1);
    allData->Modified();
    return allData;
}

void ProgressiveEngine::generateData(bool skipData)
{
    // if(!_progressive) {
    vtkImageData* allData = createData();
    _widget->setInput(allData);
    //}

    for(int i = 0; i < _sliceNum; i+=_stepsInfo[_currentStep].sliceThickness) {
        vtkImageData* data = _stepsInfo[_currentStep].useSmall ? _slicesInfo[i].smallData : _slicesInfo[i].allData;
        if(data != NULL)
            DataBuildTheard::insertCTData(data, _widget->getInput(), (_sliceNum - i) / _stepsInfo[_currentStep].sliceThickness - 1);
    }
}

void ProgressiveEngine::onAllDataReady(vtkImageData* data)
{
    //static bool inited = false;
    //if(!inited) {
    //    short* ptDSrcData = (short*)data->GetScalarPointer();
    //    //*++ptDSrcData = -2048;
    //    //*++ptDSrcData = 5680;
    //    _widget->initialize(data);
    //    _widget->setBlendTypeToBone();
    //    _widget->setCameraToBack();
    //    inited = true;
    //} else {
    //    _widget->setInput(data);
    //    data->Modified();
    //    //data->Update();
    //}

    //_widget->render();
    ////data->Modified();
}

void ProgressiveEngine::onResampleDataReady(vtkImageData* data)
{
    static int times = 0;
    times++;
    qDebug() << "\n resample complete " << times;
    double spacing[3], currentSpacing[3];
    data->GetSpacing(spacing);
    _widget->getInput()->GetSpacing(currentSpacing);
    if(spacing[2] >= currentSpacing[2]) {
        qDebug() << "abandon resample data this time";
        data->Delete();
        return;
    }

    _widget->setInput(data);


    qDebug() << "\n current resample spacing " << spacing[2];
    for(int i = 0; i < _sliceNum; i += spacing[2]) {
        vtkImageData* data = _stepsInfo[_currentStep].useSmall ? _slicesInfo[i].smallData : _slicesInfo[i].allData;
        if(data != NULL)
            DataBuildTheard::insertCTData(data, _widget->getInput(), (_sliceNum - i) / spacing[2] - 1);
    }

    _widget->setState("");
    _widget->getInput()->Modified();
    //_widget->setBlendTypeToBone();
    //_widget->getInput()->Update();
    _widget->render();
}

void ProgressiveEngine::getImagesProgressForCurrentStep(int& all, int& complete, bool small)
{
    all = 0;
    complete = 0;
    for(int i = 0; i < _sliceNum; i+=_stepsInfo[_currentStep].sliceThickness) {
        all++;
        vtkImageData* data = small ? _slicesInfo[i].smallData : _slicesInfo[i].allData;
        if(data != NULL)
            complete ++;
    }
}

bool ProgressiveEngine::resampleReadyForCurrentStep()
{
    vtkImageData* data = _widget->getInput();
    double spacing[3];
    data->GetSpacing(spacing);
    return _stepsInfo[_currentStep].sliceThickness == spacing[2];
}

