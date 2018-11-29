#include "SpeedTester.h"
#include "RemixLog.h"
#include "AppSession.h"
#include <QDebug>
#include <QTimer>

const int WaitTimeOut = 10000;
const QStringList defaultUrls = (QStringList()<<"www.baidu.com"<<"www.qq.com"<<"www.taobao.com");

void SpeedTesterPrivate::doSpeedTest()
{
    int result = 999;
    bool ok;
    QProcess cmd;

#ifdef Q_OS_WIN
    QString cmdline("ping ");
    QRegExp rx("(\\d+)ms\r\n");
    int capIndex = 1;
#elif defined(Q_OS_MAC)
    QString cmdline("ping -c 4 ");
    QRegExp rx("(\\d+\\.?\\d+)/(\\d+\\.?\\d+)/(\\d+\\.?\\d+)/(\\d+\\.?\\d+) ms");
    int capIndex = 2;
#endif

    foreach (QString url, _targetUrls)
    {
        cmd.start(cmdline + url);
        cmd.waitForFinished(WaitTimeOut);
        QString out = QString::fromLocal8Bit(cmd.readAllStandardOutput());

        if(rx.indexIn(out) > -1)
        {
            result = rx.cap(capIndex).toFloat(&ok);
            if(ok && result < 100)
                break;
        }
        else
        {
            qDebug()<< QString("doSpeedTest():connect " + url + " failed:" + out);
            LogWarning(QString("doSpeedTest():connect " + url + " failed:" + out));
        }

    }
    if(result > 999)
        result = 999;

    emit speedTestResult(result);
}

void SpeedTesterPrivate::setTargetUrls(const QStringList &list)
{
    if (!list.isEmpty())
    {
        _targetUrls = list;
    }
}

SpeedTester::SpeedTester(QObject *parent) : QObject(parent)
{
    _tester = new SpeedTesterPrivate;
    _tester->moveToThread(&_workerThread);
    _tester->setTargetUrls(getTargetUrls(defaultUrls));

    connect(this, &SpeedTester::operate, _tester, &SpeedTesterPrivate::doSpeedTest);
    connect(&_workerThread, &QThread::finished, _tester, &QObject::deleteLater);
    connect(_tester, &SpeedTesterPrivate::speedTestResult, this, &SpeedTester::handleResult);
    connect(this, &SpeedTester::targetUrlsChanged, _tester, &SpeedTesterPrivate::setTargetUrls);

    _workerThread.start();
}

SpeedTester::~SpeedTester()
{
    _workerThread.quit();
    _workerThread.wait();
}

SpeedTester& SpeedTester::getInstance()
{
    static SpeedTester instance;
    return instance;
}

void SpeedTester::getNetworkDelayInMs(QObject* obj, const std::function<void(int)> &func, int testInterval, bool isContinuous)
{
    _testInterval = testInterval;
    _isContinueSpeedTest = isContinuous;
    connect(_tester,  &SpeedTesterPrivate::speedTestResult, obj, func);
    emit operate();
    LogInfo(QString("getNetworkDelayInMs()::Interval:%1ms, isContinuous:%2").arg(testInterval).arg(isContinuous));
}

void SpeedTester::startSpeedTest(bool isContinuous)
{
    _isContinueSpeedTest = isContinuous;
    emit operate();
}

void SpeedTester::stopSpeedTest()
{
    _isContinueSpeedTest = false;
}

void SpeedTester::handleResult(int delayMilliseconds)
{
    emit speedTestResult(delayMilliseconds);
    if(_isContinueSpeedTest)
    {
        QTimer::singleShot(_testInterval, this, [this]
        {
            emit operate();
        });
    }
}

QStringList SpeedTester::getTargetUrls(const QStringList &defaultUrls)
{
    QVariant urls = AppSession::Instance()->getOption("SpeedTestTargetUrls", "TargetUrls");
    if(!urls.canConvert<QStringList>())
    {
        if(!defaultUrls.isEmpty())
        {
            AppSession::Instance()->setOption("SpeedTestTargetUrls", "TargetUrls", defaultUrls);
            return defaultUrls;
        }
    }
    else
    {
        return urls.value<QStringList>();
    }
}

void SpeedTester::setTargetUrls(const QStringList &list)
{
    if (!list.isEmpty())
    {
        AppSession::Instance()->setOption("SpeedTestTargetUrls", "TargetUrls", list);
        emit targetUrlsChanged(list);
    }
}
