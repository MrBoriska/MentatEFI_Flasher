#ifndef FLASHER_H
#define FLASHER_H

#include <QObject>
#include <QFile>
#include <QSerialPort>
#include <stdint.h>

#include "config.h"
// simplify the bytes to words translation
typedef union {
    uint16_t w;
    uint8_t b[2];
}W2B;

class Flasher : public QObject
{
    Q_OBJECT
public:
    explicit Flasher(QObject *parent = 0);

    QSerialPort *serial;

    void setPortName(QString port_name);

public slots:
    bool go_boot(int mode);
    bool leave_boot();
    QString get_status(bool open_serial = true);
    void send_flash_from_file(QString hexFilePath, int page_size);

signals:
    void changeProgress(int percent);
    void finished();
    void infoCritical(QString title, QString info);
    void infoWarning(QString title, QString info);
    void infoInfo(QString title, QString info);
    void infoDebug(QString info);

private:
    QString port_name;

    bool send_address(uint16_t addressW);
    bool erase_chip();
    bool send_checksumm(uint16_t ck1, uint16_t ck2);
    bool openSerialPort(QString port_name);
    void closeSerialPort();

};

#endif // FLASHER_H
