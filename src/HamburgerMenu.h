#pragma once

#include <QPushButton>

class QMenu;

class HamburgerMenu : public QPushButton
{
    Q_OBJECT
public:
    explicit HamburgerMenu(QWidget *parent = nullptr);

signals:
    void sectionChanged(const QString &section);

private:
    QMenu *m_menu = nullptr;
};
