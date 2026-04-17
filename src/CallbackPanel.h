#pragma once

#include <QWidget>

class QTableWidget;

class CallbackPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CallbackPanel(QWidget *parent = nullptr);
    void loadCallbacks();

private slots:
    void deleteSelectedCallback();

private:
    QTableWidget *m_table = nullptr;
};
