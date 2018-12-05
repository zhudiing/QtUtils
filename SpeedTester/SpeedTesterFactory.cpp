#include "SpeedTesterFactory.h"
#include "RemixLog.h"
#include "AppSession.h"
#include <QDebug>
#include <QTimer>

const int WaitTimeOut = 10000;
const QStringList defaultUrls = (QStringList() << "www.baidu.com" << "www.qq.com" << "www.taobao.com");

SpeedTester::SpeedTester(const QStringList &targetUrls, int testInterval, bool isContinuous) :
    _targetUrls(targetUrls),
    _testInterval(testInterval),
    _isContinuous(isContinuous)
{
}

void SpeedTester::doSpeedTest()
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

    foreach(QString url, _targetUrls)
    {
        cmd.start(cmdline + url);
        cmd.waitForFinished(WaitTimeOut);
        QString out = QString::fromLocal8Bit(cmd.readAllStandardOutput());

        if (rx.indexIn(out) > -1)
        {
            result = rx.cap(capIndex).toFloat(&ok);
            if (ok && result < 100)
            {
                break;
            }
        }
        else
        {
            qDebug() << QString("doSpeedTest():connect " + url + " failed:" + out);
            LogWarning(QString("doSpeedTest():connect " + url + " failed:" + out));
        }
    }
    if (result > 999)
    {
        result = 999;
    }

    emit speedTestResult(result);

    if (_isContinuous)
    {
        QTimer::singleShot(_testInterval, this, [this]
            {
                QMetaObject::invokeMethod(this, "doSpeedTest");
            }
            );
    }
    else
    {
        emit finished();
    }
}


SpeedTesterFactory::SpeedTesterFactory(QObject *parent) : QObject(parent)
{
}

void SpeedTesterFactory::getNetworkDelayInMs(QObject* obj, const std::function<void(int)> &func,
                                             const QStringList &targetUrls,
                                             int testInterval,
                                             bool isContinuous)
{
    setTargetUrls(targetUrls);
    QStringList urls = getTargetUrls(defaultUrls);

    QThread *thread = new QThread;
    SpeedTester *tester = new SpeedTester(urls, testInterval, isContinuous);
    tester->moveToThread(thread);

    connect(tester, &SpeedTester::finished, thread, &QThread::quit);
    connect(tester, &SpeedTester::finished, tester, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(tester, &SpeedTester::speedTestResult, obj, func);

    thread->start();
    QMetaObject::invokeMethod(tester, "doSpeedTest");
}

SpeedTesterFactory::~SpeedTesterFactory()
{
}

QStringList SpeedTesterFactory::getTargetUrls(const QStringList &defaultUrls)
{
    QVariant urls = AppSession::Instance()->getOption("SpeedTestTargetUrls", "TargetUrls");

    if (!urls.canConvert<QStringList>())
    {
        if (!defaultUrls.isEmpty())
        {
            AppSession::Instance()->setOption("SpeedTestTargetUrls", "TargetUrls", defaultUrls);
        }
        return defaultUrls;
    }
    else
    {
        return urls.value<QStringList>();
    }
}

void SpeedTesterFactory::setTargetUrls(const QStringList &list)
{
    if (!list.isEmpty())
    {
        AppSession::Instance()->setOption("SpeedTestTargetUrls", "TargetUrls", list);
    }
    else
    {
        AppSession::Instance()->setOption("SpeedTestTargetUrls", "TargetUrls", defaultUrls);
    }
}

