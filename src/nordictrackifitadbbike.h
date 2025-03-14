#ifndef NORDICTRACKIFITADBBIKE_H
#define NORDICTRACKIFITADBBIKE_H

#include <QtCore/qbytearray.h>

#ifndef Q_OS_ANDROID
#include <QtCore/qcoreapplication.h>
#else
#include <QtGui/qguiapplication.h>
#endif
#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtimer.h>

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QThread>
#include <QUdpSocket>

#include "bike.h"
#include "virtualbike.h"

#ifdef Q_OS_IOS
#include "ios/lockscreen.h"
#endif

class nordictrackifitadbbikeLogcatAdbThread : public QThread {
    Q_OBJECT

  public:
    explicit nordictrackifitadbbikeLogcatAdbThread(QString s);    
    bool runCommand(QString command);

    void run() override;

  signals:
    void onSpeedInclination(double speed, double inclination);
    void debug(QString message);
    void onWatt(double watt);
    void onHRM(int hrm);

  private:
    QString adbCommandPending = "";
    QString runAdbCommand(QString command);
    double speed = 0;
    double inclination = 0;
    double watt = 0;
    int hrm = 0;
    QString name;
    struct adbfile {
        QDateTime date;
        QString name;
    };

    void runAdbTailCommand(QString command);
};

class nordictrackifitadbbike : public bike {
    Q_OBJECT
  public:
    nordictrackifitadbbike(bool noWriteResistance, bool noHeartService, uint8_t bikeResistanceOffset,
                           double bikeResistanceGain);
    bool connected() override;
    resistance_t pelotonToBikeResistance(int pelotonResistance) override;
    bool inclinationAvailableByHardware() override;

  private:
    void forceResistance(double resistance);
    uint16_t watts() override;
    double getDouble(QString v);

    QTimer *refresh;

    uint8_t sec1Update = 0;
    QDateTime lastRefreshCharacteristicChanged = QDateTime::currentDateTime();
    QDateTime lastInclinationChanged = QDateTime::currentDateTime();
    uint8_t firstStateChanged = 0;
    uint16_t m_watts = 0;

    bool initDone = false;
    bool initRequest = false;

    bool noWriteResistance = false;
    bool noHeartService = false;

    bool gearsAvailable = false;

    QUdpSocket *socket = nullptr;
    QHostAddress lastSender;

    nordictrackifitadbbikeLogcatAdbThread *logcatAdbThread = nullptr;

    QString lastCommand;

    QString ip;

#ifdef Q_OS_IOS
    lockscreen *h = 0;
#endif

  signals:
    void disconnected();
    void debug(QString string);

  private slots:

    void processPendingDatagrams();
    void changeInclinationRequested(double grade, double percentage);
    void onHRM(int hrm);

    void update();
};

#endif // NORDICTRACKIFITADBBIKE_H
