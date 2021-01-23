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

private:
    Ui::MainWindowClass ui;

    QString inputDir;
    QString outputDir;
    int silenceVolume;
    int silenceInterval;

    void connectSignalsAndSlots();
    void loadSettings();
    void saveSettings();
    QString selectDirectory(const QString& title);
    void showMessageBox(const QString& message);

};
