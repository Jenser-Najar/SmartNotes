#include "HamburgerMenu.h"
#include <QMenu>
#include <QAction>

HamburgerMenu::HamburgerMenu(QWidget *parent)
    : QPushButton("☰", parent)
{
    setFixedSize(32, 32);
    setToolTip(tr("Menu"));

    m_menu = new QMenu(this);
    QAction *notesAction = m_menu->addAction(tr("Notes"));
    QAction *callbacksAction = m_menu->addAction(tr("Callbacks"));
    QAction *todayAction = m_menu->addAction(tr("Today"));
    QAction *overdueAction = m_menu->addAction(tr("Overdue"));
    QAction *archiveAction = m_menu->addAction(tr("Archive"));
    QAction *settingsAction = m_menu->addAction(tr("Settings"));

    setMenu(m_menu);

    connect(notesAction, &QAction::triggered, this, [this]() { emit sectionChanged("Notes"); });
    connect(callbacksAction, &QAction::triggered, this, [this]() { emit sectionChanged("Callbacks"); });
    connect(todayAction, &QAction::triggered, this, [this]() { emit sectionChanged("Today"); });
    connect(overdueAction, &QAction::triggered, this, [this]() { emit sectionChanged("Overdue"); });
    connect(archiveAction, &QAction::triggered, this, [this]() { emit sectionChanged("Archive"); });
    connect(settingsAction, &QAction::triggered, this, [this]() { emit sectionChanged("Settings"); });
}
