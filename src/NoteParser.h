#pragma once

#include <QDateTime>
#include <QString>
#include <QList>

struct CallbackInfo
{
    QDateTime reminderDateTime;
    QString originalCommand;
    QString parseStatus;
    QString parseError;
};

struct NoteParseResult
{
    bool hasCallback = false;
    QList<CallbackInfo> callbacks;
};

class NoteParser
{
public:
    static NoteParseResult parseNoteContent(const QString &content);
    static CallbackInfo parseCallbackCommand(const QString &line);
};
