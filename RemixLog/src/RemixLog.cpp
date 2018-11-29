#include "RemixLog.h"

#include <QTime>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

RemixLog * RemixLog::_instance = new RemixLog;

RemixLog::RemixLog(QObject *parent)
    :QThread(parent)
    ,_stop(false)
{
}

RemixLog::~RemixLog()
{
    _stop = true;
    flush();
}

RemixLog *RemixLog::getInstance()
{
    return _instance;
}

void RemixLog::writeLog(QString content, LogLevel level)
{
    QString log = "[" + QDateTime::currentDateTime().toString(LogLineTimeFormat) + "]";

    switch (level)
    {
    case log_info:
        log += "[info] ";
        break;
    case log_warning:
        log += "[warning] ";
        break;
    case log_error:
        log += "[error] ";
        break;
    case log_debug:
        log += "[debug] ";
        break;
    default:
        break;
    }

    log += content;
    log += "\n";

    _logLines.append(log);
}

void RemixLog::run()
{
    while (!_stop)
    {
        if (_logLines.count() > 0)
        {
            doWrite(_logLines);
        }
        msleep(5);
    }
}

void RemixLog::stop()
{
    _stop = true;
}

QString RemixLog::getFullFileName()
{
    auto root = QCoreApplication::applicationDirPath();
    auto path = combilePath(root, LogFolder);
    QDir dir(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    return combilePath(path, QDateTime::currentDateTime().toString(LogFileFormat) + ".log");
}

void RemixLog::doWrite(QList<QString>& logLines)
{
    if(_filePath.isEmpty())
    {
        _filePath = getFullFileName();
    }

    auto file = new QFile(_filePath);
    if(!file->open(QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    while(logLines.count() > 0)
    {
        auto logLine = logLines.takeFirst();
        file->write(logLine.toUtf8());
    }
    file->flush();
    file->close();
}

QString RemixLog::combilePath(QString path1, QString path2)
{
    if(path1.isEmpty() || path2.isEmpty())
    {
        return path1;
    }
    else if((path1.endsWith('/') || path1.endsWith('\\'))
            && (path2.startsWith('/') || path2.startsWith('\\')))
    {
        return path1 + path2.mid(1);
    }
    else if((path1.endsWith('/') || path1.endsWith('\\'))
            || (path2.startsWith('/') || path2.startsWith('\\')))
    {
        return path1 + path2;
    }
    else
    {
        return path1 + "/" + path2;
    }
}

void RemixLog::flush()
{
    doWrite(_logLines);
}
