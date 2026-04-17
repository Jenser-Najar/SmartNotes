#pragma once

#include <QWidget>
#include <QVariant>

class CallbackData;
class QPlainTextEdit;

class NotesEditor : public QWidget
{
    Q_OBJECT
public:
    explicit NotesEditor(QWidget *parent = nullptr);
    QString text() const;
    void setText(const QString &text);
    void appendReminderNoteBlock(const CallbackData &reminderInfo);

signals:
    void textChanged();

private slots:
    void onTextChanged();

private:
    QPlainTextEdit *m_editor = nullptr;
};
