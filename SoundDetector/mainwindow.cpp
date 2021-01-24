#include "mainwindow.h"
#include "wavfilesounddetect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //ui->menubar->setVisiblility(false);

    ui.menuBar->setVisible(false);
    ui.mainToolBar->setVisible(false);
    ui.statusBar->setVisible(false);

    connectSignalsAndSlots();

    loadSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::connectSignalsAndSlots()
{
    connect(ui.btnInputDir, SIGNAL(clicked()), this, SLOT(btnInputDirClicked()));
    connect(ui.btnOutputDir, SIGNAL(clicked()), this, SLOT(btnOutputDirClicked()));
    connect(ui.btnProcessFiles, SIGNAL(clicked()), this, SLOT(btnProcessFilesClicked()));
}

void MainWindow::loadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    _inputDir = settings.value("InputDir", QDir::homePath()).toString();
    if (!QDir(_inputDir).exists()) _inputDir = QDir::homePath();
    _outputDir = settings.value("OutputDir", QDir::homePath()).toString();
    if (!QDir(_outputDir).exists()) _outputDir = QDir::homePath();
    _silenceVolume = settings.value("SilenceVolume", 100).toInt();
    _silenceInterval = settings.value("SilenceInterval", 500).toInt();
    settings.endGroup();

    ui.edtInputDir->setText(_inputDir);
    ui.edtOutputDir->setText(_outputDir);
    ui.spnSilenceVolume->setValue(_silenceVolume);
    ui.spnSilenceInterval->setValue(_silenceInterval);
}

void MainWindow::saveSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("InputDir", _inputDir);
    settings.setValue("OutputDir", _outputDir);
    settings.setValue("SilenceVolume", _silenceVolume);
    settings.setValue("SilenceInterval", _silenceInterval);
    settings.endGroup();
}

void MainWindow::showMessageBox(const QString& message)
{
    QMessageBox messageBox;
    messageBox.setInformativeText(message);
    messageBox.exec();
}

QString MainWindow::selectDirectory(const QString& title)
{
    QFileDialog dlgSelectDir(this);
    dlgSelectDir.setWindowTitle(title);
    dlgSelectDir.setDirectory("");
    dlgSelectDir.setFileMode(QFileDialog::DirectoryOnly);
    if (dlgSelectDir.exec())
    {
        QStringList dirList = dlgSelectDir.selectedFiles();
        if (dirList.size() > 0)
        {
            return dirList[0];
        }
    }
    return "";
}

void MainWindow::btnInputDirClicked()
{
    QString inputDirStr = selectDirectory("Выбор папки со входными звуковыми файлами");
    if (inputDirStr != "")
    {
        _inputDir = inputDirStr;
        ui.edtInputDir->setText(_inputDir);
    }
}

void MainWindow::btnOutputDirClicked()
{
    QString outputDirStr = selectDirectory("Выбор папки с выходными файлами");
    if (outputDirStr != "")
    {
        _outputDir = outputDirStr;
        ui.edtOutputDir->setText(_outputDir);
    }
}

void MainWindow::btnProcessFilesClicked()
{
    bool success = true;
    QStringList filters{"*.wav"};
    QDir inputDir(_inputDir);
    QFileInfoList wavFileInfos = inputDir.entryInfoList(filters, QDir::Files);
    foreach (QFileInfo wavFileInfo, wavFileInfos)
    {
        WavSoundDetectProcessor soundProcessor;
        success &= soundProcessor.processWavFile(wavFileInfo.filePath(),
                        _outputDir + "/" + wavFileInfo.fileName() + ".txt", 100, _silenceVolume, _silenceInterval);
    }
    if (wavFileInfos.size() > 0)
    {
        if (success)
        {
            showMessageBox("Все файлы успешно обработаны");
        }
        else
        {
            showMessageBox("Один или несколько файлов не обработаны");
        }
    }
    else
    {
        showMessageBox("Отсутствуют файлы для обработки");
    }
}
