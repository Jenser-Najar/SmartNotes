#pragma once

#include <QObject>
#include <QDateTime>
#include <QVariant>
#include <QList>

class DatabaseManager;
class CallbackData;

class NotesRepository : public QObject
{
    Q_OBJECT
public:
    explicit NotesRepository(QObject *parent = nullptr);
    QString saveNote(const QString &content, const QString &existingNoteId = QString());
    QString loadNoteContent(const QString &noteId) const;
    QVariantMap findReminderByNoteId(const QString &noteId) const;
    QString findNoteIdByReminderId(const QString &reminderId) const;
    void updateExistingReminder(const QString &reminderId, const QString &command, const QDateTime &remindAt);
    void updateExistingNote(const QString &noteId, const QString &content, const QString &phoneNumber);
    void markReminderOverdue(const QString &reminderId);
    QVariantList searchNotes(const QString &query) const;
    QList<CallbackData> loadPendingCallbacks() const;
    int pendingRemindersCount() const;
    void completeReminder(const QString &reminderId);
    void snoozeReminder(const QString &reminderId, int minutes);
    CallbackData nextPendingReminder() const;
    void deleteReminder(const QString &reminderId);
    void deleteRemindersByNoteId(const QString &noteId);

private:
    QString parseLeadName(const QString &content) const;
    QString parsePhoneNumber(const QString &content) const;
    DatabaseManager *m_dbManager = nullptr;
};
