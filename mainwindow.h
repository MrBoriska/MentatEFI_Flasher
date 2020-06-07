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

    void on_comSelector_currentIndexChanged(int index);
    void on_protocolSelector_currentIndexChanged(int index);

    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);


signals:
    void flashing_command(QString hexFilePath, int page_size);

private:
    Ui::MainWindow *ui;
    Flasher *flasher;

    void fillPortsInfo();
    void set_flash_file_url(QString filename, bool update_settings=true);
};

#endif // MAINWINDOW_H
