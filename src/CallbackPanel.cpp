#include "CallbackPanel.h"
#include "NotesRepository.h"
#include "CallbackData.h"
#include <QDateTime>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

CallbackPanel::CallbackPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(4);

    auto *title = new QLabel(tr("Active callbacks"), this);
    title->setObjectName("PanelTitle");
    layout->addWidget(title);

    auto *buttonLayout = new QHBoxLayout();
    auto *deleteButton = new QPushButton(tr("Delete Selected"), this);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({tr("Lead"), tr("Phone"), tr("When"), tr("Status")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setObjectName("CallbackTable");
    layout->addWidget(m_table, 1);
    connect(deleteButton, &QPushButton::clicked, this, &CallbackPanel::deleteSelectedCallback);
}

void CallbackPanel::loadCallbacks()
{
    NotesRepository repository(this);
    const QList<CallbackData> callbacks = repository.loadPendingCallbacks();
    m_table->clearContents();
    m_table->setRowCount(callbacks.count());

    for (int row = 0; row < callbacks.count(); ++row) {
        const CallbackData &callback = callbacks.at(row);
        auto *leadItem = new QTableWidgetItem(callback.leadName());
        auto *phoneItem = new QTableWidgetItem(callback.phoneNumber());
        auto *timeItem = new QTableWidgetItem(callback.remindAt().toLocalTime().toString("MMM d hh:mm ap"));
        auto *statusItem = new QTableWidgetItem(callback.status());

        m_table->setItem(row, 0, leadItem);
        m_table->setItem(row, 1, phoneItem);
        m_table->setItem(row, 2, timeItem);
        m_table->setItem(row, 3, statusItem);
    }
}

void CallbackPanel::deleteSelectedCallback()
{
    int row = m_table->currentRow();
    if (row < 0) return;

    NotesRepository repository(this);
    const QList<CallbackData> callbacks = repository.loadPendingCallbacks();
    if (row >= callbacks.count()) return;

    const CallbackData &callback = callbacks.at(row);
    const QString reminderId = callback.id();
    if (!reminderId.isEmpty()) {
        repository.deleteReminder(reminderId);
        loadCallbacks(); // refresh
    }
}
