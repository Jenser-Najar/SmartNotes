#pragma once

#include <QMainWindow>
#include <QShortcut>
#include <QVariant>

class HamburgerMenu;
class NotesEditor;
class CallbackPanel;
class NotesRepository;
class ReminderService;
class ReminderPopup;
class CallbackData;

class QLabel;
class QLineEdit;
class QStackedWidget;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSectionActivated(const QString &section);
    void onSearchTextChanged(const QString &text);
    void onEditorTextChanged();
    void onReminderDue(const CallbackData &reminderInfo);
    void onReminderActionOpen();
    void onReminderActionSnooze();
    void onReminderActionDone();
    void refreshCallbackPanel();
    void refreshStatusBar();

private:
    void setupUi();
    void setupConnections();
    void loadRecentNotes();
    void saveCurrentNote();
    void updateReminderIndicator();
    void showToastMessage(const QString &text);

    HamburgerMenu *m_menu = nullptr;
    NotesEditor *m_editor = nullptr;
    CallbackPanel *m_callbackPanel = nullptr;
    NotesRepository *m_repository = nullptr;
    ReminderService *m_reminderService = nullptr;
    ReminderPopup *m_popup = nullptr;
    QStackedWidget *m_mainStack = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_indicatorLabel = nullptr;
    QShortcut *m_saveShortcut = nullptr;
    QString m_currentNoteId;
};
