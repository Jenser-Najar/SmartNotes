#include "ThemeManager.h"
#include <QApplication>
#include <QPalette>

void ThemeManager::applyDarkTheme(QApplication *app)
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(28, 30, 34));
    palette.setColor(QPalette::WindowText, QColor(235, 235, 235));
    palette.setColor(QPalette::Base, QColor(18, 20, 24));
    palette.setColor(QPalette::AlternateBase, QColor(34, 36, 42));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
    palette.setColor(QPalette::Text, QColor(235, 235, 235));
    palette.setColor(QPalette::Button, QColor(36, 38, 44));
    palette.setColor(QPalette::ButtonText, QColor(235, 235, 235));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(100, 190, 255));
    palette.setColor(QPalette::Highlight, QColor(70, 130, 220));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    app->setPalette(palette);

    app->setStyleSheet(R"(
        QWidget {
            font-family: "Segoe UI", "Arial", sans-serif;
            color: #EBEBEB;
            background-color: #1C1E22;
        }
        QMainWindow {
            background: #1C1E22;
        }
        QLabel#TitleLabel {
            font-size: 18px;
            font-weight: 600;
        }
        QLabel#IndicatorLabel {
            color: #B3C7FF;
            font-size: 12px;
        }
        QTextEdit, QPlainTextEdit {
            background: #14161A;
            border: 1px solid #2F3238;
            border-radius: 10px;
            padding: 12px;
            color: #F4F4F4;
            font-size: 14px;
        }
        QListWidget, QTableWidget {
            background: #1F2126;
            border: 1px solid #2F3238;
            border-radius: 10px;
        }
        QPushButton {
            background: #2F3450;
            border: 1px solid #3E445F;
            border-radius: 8px;
            padding: 8px 12px;
            color: #ECEFF4;
        }
        QPushButton:hover {
            background: #3A4160;
        }
        QPushButton:pressed {
            background: #2C324D;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
        }
        QScrollBar::handle:vertical {
            background: #3A3F4D;
            border-radius: 5px;
        }
        QScrollBar::add-line, QScrollBar::sub-line {
            height: 0;
        }
    )");
}
