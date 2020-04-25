#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QThread>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QDateTime>

#include <QDragEnterEvent>
#include <QMimeData>

#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // инициализация класса прошивальщика
    this->flasher = new Flasher();

    this->connect(flasher, &Flasher::changeProgress, this, &MainWindow::changeProgress);
    this->connect(flasher, &Flasher::infoCritical, this, &MainWindow::infoCritical);
    this->connect(flasher, &Flasher::infoWarning, this, &MainWindow::infoWarning);
    this->connect(flasher, &Flasher::infoInfo, this, &MainWindow::infoInfo);
    this->connect(flasher, &Flasher::infoDebug, this, &MainWindow::infoDebug);

    // Конфигурирование GUI
    ui->setupUi(this);

    ui->bootTriggerButton->setEnabled(false);
    ui->filePathEdit->setEnabled(true);
    ui->searchPath->setEnabled(true);
    ui->flashButton->setEnabled(false);
    ui->pageSizeSpin->setEnabled(false);

    setAcceptDrops(true);

    // Получение списка COM портов
    fillPortsInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillPortsInfo()
{
    ui->comSelector->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : "n/a")
             << (!manufacturer.isEmpty() ? manufacturer : "n/a")
             << (!serialNumber.isEmpty() ? serialNumber : "n/a")
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : "n/a")
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : "n/a");

        ui->comSelector->addItem(info.portName() + " (" + list.at(1) + ")", list);
    }
}


void MainWindow::on_searchPath_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open hex"), "", tr("Hex file (*.hex)"));

    qDebug() << fileName;

    this->set_flash_file_url(fileName);
}

void MainWindow::set_flash_file_url(QString filename)
{
    ui->filePathEdit->setText(filename);

    if (!filename.isEmpty()) {
        ui->flashButton->setEnabled(true);
        ui->pageSizeSpin->setEnabled(true);
    } else {
        ui->flashButton->setEnabled(false);
        ui->pageSizeSpin->setEnabled(false);
    }
}

void MainWindow::on_flashButton_clicked()
{
    QString hexFilePath = ui->filePathEdit->text();

    // Замораживаем кнопки
    ui->bootTriggerButton->setEnabled(false);
    ui->filePathEdit->setEnabled(false);
    ui->searchPath->setEnabled(false);
    ui->flashButton->setEnabled(false);
    ui->pageSizeSpin->setEnabled(false);
    ui->getStatus->setEnabled(false);
    ui->comSelector->setEnabled(false);

    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, [=]() {
        // обработка ответа
        // Размораживаем кнопки
        ui->bootTriggerButton->setEnabled(true);
        ui->filePathEdit->setEnabled(true);
        ui->searchPath->setEnabled(true);
        ui->flashButton->setEnabled(true);
        ui->pageSizeSpin->setEnabled(true);
        ui->getStatus->setEnabled(true);
        ui->comSelector->setEnabled(true);

        emit infoInfo("Flashed!", "Elapsed time: " + QDateTime::fromMSecsSinceEpoch(timer->elapsed()).toString("mm:ss"));

        // важно!
        delete timer;
        delete watcher;
    });
    // запуск в отдельном потоке
    QFuture <void> future = QtConcurrent::run(
        flasher,
        &Flasher::send_flash_from_file,
        hexFilePath,
        ui->pageSizeSpin->value()
    );
    watcher->setFuture(future);

}

void MainWindow::on_bootTriggerButton_clicked(bool checked)
{
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();
    connect(watcher, &QFutureWatcher<bool>::finished, [=]() {
        // обработка ответа
        if (!watcher->future().result()) {
            ui->bootTriggerButton->setChecked(false);
        }
        // важно!
        delete watcher;
    });

    // проверка находимся ли мы в режиме бута. Сейчас только так(
    //connect(watcher, &QFutureWatcher<bool>::destroyed, this, &MainWindow::on_getStatus_clicked);

    // Запуск в другом потоке
    QFuture <bool> future;
    if (checked) {
        future = QtConcurrent::run(
            flasher,
            &Flasher::go_boot,
            0x01
        );
    } else {
        future = QtConcurrent::run(
            flasher,
            &Flasher::leave_boot
        );
    }
    watcher->setFuture(future);
}

void MainWindow::on_getStatus_clicked()
{
    QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
    connect(watcher, &QFutureWatcher<QString>::finished, [=]() {
        // обработка ответа
        QString info = watcher->future().result();

        ui->bootTriggerButton->setEnabled(true);
        if (info == "no connect") {
            ui->bootTriggerButton->setEnabled(false);
            return;
        }
        if (info != "no data") {
            ui->bootTriggerButton->setChecked(true);
        } else {
            ui->bootTriggerButton->setChecked(false);
        }

        // важно!
        delete watcher;
    });

    // Запуск в другом потоке
    QFuture <QString> future = QtConcurrent::run(
        flasher,
        &Flasher::get_status,
        true
    );
    watcher->setFuture(future);
}

void MainWindow::changeProgress(int percent)
{
    if (percent >= 0) {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(percent);
        //qDebug() << "Progress: " << percent;

    } else {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(0);
        //qDebug() << "Progress: (infinite)";
    }
}

void MainWindow::infoCritical(QString title, QString info)
{
    QMessageBox::critical(this, title, info);
}

void MainWindow::infoWarning(QString title, QString info)
{
    QMessageBox::warning(this, title, info);
}

void MainWindow::infoInfo(QString title, QString info)
{
    QMessageBox::information(this, title, info);
}

void MainWindow::infoDebug(QString info)
{
    qDebug() << info;
}

void MainWindow::on_comSelector_currentIndexChanged(int index)
{
    flasher->setPortName(
        ui->comSelector->itemData(index).toList()[0].toString()
    );
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName;
        this->set_flash_file_url(fileName);
        break;
    }
}
