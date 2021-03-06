#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGraphicsLayout>
#include <QFrame>
#include "onsetdetector.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_timeChart = new GraphBuilder(this);
    m_freqChart = new GraphBuilder(this);

    QBrush brush(QRgb(0x777777));
    m_timeChart->chart()->setBackgroundBrush(brush);
    m_freqChart->chart()->setBackgroundBrush(brush);
    m_timeChart->chart()->setBackgroundRoundness(0);
    m_freqChart->chart()->setBackgroundRoundness(0);

    m_timeChart->chart()->setContentsMargins(0, 0, 0, 0);
    m_timeChart->chart()->layout()->setContentsMargins(0, 0, 0, 0);
    m_freqChart->chart()->setContentsMargins(0, 0, 0, 0);
    m_freqChart->chart()->layout()->setContentsMargins(0, 0, 0, 0);

    m_timeChart->setFrameStyle(QFrame::Box | QFrame::Sunken);
    m_freqChart->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->layout1->setSpacing(0);

    ui->layout1->addWidget(m_timeChart);
    ui->layout1->addWidget(m_freqChart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open WAV file", "",
                                                    "Wave audion (*.wav)");
    if (filename.isEmpty())
        return;
    WavFile file;
    if(!file.open(filename))
        return;
    m_timeData = file.getAudioData();

    m_timeChart->plot(m_timeData, "Waveform");

    m_freqData = FourierTransform::cooleyTukeyFFT(m_timeData, 0, m_timeData.size());

    m_freqChart->plot(m_freqData, "Frequencies");

    auto onset = OnsetDetector::detectOnset(m_timeData);

    m_freqChart->plot(onset, "Onset");
}
