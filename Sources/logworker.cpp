#include "logworker.h"
#include <QtWidgets>

LogWorker::LogWorker(QObject *parent, const QString &logPath) : QObject(parent)
{
    this->logSeek = 0;
    this->logNumLine = 0;
    this->logPath = logPath;
}

LogWorker::~LogWorker()
{

}


void LogWorker::readLog()
{
    QFile logFile(logPath);
    if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Cannot open log...", Error);
        emit pLog(tr("Log: ERROR:Cannot open log..."));
        return;
    }

    logFile.seek(logSeek);

    char line[5000];
    int lineLenght;

    while((lineLenght = logFile.readLine(line, sizeof(line))) > 0)
    {
        logSeek += lineLenght;
        if((line[lineLenght-1] != '\n') &&
                lineLenght<((int)sizeof(line)-1))
        {
            //Leida linea a medias.
            continue;
        }

        emit newLogLineRead(QString(line), ++logNumLine);

        emit seekChanged(logSeek);
    }

    logFile.close();
}


void LogWorker::resetSeek()
{
    logSeek = 0;
    logNumLine = 0;
}

