#ifndef PROGRESSIVEVOLUME_H
#define PROGRESSIVEVOLUME_H

#include <QtGui/QWidget>
#include "ui_progressivevolume.h"
#include "ProgressiveEngine.h"

class VolumeRaycastingWidget;
class vtkImageData;
class ProgressiveVolume : public QWidget
{
    Q_OBJECT

public:
    ProgressiveVolume(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~ProgressiveVolume();

protected:
    void prepareData();
    void insertCTData(vtkImageData* srcData, vtkImageData* destData, int z);
private:
    Ui::ProgressiveVolumeClass ui;
    VolumeRaycastingWidget* _volumeWidget;
    ProgressiveEngine _engine;
};

#endif // PROGRESSIVEVOLUME_H
