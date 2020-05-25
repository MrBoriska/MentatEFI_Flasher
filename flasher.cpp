#include "flasher.h"
#include <QDebug>

#include <QThread>
#include <QtSerialPort/QSerialPortInfo>

Flasher::Flasher(QObject *parent) : QObject(parent)
{
    port_name = "";
    serial = 0;
    protocol_type = OLD_PROTO;
}

/**
 * @brief Открывает последовательное соединение с портом
 * @param port_name
 * @return
 */
bool Flasher::openSerialPort(QString port_name)
{
    this->serial = new QSerialPort();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if (info.portName() == port_name) {
            serial->setPort(info);

            serial->setBaudRate(QSerialPort::Baud57600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            //serial->setRequestToSend(true);

            if (serial->open(QIODevice::ReadWrite)) {
                //success connected

                qDebug() << "serial connected";
                return true;
            } else {
                emit infoCritical(tr("Error"), serial->errorString());
                qDebug() << "serial not connected";
                return false;
            }
            break;
        }
    }

    emit infoCritical(tr("Error"), "COM port not found!");
    qDebug() << "serial not found";

    return false;
}

/**
 * @brief Закрывает последовательное соединение
 */
void Flasher::closeSerialPort()
{
    if (serial != 0 && serial->isOpen()) {
        serial->close();
        delete serial;
        serial = 0;
        qDebug() << "serial disconnected";
    }
}

void Flasher::setPortName(QString port_name) {
    this->port_name = port_name;
}

void Flasher::setProtocol(PROTOCOLS proto)
{
    this->protocol_type = proto;
}

/**
 * @brief Осуществляет запись прошивки в ЭБУ из файла
 * @param file
 */
void Flasher::send_flash_from_file(QString hexFilePath, int page_size) {

    emit changeProgress(0);
    this->closeSerialPort();
    if (!this->openSerialPort(this->port_name)) {
        emit changeProgress(0);
        return;
    }

    // Очистка старой прошивки
    if (!this->erase_chip()) return;

    // Открытие файла прошивки
    QFile file(hexFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        emit infoCritical(tr("Error"), file.errorString());
        this->closeSerialPort();
        return;
    }
    qint64 hexfilesize = file.size();

    W2B data;
    uint16_t addressW = 0x0000;
    qint64 readed = 0;
    qint64 step_bytes = 1;
    uint16_t ck1=0, ck2=0;
    QByteArray temp;
    bool is_lsb = false;
    int current_progress = 0;
    int page_count = 0;

    while (!file.atEnd()) {
        QDebug debug = qDebug();
        // Сначала пропускаем спец симовол начала строки
        if (step_bytes == 1) {
            temp = file.read(step_bytes);
            readed += step_bytes;
            if (temp.data()[0] != ':') {
                continue;
            }
            step_bytes = 2;
        }
        // Потом шагаем по два символа, считывая байтики
        if (step_bytes == 2) {
            // length of data in line
            int data_len = file.read(step_bytes).toInt(Q_NULLPTR, 16); readed += step_bytes;

            // data address (not using...)
            file.read(4);
            readed += 4;

            // type of data
            temp = file.read(2);
            readed += 2;

            // data record type
            if (QString(temp) == "00") {
                int i = 0;
                while (i < data_len) {
                    if (!is_lsb) {
                        data.b[0] = file.read(2).toUInt(Q_NULLPTR, 16);
                        readed += 2;
                    } else {
                        data.b[1] = file.read(2).toUInt(Q_NULLPTR, 16);
                        readed += 2;

                        if (page_count == 0) {
                            // todo: тут можно безопасно остановить процесс прошивки
                            // или повторно осуществить запись страницы,
                            // например из-за несовпадения чек-сумм

                            ck1 = ck2 = addressW;

                            if (!send_address(addressW)) {
                                emit infoCritical(tr("Error"), "Ошибка при отправке адреса страницы!");

                                file.close();
                                this->closeSerialPort();
                                return;
                            }

                            // init write page
                            char command[] = {'D','k','k'};

                            debug << QByteArray(command,3).toHex() << "\n";

                            #ifndef DEBUG_MODE
                            serial->write(command, 3);
                            serial->waitForBytesWritten(10000);
                            #endif
                        }

                        // send bytes to EFI
                        char rev_data[] = {char(data.b[0]),char(data.b[1])};

                        debug << QByteArray(rev_data,2).toHex();

                        #ifndef DEBUG_MODE
                        serial->write(rev_data, 2);
                        serial->waitForBytesWritten(10000);
                        #endif

                        ck1 += data.w;
                        ck2 ^= data.w;

                        addressW += 1;

                        if (page_count == page_size-1) {
                            debug << "\n";
                            debug = qDebug();

                            if (!send_checksumm(ck1,ck2)) {
                                emit infoCritical(tr("Error"), "Ошибка при проверке контрольной суммы!");

                                file.close();
                                this->closeSerialPort();
                                return; // todo: необходимо осуществлять повторные несколько попыток отправки страницы
                            }

                            page_count = 0;

                        } else {
                            page_count++;
                        }
                    }

                    is_lsb = !is_lsb;
                    i++;
                }

                // skip std cksum ans endline symbols
                step_bytes = 1;
            }
            // end of file record type
            else if (QString(temp) == "01") {
                // fill page
                while (page_count < page_size) {
                    data.b[0] = 0xFF;
                    data.b[1] = 0xFF;

                    debug << QByteArray((char *)data.b,2).toHex();

                    #ifndef DEBUG_MODE
                    serial->write((char *)data.b, 2);
                    serial->waitForBytesWritten(10000);
                    #endif

                    ck1 += data.w;
                    ck2 ^= data.w;
                    page_count++;
                }

                debug << "\n";
                debug = qDebug();

                if (!send_checksumm(ck1,ck2)) {
                    emit infoCritical(tr("Error"), "Ошибка при проверке контрольной суммы!");

                    file.close();
                    this->closeSerialPort();
                    return; // todo: необходимо осуществлять повторные несколько попыток отправки страницы
                }
            }
            // address record type
            else if (QString(temp) == "02") {

                if (page_count != 0) {
                    emit infoWarning(tr("Warning"), "Предыдущая страница еще не окончена!");
                    //todo: возможно, нужно заполнить остаток страницы 0xFF
                }

                // data address
                uint16_t addr_offset = file.read(4).toUInt(Q_NULLPTR, 16);
                readed += 4;
                addressW = (addr_offset << 4)/2;

                // skip std cksum ans endline symbols
                step_bytes = 1;
            }
            else {
                qDebug() << "Unsupported record type: " << QString(temp) << ". Ignoring!";

                file.read(data_len*2).toUInt(Q_NULLPTR, 16);

                // skip std cksum ans endline symbols
                step_bytes = 1;
            }

        }
        if (current_progress != int(100*readed/hexfilesize)) {
            emit changeProgress(current_progress = 100*readed/hexfilesize);
        }
    }


    emit changeProgress(100);
    file.close();
    this->closeSerialPort();
}

/**
 * @brief Отправляет адрес загружаемой страницы памяти ЭБУ
 * @param addressW
 * @return
 */
bool Flasher::send_address(uint16_t addressW) {

    QDebug debug = qDebug();
    debug << "send address...";

    W2B addr;
    addr.w = addressW;

    char data[] = {0x41, char(addr.b[1]), char(addr.b[0])};

    #ifdef DEBUG_MODE
        debug << QByteArray(data,3).toHex();
        return true;
    #endif

    serial->write(data, 3);
    serial->waitForBytesWritten(10000);
    debug << "...";

    while (serial->waitForReadyRead(1000)) {
        debug << "..";
        QByteArray recv = serial->readAll();
        if (recv.size() && recv.at(recv.size()-1) == 0x0d) {
            debug << "success";
            return true;
        }
    }
    debug << "error with input buffer: " << serial->readAll();

    return false;
}

/**
 * @brief Отправляет чек-суммы и ждет ответа от загрузчика о их правильности
 * @param ck1
 * @param ck2
 * @return
 */
bool Flasher::send_checksumm(uint16_t ck1, uint16_t ck2) {

    qDebug() << "send checksumm...";

    char data[] = {char((ck1 >> 8)),
                   char((ck1 & 0xFF)),
                   char((ck2 >> 8)),
                   char((ck2 & 0xFF))};

    #ifdef DEBUG_MODE
        qDebug() << QByteArray(data,4).toHex();
        return true;
    #endif

    serial->write(data, 4);
    serial->waitForBytesWritten(10000);
    qDebug() << "msg sent...";

    QByteArray recv;
    while (serial->waitForReadyRead(1000)) {
        qDebug() << "...";
        recv.append(serial->readAll());

        //uint8_t rck1 = recv.mid(0,1).toUInt(Q_NULLPTR, 16);
        //uint8_t rck2 = recv.mid(1,1).toUInt(Q_NULLPTR, 16);
        //uint8_t rck3 = recv.mid(2,1).toUInt(Q_NULLPTR, 16);
        //uint8_t rck4 = recv.mid(3,1).toUInt(Q_NULLPTR, 16);

        //if (rck1 != (ck1 >> 8)) return false;
        //if (rck2 != (ck1 & 0xFF)) return false;
        //if (rck3 != (ck2 >> 8)) return false;
        //if (rck4 != (ck2 & 0xFF)) return false;

        if (recv.size() && recv.at(recv.size()-1) == 0x0d) {
            qDebug() << "checksumm right!";
            return true;
        } else if (recv.size() && recv.at(recv.size()-1) == 'N') {
            qDebug() << "checksumm FAIL!";
            return false;
        }
    }

    qDebug() << "invalid responce: " << serial->readAll();
    return false;
}

/**
 * @brief Входит в режим прошивки
 * @return
 */
bool Flasher::go_boot(int mode) {

    qDebug() << "go boot mode...";

    emit changeProgress(-1);

    this->closeSerialPort();
    if (!this->openSerialPort(this->port_name)) {
        emit changeProgress(0);
        return false;
    }

    if (this->protocol_type == CAN_STYLE_PROTO) {
        qDebug() << "syncing...";
        char data_[] = {'F'};
        serial->write((char*)data_,1);
        serial->waitForBytesWritten(10000);
        while(serial->waitForReadyRead(1000));
        qDebug() << serial->readAll();

        qDebug() << "send command...";
        // table 0x01 - config table
        // table_offset 0x19 - options field in config table
        //               {      size, type, CANID, table, tbl_offset,   tbl_size, mode,                  crc32}
        uint8_t data[] = {0x00, 0x08, 0x77,  0x00,  0x01, 0x00, 0x19, 0x00, 0x01, 0x01, 0x4b, 0xe6, 0x8d, 0x87};
        serial->write((char*)data,14);
        serial->waitForBytesWritten(10000);
    } else {
        char data[] = {0x4F, 0x00, 0x19, 0x01};
        if (mode == 0x01 || mode == 0x02)
            data[3] = mode;
        serial->write((char*)data,4);
        serial->waitForBytesWritten(10000);
    }

    qDebug() << "msg sent...";

    // На всякий ждем ответ от мк (не дольше 1000мс)
    // Нужно на случай, если входящий буфер не пуст.
    while(serial->waitForReadyRead(1000));
    qDebug() << "clean input buffer: " << serial->readAll() << " ok";

    // Проверка статуса (в бут режиме или нет)
    QString ret = this->get_status(false);

    this->closeSerialPort();
    emit changeProgress(0);
    return (ret != "no data");
}

/**
 * @brief Выходит из режима прошивки
 * @return
 */
bool Flasher::leave_boot() {

    qDebug() << "leaving boot mode...";

    emit changeProgress(-1);

    this->closeSerialPort();
    if (!this->openSerialPort(this->port_name)) {
        emit changeProgress(0);
        return false;
    }

    char data[] = {0x45};
    serial->write((char*)data,1);
    serial->waitForBytesWritten(10000);
    qDebug() << "msg sent...";

    // Ждем выхода
    QByteArray recv;
    while (serial->waitForReadyRead(1000)) {
        qDebug() << "...";
        recv.append(serial->readAll());
        if (recv.size() && recv.at(recv.size()-1) == 0x0d) {
            qDebug() << "success " << recv;
            this->closeSerialPort();
            emit changeProgress(0);
            return true;
        }
    }
    qDebug() << "failed!";
    emit infoCritical(tr("Error"), "Failed to leave boot mode!");

    this->closeSerialPort();
    emit changeProgress(0);
    return false;
}

/**
 * @brief Очищает прошивку ЭБУ
 * @return
 */
bool Flasher::erase_chip() {

    qDebug() << "erasing chip...";

    emit changeProgress(-1);

    char data[] = {0x65};

    #ifdef DEBUG_MODE
        return true;
    #endif

    serial->write((char*)data,1);
    serial->waitForBytesWritten(10000);
    qDebug() << "msg sent...";

    // Ждем очистки...
    while (serial->waitForReadyRead(10000)) {
        qDebug() << "...";
        QByteArray recv = serial->readAll();
        if (recv.size() && recv.at(recv.size()-1) == 0x0d) {
            qDebug() << "success!";
            emit changeProgress(0);
            return true;
        }
    }
    qDebug() << "error " << serial->readAll();
    emit changeProgress(0);
    return false;
}

/**
 * @brief Flasher::get_status
 * @return
 */
QString Flasher::get_status(bool open_serial) {

    qDebug() << "getting status...";

    emit changeProgress(-1);

    if (open_serial) {
        this->closeSerialPort();
        if (!this->openSerialPort(this->port_name)) {
            emit changeProgress(0);
            return "no connect";
        }
    }

    char data[] = {0x53, 0x0d}; // специально добавлен ненужный символ в конец,
                                // чтобы мк ругнулся на него последним символом в ответе
    serial->write((char*)data,2);
    serial->waitForBytesWritten(10000);
    qDebug() << "msg sent...";

    QByteArray recv;
    while (serial->waitForReadyRead(2000)) {
        qDebug() << "...";
        recv.append(serial->readAll());
        if (recv.size() && recv.at(recv.size()-1) == '?') { // этим символом мк ругается
                                                            // и заодно говорит, что закончил отправку :)
            recv.remove(recv.size()-1,1); // удалим его из вывода
            qDebug() << "success! " << recv;
            if (open_serial) this->closeSerialPort();
            emit changeProgress(0);
            return recv;
        }
    }
    if (recv.size() > 0) {
        qDebug() << "recieve without end symbol: " << recv;
    }

    qDebug() << "no data!";
    if (open_serial) this->closeSerialPort();
    emit changeProgress(0);
    return "no data";
}
