#include "mainwindow.h"

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
}

void MainWindow::loadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    inputDir = settings.value("InputDir", "").toString();
    outputDir = settings.value("OutputDir", "").toString();
    silenceVolume = settings.value("SilenceVolume", 100).toInt();
    silenceInterval = settings.value("SilenceInterval", 500).toInt();
    settings.endGroup();

    ui.edtInputDir->setText(inputDir);
    ui.edtOutputDir->setText(outputDir);
    ui.spnSilenceVolume->setValue(silenceVolume);
    ui.spnSilenceInterval->setValue(silenceInterval);
}

void MainWindow::saveSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("InputDir", inputDir);
    settings.setValue("OutputDir", outputDir);
    settings.setValue("SilenceVolume", silenceVolume);
    settings.setValue("SilenceInterval", silenceInterval);
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
        inputDir = inputDirStr;
        ui.edtInputDir->setText(inputDir);
    }
}

void MainWindow::btnOutputDirClicked()
{
    QString outputDirStr = selectDirectory("Выбор папки с выходными файлами");
    if (outputDirStr != "")
    {
        outputDir = outputDirStr;
        ui.edtOutputDir->setText(outputDir);
    }
}
