#pragma once

#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    QSqlDatabase database() const;
    bool initializeDatabase();
    QString databasePath() const;

private:
    QSqlDatabase m_database;
    QString m_databasePath;
    bool ensureSchema();
};
