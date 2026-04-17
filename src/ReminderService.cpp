#include "ReminderService.h"
#include "NotesRepository.h"
#include "CallbackData.h"
#include <QApplication>
#include <QDateTime>
#include <QDebug>

ReminderService::ReminderService(NotesRepository *repository, QObject *parent)
    : QObject(parent)
    , m_repository(repository)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ReminderService::checkNextReminder);

    m_soundTimer = new QTimer(this);
    m_soundTimer->setInterval(300); // gentle beeps every 300ms
    connect(m_soundTimer, &QTimer::timeout, this, &ReminderService::playReminderSound);

    scheduleNextReminder();
}

void ReminderService::scheduleNextReminder()
{
    const CallbackData nextReminder = m_repository->nextPendingReminder();
    if (nextReminder.id().isEmpty()) {
        m_timer->stop();
        return;
    }

    const QDateTime remindAt = nextReminder.remindAt();
    if (!remindAt.isValid()) {
        m_timer->stop();
        return;
    }

    qint64 msecs = QDateTime::currentDateTime().msecsTo(remindAt);
    if (msecs < 0) {
        msecs = 0;
    }
    m_timer->start(qMax<qint64>(1000, msecs));
}

void ReminderService::checkNextReminder()
{
    const QDateTime now = QDateTime::currentDateTime();

    // Process all due reminders
    while (true) {
        const CallbackData nextReminder = m_repository->nextPendingReminder();
        if (nextReminder.id().isEmpty()) {
            break; // No more reminders
        }

        const QString reminderId = nextReminder.id();
        const QDateTime remindAt = nextReminder.remindAt();
        if (!remindAt.isValid()) {
            break;
        }

        if (remindAt > now) {
            break; // Next reminder is in the future
        }

        // Prevent duplicate triggers for the same reminder within 60 seconds
        if (!reminderId.isEmpty() && reminderId == m_lastTriggeredId && m_lastTriggerTime.secsTo(now) < 60) {
            break;
        }

        m_lastTriggeredId = reminderId;
        m_lastTriggerTime = now;
        emit reminderDue(nextReminder);
        m_soundStartTime = now;
        m_soundTimer->start();
        // Reminder is deleted in onReminderDue
    }

    // Schedule the next future reminder
    scheduleNextReminder();
}

void ReminderService::refreshSchedule()
{
    scheduleNextReminder();
}

void ReminderService::playReminderSound()
{
    QApplication::beep();
    if (m_soundStartTime.secsTo(QDateTime::currentDateTime()) >= 2.5) {
        m_soundTimer->stop();
    }
}
