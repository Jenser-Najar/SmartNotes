#pragma once

#include <QDialog>
#include <QVariant>

class CallbackData;
class QLabel;
class QPushButton;

class ReminderPopup : public QDialog
{
    Q_OBJECT
public:
    explicit ReminderPopup(QWidget *parent = nullptr);
    void showReminder(const CallbackData &reminderInfo);
    QString currentReminderId() const;

signals:
    void openRequested();
    void snoozeRequested();
    void doneRequested();

private:
    QString m_reminderId;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_previewLabel = nullptr;
    QLabel *m_whenLabel = nullptr;
    QPushButton *m_openButton = nullptr;
    QPushButton *m_snoozeButton = nullptr;
    QPushButton *m_doneButton = nullptr;
};
