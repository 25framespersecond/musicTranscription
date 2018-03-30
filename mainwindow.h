#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <vector>
#include "wavfile.h"
#include "fouriertransform.h"
#include "graphbuilder.h"
#include "timeaudiodata.h"
#include "frequencyaudiodata.h"
#include "notedetector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    GraphBuilder *m_timeChart;
    GraphBuilder *m_freqChart;

    Ui::MainWindow *ui;
    TimeAudioData m_timeData;
    FrequencyAudioData m_freqData;
};

#endif // MAINWINDOW_H
