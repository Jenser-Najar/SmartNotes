#include "NotesRepository.h"
#include "DatabaseManager.h"
#include "NoteParser.h"
#include "CallbackData.h"
#include <QDateTime>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>
#include <QDebug>

NotesRepository::NotesRepository(QObject *parent)
    : QObject(parent)
{
    m_dbManager = new DatabaseManager(this);
}

QString NotesRepository::saveNote(const QString &content, const QString &existingNoteId)
{
    const QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    const QString leadName = parseLeadName(content);
    const QString phoneNumber = parsePhoneNumber(content);
    auto parsed = NoteParser::parseNoteContent(content);

    QString noteId = existingNoteId;

    if (!noteId.isEmpty()) {
        // Update existing note, then resave all callbacks from the current text.
        // This keeps every callback in the current note, without duplicates.
        updateExistingNote(noteId, content, phoneNumber);

        deleteRemindersByNoteId(noteId);
        for (const auto &callback : parsed.callbacks) {
            if (!callback.reminderDateTime.isValid()) {
                continue;
            }
            QSqlQuery reminderInsert(m_dbManager->database());
            reminderInsert.prepare(R"(
                INSERT INTO reminders (note_id, original_command, content, remind_at, status)
                VALUES (:note_id, :original_command, :content, :remind_at, 'pending')
            )");
            reminderInsert.bindValue(":note_id", noteId.toLongLong());
            reminderInsert.bindValue(":original_command", callback.originalCommand);
            reminderInsert.bindValue(":content", content);
            reminderInsert.bindValue(":remind_at", callback.reminderDateTime.toString(Qt::ISODate));
            if (!reminderInsert.exec()) {
                qWarning() << "Failed to save reminder:" << reminderInsert.lastError().text();
            }
        }

        return noteId;
    }


    QSqlQuery insert(m_dbManager->database());
    insert.prepare(R"(
        INSERT INTO notes (content, created_at, updated_at, archived, lead_name, phone_number)
        VALUES (:content, :created_at, :updated_at, 0, :lead_name, :phone_number)
    )");
    insert.bindValue(":content", content);
    insert.bindValue(":created_at", now);
    insert.bindValue(":updated_at", now);
    insert.bindValue(":lead_name", leadName);
    insert.bindValue(":phone_number", phoneNumber);

    if (!insert.exec()) {
        qWarning() << "Failed to save note:" << insert.lastError().text();
        return QString();
    }

    noteId = QString::number(insert.lastInsertId().toLongLong());
    for (const auto &callback : parsed.callbacks) {
        if (callback.reminderDateTime.isValid()) {
            QSqlQuery reminderInsert(m_dbManager->database());
            reminderInsert.prepare(R"(
                INSERT INTO reminders (note_id, original_command, content, remind_at, status)
                VALUES (:note_id, :original_command, :content, :remind_at, 'pending')
            )");
            reminderInsert.bindValue(":note_id", noteId.toLongLong());
            reminderInsert.bindValue(":original_command", callback.originalCommand);
            reminderInsert.bindValue(":content", content);
            reminderInsert.bindValue(":remind_at", callback.reminderDateTime.toString(Qt::ISODate));
            if (!reminderInsert.exec()) {
                qWarning() << "Failed to save reminder:" << reminderInsert.lastError().text();
            }
        }
    }

    return noteId;
}

QVariantMap NotesRepository::findReminderByNoteId(const QString &noteId) const
{
    QVariantMap record;
    if (noteId.trimmed().isEmpty()) {
        return record;
    }

    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT id, note_id, original_command, remind_at, status
        FROM reminders
        WHERE note_id = :note_id
        LIMIT 1
    )");
    query.bindValue(":note_id", noteId.toLongLong());
    if (!query.exec() || !query.next()) {
        return record;
    }

    record["reminder_id"] = query.value(0).toString();
    record["note_id"] = query.value(1).toString();
    record["original_command"] = query.value(2).toString();
    record["remind_at"] = query.value(3).toString();
    record["status"] = query.value(4).toString();
    return record;
}

QString NotesRepository::findNoteIdByReminderId(const QString &reminderId) const
{
    if (reminderId.trimmed().isEmpty()) {
        return QString();
    }

    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT note_id FROM reminders WHERE id = :id LIMIT 1
    )");
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec() || !query.next()) {
        return QString();
    }

    return query.value(0).toString();
}

void NotesRepository::updateExistingNote(const QString &noteId, const QString &content, const QString &phoneNumber)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        UPDATE notes
        SET content = :content,
            updated_at = :updated_at,
            phone_number = :phone_number
        WHERE id = :id
    )");
    query.bindValue(":content", content);
    query.bindValue(":updated_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.bindValue(":phone_number", phoneNumber);
    query.bindValue(":id", noteId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to update existing note:" << query.lastError().text();
    }
}

void NotesRepository::updateExistingReminder(const QString &reminderId, const QString &command, const QDateTime &remindAt)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        UPDATE reminders
        SET original_command = :command,
            remind_at = :remind_at,
            status = 'pending',
            snoozed_until = NULL,
            last_triggered_at = NULL
        WHERE id = :id
    )");
    query.bindValue(":command", command);
    query.bindValue(":remind_at", remindAt.toString(Qt::ISODate));
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to update existing reminder:" << query.lastError().text();
    }
}

void NotesRepository::markReminderOverdue(const QString &reminderId)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        UPDATE reminders
        SET status = 'overdue', last_triggered_at = :triggered_at
        WHERE id = :id
    )");
    query.bindValue(":triggered_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to mark reminder overdue:" << query.lastError().text();
    }
}

QString NotesRepository::loadNoteContent(const QString &noteId) const
{
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT content FROM notes WHERE id = :id LIMIT 1");
    query.bindValue(":id", noteId.toLongLong());
    if (!query.exec() || !query.next()) {
        return QString();
    }
    return query.value(0).toString();
}

QList<CallbackData> NotesRepository::loadPendingCallbacks() const
{
    QList<CallbackData> list;
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT r.id, r.note_id, n.lead_name, n.phone_number, r.content, r.original_command, r.remind_at, r.status, r.snoozed_until
        FROM reminders r
        JOIN notes n ON n.id = r.note_id
        WHERE COALESCE(r.status, 'pending') IN ('pending', 'snoozed', 'overdue')
        ORDER BY r.remind_at ASC
    )");
    if (!query.exec()) {
        qWarning() << "Failed to load callbacks:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        CallbackData callback(
            query.value(0).toString(), // reminder_id
            query.value(1).toString(), // note_id
            query.value(2).toString(), // lead_name
            query.value(3).toString(), // phone_number
            query.value(4).toString(), // content
            query.value(5).toString(), // original_command
            QDateTime::fromString(query.value(6).toString(), Qt::ISODate), // remind_at
            query.value(7).toString() // status
        );
        list.append(callback);
    }
    return list;
}

int NotesRepository::pendingRemindersCount() const
{
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT COUNT(*) FROM reminders WHERE COALESCE(status, 'pending') IN ('pending', 'snoozed', 'overdue')");
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

void NotesRepository::completeReminder(const QString &reminderId)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        UPDATE reminders SET status = 'done', last_triggered_at = :triggered_at
        WHERE id = :id
    )");
    query.bindValue(":triggered_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to complete reminder:" << query.lastError().text();
    }
}

void NotesRepository::snoozeReminder(const QString &reminderId, int minutes)
{
    QDateTime newTime = QDateTime::currentDateTimeUtc().addSecs(minutes * 60);
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        UPDATE reminders SET snoozed_until = :snoozed_until, remind_at = :remind_at
        WHERE id = :id
    )");
    query.bindValue(":snoozed_until", newTime.toString(Qt::ISODate));
    query.bindValue(":remind_at", newTime.toString(Qt::ISODate));
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to snooze reminder:" << query.lastError().text();
    }
}

QVariantList NotesRepository::searchNotes(const QString &queryText) const
{
    QVariantList list;
    if (queryText.trimmed().isEmpty()) {
        return list;
    }

    const QString wildcard = QString("%1").arg(queryText.trimmed());
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT id, content, lead_name, phone_number
        FROM notes
        WHERE content LIKE :query OR lead_name LIKE :query OR phone_number LIKE :query
        ORDER BY updated_at DESC
        LIMIT 10
    )");
    query.bindValue(":query", QString("%%1%").arg(wildcard));
    if (!query.exec()) {
        qWarning() << "Failed to search notes:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        QVariantMap record;
        record["id"] = query.value(0).toString();
        record["content"] = query.value(1).toString();
        record["lead_name"] = query.value(2).toString();
        record["phone_number"] = query.value(3).toString();
        list.append(record);
    }
    return list;
}

CallbackData NotesRepository::nextPendingReminder() const
{
    QSqlQuery query(m_dbManager->database());
    query.prepare(R"(
        SELECT r.id, r.note_id, n.lead_name, n.phone_number, r.content, r.original_command, r.remind_at
        FROM reminders r
        JOIN notes n ON n.id = r.note_id
        WHERE COALESCE(r.status, 'pending') IN ('pending', 'snoozed')
        ORDER BY r.remind_at ASC
        LIMIT 1
    )");
    if (!query.exec() || !query.next()) {
        return CallbackData();
    }
    return CallbackData(
        query.value(0).toString(), // reminder_id
        query.value(1).toString(), // note_id
        query.value(2).toString(), // lead_name
        query.value(3).toString(), // phone_number
        query.value(4).toString(), // content
        query.value(5).toString(), // original_command
        QDateTime::fromString(query.value(6).toString(), Qt::ISODate), // remind_at
        QString() // status not needed here
    );
}

QString NotesRepository::parseLeadName(const QString &content) const
{
    const auto lines = content.split('\n', Qt::SkipEmptyParts);
    const QRegularExpression digitRx("\\d");
    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        if (!trimmed.isEmpty() && !digitRx.match(trimmed).hasMatch()) {
            return trimmed;
        }
    }
    return QString();
}

QString NotesRepository::parsePhoneNumber(const QString &content) const
{
    const auto lines = content.split('\n', Qt::SkipEmptyParts);
    const QRegularExpression nonPhoneRx("[^0-9+\\- ]");
    for (const QString &line : lines) {
        QString digits = line;
        digits.remove(nonPhoneRx);
        if (digits.length() >= 7 && digits.length() <= 20) {
            return digits.trimmed();
        }
    }
    return QString();
}

void NotesRepository::deleteReminder(const QString &reminderId)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare("DELETE FROM reminders WHERE id = :id");
    query.bindValue(":id", reminderId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to delete reminder:" << query.lastError().text();
    }
}

void NotesRepository::deleteRemindersByNoteId(const QString &noteId)
{
    QSqlQuery query(m_dbManager->database());
    query.prepare("DELETE FROM reminders WHERE note_id = :note_id");
    query.bindValue(":note_id", noteId.toLongLong());
    if (!query.exec()) {
        qWarning() << "Failed to delete reminders:" << query.lastError().text();
    }
}
