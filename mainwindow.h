#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include <inttypes.h>
#include <QTextStream>

#include "config.h"
#include "flasher.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void fillPortsInfo();
private slots:
    void on_searchPath_clicked();
    void on_flashButton_clicked();
    void on_bootTriggerButton_clicked(bool checked);
    void on_getStatus_clicked();

    void changeProgress(int percent);
    void infoCritical(QString title, QString info);
    void infoWarning(QString title, QString info);
    void infoInfo(QString title, QString info);
    void infoDebug(QString info);

    void on_comSelector_currentIndexChanged(const QString &port_name);

    void on_protocolSelector_currentIndexChanged(int index);

signals:
    void flashing_command(QString hexFilePath, int page_size);

private:
    Ui::MainWindow *ui;
    Flasher *flasher;
};

#endif // MAINWINDOW_H
