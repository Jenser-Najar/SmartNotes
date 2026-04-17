#include "ReminderPopup.h"
#include "CallbackData.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

ReminderPopup::ReminderPopup(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint);
    setModal(false);
    setAttribute(Qt::WA_TranslucentBackground);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    m_titleLabel = new QLabel(tr("Callback Reminder"), this);
    m_previewLabel = new QLabel(this);
    m_whenLabel = new QLabel(this);
    m_openButton = new QPushButton(tr("Open"), this);
    m_snoozeButton = new QPushButton(tr("Snooze 15 min"), this);
    m_doneButton = new QPushButton(tr("Done"), this);

    m_previewLabel->setWordWrap(true);
    m_titleLabel->setStyleSheet("font-weight: 600; font-size: 14px;");

    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_previewLabel);
    mainLayout->addWidget(m_whenLabel);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_openButton);
    buttonLayout->addWidget(m_snoozeButton);
    buttonLayout->addWidget(m_doneButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_openButton, &QPushButton::clicked, this, &ReminderPopup::openRequested);
    connect(m_snoozeButton, &QPushButton::clicked, this, &ReminderPopup::snoozeRequested);
    connect(m_doneButton, &QPushButton::clicked, this, &ReminderPopup::doneRequested);
}

QString ReminderPopup::currentReminderId() const
{
    return m_reminderId;
}

void ReminderPopup::showReminder(const CallbackData &reminderInfo)
{
    m_reminderId = reminderInfo.id();
    const QString lead = reminderInfo.leadName();
    const QString phone = reminderInfo.phoneNumber();
    const QString content = reminderInfo.content();
    const QString status = reminderInfo.status();
    const QDateTime remindAt = reminderInfo.remindAt().toLocalTime();

    m_previewLabel->setText(tr("%1 %2\n%3").arg(lead, phone, content.left(120)));
    if (!status.isEmpty()) {
        m_whenLabel->setText(tr("Status: %1").arg(status));
    } else {
        m_whenLabel->setText(tr("Reminder for %1").arg(remindAt.toString("hh:mm ap")));
    }
    adjustSize();
    show();
}
