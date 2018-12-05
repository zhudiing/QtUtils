#ifndef SPEEDTESTERFACTORY_H
#define SPEEDTESTERFACTORY_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <functional>
#include <QStringList>

class SpeedTester : public QObject
{
    Q_OBJECT
public:
    explicit SpeedTester(const QStringList &targetUrls, int testInterval, bool isContinuous);
public slots:
    void doSpeedTest();
signals:
    void finished();
    void speedTestResult(int delayMilliseconds);
private:
    QStringList _targetUrls;
    int _testInterval{3000};
    bool _isContinuous{false};
};

class SpeedTesterFactory : public QObject
{
    Q_OBJECT
public:
    static void getNetworkDelayInMs(QObject* obj, const std::function<void(int)> &func,
                                    const QStringList &targetUrls = QStringList(),
                                    int testInterval = 3000,
                                    bool isContinuous = true);
private:
    explicit SpeedTesterFactory(QObject *parent = nullptr);
    ~SpeedTesterFactory();
    SpeedTesterFactory(SpeedTesterFactory &) = delete;
    SpeedTesterFactory & operator =(SpeedTesterFactory &) = delete;

    static QStringList getTargetUrls(const QStringList &defaultUrls = QStringList());
    static void setTargetUrls(const QStringList &list);
};

#endif // SPEEDTESTERFACTORY_H
