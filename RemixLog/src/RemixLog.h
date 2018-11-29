#ifndef REMIXLOG_H
#define REMIXLOG_H

#include <QThread>
#include <QString>
#include <QFile>
#include <QList>

#define LogInfo(log) RemixLog::getInstance()->writeLog((log), RemixLog::LogLevel::log_info)
#define LogWarning(log) RemixLog::getInstance()->writeLog((log), RemixLog::LogLevel::log_warning)
#define LogError(log) RemixLog::getInstance()->writeLog((log), RemixLog::LogLevel::log_error)
#define LogDebug(log) RemixLog::getInstance()->writeLog((log), RemixLog::LogLevel::log_debug)
#define LogOther(log) RemixLog::getInstance()->writeLog((log), RemixLog::LogLevel::log_other)

#define LogFlush() RemixLog::getInstance()->flush()

#define LogFolder "./logs/"
#define LogFileFormat "yyyyMMdd.hhmmss"
#define LogLineTimeFormat "yyyy-MM-dd hh:mm:ss.zzz"



class RemixLog : public QThread
{
    Q_OBJECT

public:
    enum LogLevel
    {
        log_info = 1,
        log_warning,
        log_error,
        log_debug,
        log_other
    };

private:
    RemixLog(QObject *parent = 0);
    static RemixLog * _instance;

public:
    ~RemixLog();
    static RemixLog * getInstance();

public:
    void writeLog(QString content, LogLevel level = log_info);
    void flush();

    void run();
    void stop();

private:
    inline QString getFullFileName();
    inline void doWrite(QList<QString> &logLines);
    inline QString combilePath(QString path1, QString path2);

private:
    QString _filePath;
    QList<QString> _logLines;

    bool _stop;
};

#endif // REMIXLOG_H
