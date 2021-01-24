#pragma once

#include <QtWidgets>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void btnInputDirClicked();
    void btnOutputDirClicked();
    void btnProcessFilesClicked();

private:
    Ui::MainWindowClass ui;

    QString _inputDir;
    QString _outputDir;
    int _silenceVolume;
    int _silenceInterval;

    void connectSignalsAndSlots();
    void loadSettings();
    void saveSettings();
    QString selectDirectory(const QString& title);
    void showMessageBox(const QString& message);

};
