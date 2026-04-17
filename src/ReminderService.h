#pragma once

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QVariant>

class NotesRepository;
class CallbackData;

class ReminderService : public QObject
{
    Q_OBJECT
public:
    explicit ReminderService(NotesRepository *repository, QObject *parent = nullptr);
    Q_SLOT void refreshSchedule();

signals:
    void reminderDue(const CallbackData &reminderInfo);

private slots:
    void checkNextReminder();
    void playReminderSound();

private:
    NotesRepository *m_repository = nullptr;
    QTimer *m_timer = nullptr;
    QTimer *m_soundTimer = nullptr;
    QString m_lastTriggeredId;
    QDateTime m_lastTriggerTime;
    QDateTime m_soundStartTime;
    void scheduleNextReminder();
};
