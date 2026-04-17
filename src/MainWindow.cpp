#include "MainWindow.h"
#include "HamburgerMenu.h"
#include "NotesEditor.h"
#include "CallbackPanel.h"
#include "NotesRepository.h"
#include "ReminderService.h"
#include "ReminderPopup.h"
#include "DatabaseManager.h"
#include "NoteParser.h"
#include "CallbackData.h"

#include <QApplication>
#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_repository = new NotesRepository(this);
    m_reminderService = new ReminderService(m_repository, this);
    m_popup = new ReminderPopup(this);

    setupUi();
    setupConnections();
    loadRecentNotes();
    updateReminderIndicator();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    m_menu = new HamburgerMenu(this);
    m_editor = new NotesEditor(this);
    m_callbackPanel = new CallbackPanel(this);
    m_mainStack = new QStackedWidget(this);

    m_mainStack->addWidget(m_editor);
    m_mainStack->addWidget(m_callbackPanel);

    auto *titleLabel = new QLabel(tr("Smart Notes"), this);
    titleLabel->setObjectName("TitleLabel");

    auto *reminderIcon = new QLabel(tr("🔔"), this);
    reminderIcon->setObjectName("ReminderIcon");
    m_indicatorLabel = new QLabel(tr("No reminders"), this);
    m_indicatorLabel->setObjectName("IndicatorLabel");
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Search"));
    m_searchEdit->setMaximumWidth(100);
    m_searchEdit->setMinimumWidth(70);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    titleLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    reminderIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_indicatorLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(2, 2, 2, 2);
    headerLayout->setSpacing(4);
    headerLayout->addWidget(m_menu);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_searchEdit);
    headerLayout->addWidget(reminderIcon);
    headerLayout->addWidget(m_indicatorLabel);

    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(2);
    contentLayout->addWidget(m_mainStack, 1);

    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(2, 2, 2, 2);
    rootLayout->setSpacing(2);
    rootLayout->addLayout(headerLayout);
    rootLayout->addLayout(contentLayout, 1);

    m_statusLabel = new QLabel(tr("Ready"), this);
    statusBar()->addWidget(m_statusLabel, 1);

    setWindowTitle(tr("Smart Notes"));
    setMinimumSize(280, 240);
}

void MainWindow::setupConnections()
{
    connect(m_menu, &HamburgerMenu::sectionChanged, this, &MainWindow::onSectionActivated);
    connect(m_reminderService, &ReminderService::reminderDue, this, &MainWindow::onReminderDue);
    connect(m_popup, &ReminderPopup::openRequested, this, &MainWindow::onReminderActionOpen);
    connect(m_popup, &ReminderPopup::snoozeRequested, this, &MainWindow::onReminderActionSnooze);
    connect(m_popup, &ReminderPopup::doneRequested, this, &MainWindow::onReminderActionDone);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(m_editor, &NotesEditor::textChanged, this, &MainWindow::onEditorTextChanged);

    m_saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(m_saveShortcut, &QShortcut::activated, this, &MainWindow::saveCurrentNote);
}

void MainWindow::loadRecentNotes()
{
    m_callbackPanel->loadCallbacks();
}

void MainWindow::saveCurrentNote()
{
    const QString content = m_editor->text();
    if (content.trimmed().isEmpty()) {
        m_statusLabel->setText(tr("Ready"));
        return;
    }

    const QString noteId = m_repository->saveNote(content, m_currentNoteId);
    if (!noteId.isEmpty()) {
        m_currentNoteId = noteId;
        m_statusLabel->setText(tr("Saved %1").arg(QDateTime::currentDateTime().toString("hh:mm ap")));

        // Parse for callbacks after saving
        auto parsed = NoteParser::parseNoteContent(content);
        if (parsed.hasCallback && !parsed.callbacks.isEmpty()) {
            if (parsed.callbacks.size() == 1) {
                showToastMessage(QString("Reminder created for %1").arg(parsed.callbacks.first().reminderDateTime.toString("hh:mm ap")));
            } else {
                showToastMessage(QString("%1 reminders created").arg(parsed.callbacks.size()));
            }
        }
        updateReminderIndicator();
        m_reminderService->refreshSchedule();
    }
}

void MainWindow::updateReminderIndicator()
{
    const int pendingCount = m_repository->pendingRemindersCount();
    QString text = pendingCount > 0 ? tr("%1 callback active").arg(pendingCount) : tr("No callbacks");
    m_indicatorLabel->setText(text);
    refreshCallbackPanel();
}

void MainWindow::onSectionActivated(const QString &section)
{
    if (section == QLatin1String("Callbacks")) {
        m_mainStack->setCurrentWidget(m_callbackPanel);
        refreshCallbackPanel();
        return;
    }

    m_mainStack->setCurrentWidget(m_editor);
    if (section == QLatin1String("Notes")) {
        m_statusLabel->setText(tr("Editing note"));
    }
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    if (text.trimmed().isEmpty()) {
        return;
    }

    const QVariantList matches = m_repository->searchNotes(text);
    if (!matches.isEmpty()) {
        const QVariantMap note = matches.first().toMap();
        m_currentNoteId = note.value("id").toString();
        m_editor->setText(note.value("content").toString());
        m_statusLabel->setText(tr("Search result opened"));
    }
}

void MainWindow::onEditorTextChanged()
{
    if (m_editor->text().trimmed().isEmpty()) {
        m_currentNoteId.clear();
        m_statusLabel->setText(tr("New note"));
    }
}

void MainWindow::onReminderDue(const CallbackData &reminderInfo)
{
    // Show the stored callback snapshot for every due reminder
    const QString noteId = m_repository->findNoteIdByReminderId(reminderInfo.id());
    if (!noteId.isEmpty()) {
        m_currentNoteId = noteId;
    }
    const QString content = m_repository->loadNoteContent(noteId);
    m_editor->setText(content);
    m_mainStack->setCurrentWidget(m_editor);
    m_statusLabel->setText(tr("Reminder opened in notes"));

    // Delete only this specific reminder
    m_repository->deleteReminder(reminderInfo.id());
    updateReminderIndicator();
    showToastMessage(tr("Reminder processed: %1").arg(reminderInfo.leadName()));
}

void MainWindow::onReminderActionOpen()
{
    if (m_popup->currentReminderId().isEmpty()) {
        return;
    }
    const QString noteId = m_repository->findNoteIdByReminderId(m_popup->currentReminderId());
    if (noteId.isEmpty()) {
        return;
    }
    m_currentNoteId = noteId;
    const QString content = m_repository->loadNoteContent(noteId);
    m_editor->setText(content);
    m_mainStack->setCurrentWidget(m_editor);
    m_statusLabel->setText(tr("Opened note"));
}

void MainWindow::onReminderActionSnooze()
{
    if (!m_popup->currentReminderId().isEmpty()) {
        m_repository->snoozeReminder(m_popup->currentReminderId(), 15);
        m_popup->close();
        showToastMessage(tr("Snoozed for 15 minutes"));
    }
    updateReminderIndicator();
}

void MainWindow::onReminderActionDone()
{
    if (!m_popup->currentReminderId().isEmpty()) {
        m_repository->completeReminder(m_popup->currentReminderId());
        m_popup->close();
        showToastMessage(tr("Reminder marked done"));
    }
    updateReminderIndicator();
}

void MainWindow::refreshCallbackPanel()
{
    m_callbackPanel->loadCallbacks();
}

void MainWindow::refreshStatusBar()
{
    updateReminderIndicator();
}

void MainWindow::showToastMessage(const QString &text)
{
    statusBar()->showMessage(text, 4200);
}
