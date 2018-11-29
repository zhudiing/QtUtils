#ifndef SPEEDTESTER_H
#define SPEEDTESTER_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <functional>

class SpeedTesterPrivate: public QObject
{
    Q_OBJECT
public slots:
    void doSpeedTest();
    void setTargetUrls(const QStringList &list);
signals:
    void speedTestResult(int delayMilliseconds);
private:
    QStringList _targetUrls;
};

class SpeedTester : public QObject
{
    Q_OBJECT
public:
    static SpeedTester& getInstance();
    void getNetworkDelayInMs(QObject* obj, const std::function<void(int)> &func, int testInterval = 3000, bool isContinuous = true);
    void startSpeedTest(bool isContinuous = true);
    void stopSpeedTest();
    QStringList getTargetUrls(const QStringList &defaultUrls = QStringList());
    void setTargetUrls(const QStringList &list);

private:
    explicit SpeedTester(QObject *parent = nullptr);
    ~SpeedTester();
    SpeedTester(SpeedTester &) = delete;
    SpeedTester & operator = (SpeedTester &) = delete;


signals:
    void speedTestResult(int delayMilliseconds);
    void operate();
    void targetUrlsChanged(const QStringList &list);

private slots:
    void handleResult(int delayMilliseconds);

private:
    int _testInterval{3000};
    bool _isContinueSpeedTest{false};
    SpeedTesterPrivate *_tester{nullptr};
    QThread _workerThread;
};

#endif // SPEEDTESTER_H
