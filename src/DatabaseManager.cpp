#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_databasePath = QDir(dataDir).filePath("smartnotes.db");
    const QString connectionName = QString::number(reinterpret_cast<qintptr>(this));
    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_database.setDatabaseName(m_databasePath);
    initializeDatabase();
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

QString DatabaseManager::databasePath() const
{
    return m_databasePath;
}

bool DatabaseManager::initializeDatabase()
{
    if (!m_database.open()) {
        qWarning() << "Unable to open database:" << m_database.lastError().text();
        return false;
    }

    return ensureSchema();
}

bool DatabaseManager::ensureSchema()
{
    QSqlQuery query(m_database);
    const QString notesTable = R"(
        CREATE TABLE IF NOT EXISTS notes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            content TEXT NOT NULL,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            archived INTEGER NOT NULL DEFAULT 0,
            lead_name TEXT,
            phone_number TEXT
        )
    )";

    const QString remindersTable = R"(
        CREATE TABLE IF NOT EXISTS reminders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            note_id INTEGER NOT NULL,
            original_command TEXT,
            content TEXT,
            remind_at TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT 'pending',
            last_triggered_at TEXT,
            snoozed_until TEXT,
            FOREIGN KEY(note_id) REFERENCES notes(id)
        )
    )";

    if (!query.exec(notesTable)) {
        qWarning() << "Failed to create notes table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(remindersTable)) {
        qWarning() << "Failed to create reminders table:" << query.lastError().text();
        return false;
    }

    // Add content column if it doesn't exist (migration)
    QSqlQuery alterQuery(m_database);
    alterQuery.exec("ALTER TABLE reminders ADD COLUMN content TEXT;");
    // Ignore error if column already exists

    return true;
}
