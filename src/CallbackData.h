#pragma once

#include <QString>
#include <QDateTime>

class CallbackData
{
public:
    CallbackData() = default;
    CallbackData(const QString &id, const QString &noteId, const QString &leadName,
                 const QString &phoneNumber, const QString &content,
                 const QString &originalCommand, const QDateTime &remindAt,
                 const QString &status)
        : m_id(id), m_noteId(noteId), m_leadName(leadName), m_phoneNumber(phoneNumber),
          m_content(content), m_originalCommand(originalCommand), m_remindAt(remindAt),
          m_status(status) {}

    QString id() const { return m_id; }
    QString noteId() const { return m_noteId; }
    QString leadName() const { return m_leadName; }
    QString phoneNumber() const { return m_phoneNumber; }
    QString content() const { return m_content; }
    QString originalCommand() const { return m_originalCommand; }
    QDateTime remindAt() const { return m_remindAt; }
    QString status() const { return m_status; }

    void setStatus(const QString &status) { m_status = status; }
    void setRemindAt(const QDateTime &remindAt) { m_remindAt = remindAt; }

private:
    QString m_id;
    QString m_noteId;
    QString m_leadName;
    QString m_phoneNumber;
    QString m_content;
    QString m_originalCommand;
    QDateTime m_remindAt;
    QString m_status;
};