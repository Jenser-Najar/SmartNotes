#include "NotesEditor.h"
#include "CallbackData.h"
#include "NoteParser.h"
#include <QFont>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextOption>
#include <QVBoxLayout>
#include <QTextBlock>
#include <QVariantMap>

NotesEditor::NotesEditor(QWidget *parent)
    : QWidget(parent)
{
    m_editor = new QPlainTextEdit(this);
    m_editor->setPlaceholderText(tr("Write notes here. Use 'callback 2 horas' or 'callback tomorrow 10am' to create reminders."));
    m_editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_editor->setTabChangesFocus(false);
    m_editor->setFont(QFont("Segoe UI", 14));
    m_editor->setFrameStyle(QFrame::NoFrame);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(m_editor, 1);

    connect(m_editor, &QPlainTextEdit::textChanged, this, &NotesEditor::onTextChanged);
}

QString NotesEditor::text() const
{
    return m_editor->toPlainText();
}

void NotesEditor::setText(const QString &text)
{
    m_editor->setPlainText(text);
}

void NotesEditor::onTextChanged()
{
    emit textChanged();
}

void NotesEditor::appendReminderNoteBlock(const CallbackData &reminderInfo)
{
    // Append the original callback command as a unique reminder entry
    const QString command = reminderInfo.originalCommand();
    if (!command.isEmpty()) {
        QTextCursor cursor(m_editor->textCursor());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText("\n\nReminder activated: " + command);
        m_editor->setTextCursor(cursor);
        m_editor->ensureCursorVisible();
    }
}
