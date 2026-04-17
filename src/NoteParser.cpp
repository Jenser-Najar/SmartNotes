#include "NoteParser.h"
#include <QObject>
#include <QRegularExpression>
#include <QTime>

static bool parseTimeUnits(const QString &input, int &hours, int &minutes)
{
    hours = 0;
    minutes = 0;
    auto text = input.toLower();
    const QRegularExpression hourRx(R"((\d+)\s*(h|hora|horas))");
    const QRegularExpression minuteRx(R"((\d+)\s*(m|min|mins|minuto|minutos))");

    auto hourMatch = hourRx.globalMatch(text);
    while (hourMatch.hasNext()) {
        const auto match = hourMatch.next();
        hours += match.captured(1).toInt();
    }
    auto minuteMatch = minuteRx.globalMatch(text);
    while (minuteMatch.hasNext()) {
        const auto match = minuteMatch.next();
        minutes += match.captured(1).toInt();
    }
    return hours != 0 || minutes != 0;
}

CallbackInfo NoteParser::parseCallbackCommand(const QString &line)
{
    CallbackInfo result;
    const QString trimmed = line.trimmed();
    if (!trimmed.toLower().startsWith("callback")) {
        result.parseStatus = "no_command";
        return result;
    }

    result.originalCommand = trimmed;
    const QString remainder = trimmed.mid(QString("callback").length()).trimmed();
    if (remainder.isEmpty()) {
        result.parseStatus = "missing_time";
        result.parseError = QObject::tr("No delay or time found");
        return result;
    }

    const QRegularExpression tomorrowRx(R"((mañana|mañ|tomorrow)\s+(\d{1,2})(?::(\d{2}))?\s*(am|pm)?)", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression timeRx(R"((\d{1,2}):(\d{2})\s*(am|pm)?)", QRegularExpression::CaseInsensitiveOption);

    QDateTime now = QDateTime::currentDateTime();
    QDateTime resultDate = now;
    int hours = 0;
    int minutes = 0;

    auto tomorrowMatch = tomorrowRx.match(remainder);
    if (tomorrowMatch.hasMatch()) {
        const QString hourText = tomorrowMatch.captured(2);
        const QString minuteText = tomorrowMatch.captured(3);
        const QString ampm = tomorrowMatch.captured(4).toLower();
        int hour = hourText.toInt();
        int minute = minuteText.isEmpty() ? 0 : minuteText.toInt();
        if (ampm == "pm" && hour < 12) hour += 12;
        if (ampm == "am" && hour == 12) hour = 0;
        resultDate = now.addDays(1);
        resultDate.setTime(QTime(hour, minute));
        result.parseStatus = "parsed_tomorrow";
        result.reminderDateTime = resultDate;
        return result;
    }

    auto timeMatch = timeRx.match(remainder);
    if (timeMatch.hasMatch()) {
        int hour = timeMatch.captured(1).toInt();
        int minute = timeMatch.captured(2).toInt();
        const QString ampm = timeMatch.captured(3).toLower();
        if (ampm == "pm" && hour < 12) hour += 12;
        if (ampm == "am" && hour == 12) hour = 0;
        resultDate.setTime(QTime(hour, minute));
        if (resultDate < now) {
            resultDate = resultDate.addDays(1);
        }
        result.parseStatus = "parsed_clock_time";
        result.reminderDateTime = resultDate;
        return result;
    }

    if (parseTimeUnits(remainder, hours, minutes)) {
        resultDate = now.addSecs(hours * 3600 + minutes * 60);
        result.parseStatus = "parsed_duration";
        result.reminderDateTime = resultDate;
        return result;
    }

    result.parseStatus = "invalid_time";
    result.parseError = QObject::tr("Unable to parse callback time");
    return result;
}

NoteParseResult NoteParser::parseNoteContent(const QString &content)
{
    NoteParseResult result;
    const QString lowerContent = content.toLower();
    int searchPos = 0;

    while (true) {
        int callbackPos = lowerContent.indexOf("callback", searchPos);
        if (callbackPos == -1) {
            break;
        }

        int nextCallbackPos = lowerContent.indexOf("callback", callbackPos + 8);
        int lineEndPos = content.indexOf('\n', callbackPos);
        int endPos = content.length();

        if (nextCallbackPos != -1 && (lineEndPos == -1 || nextCallbackPos < lineEndPos)) {
            endPos = nextCallbackPos;
        } else if (lineEndPos != -1) {
            endPos = lineEndPos;
        }

        QString callbackText = content.mid(callbackPos, endPos - callbackPos).trimmed();
        CallbackInfo callback = parseCallbackCommand(callbackText);
        if (callback.parseStatus != "no_command" && callback.parseStatus != "invalid_time") {
            result.callbacks.append(callback);
            result.hasCallback = true;
        }

        searchPos = endPos;
    }

    return result;
}
