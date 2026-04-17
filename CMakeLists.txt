cmake_minimum_required(VERSION 3.16)
project(SmartNotes VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ThemeManager.cpp
    src/DatabaseManager.cpp
    src/NotesRepository.cpp
    src/NoteParser.cpp
    src/ReminderService.cpp
    src/ReminderPopup.cpp
    src/HamburgerMenu.cpp
    src/NotesEditor.cpp
    src/CallbackPanel.cpp
    src/CallbackData.cpp
)

qt6_add_resources(RESOURCE_FILES resources/resources.qrc)

set(HEADERS
    src/MainWindow.h
    src/ThemeManager.h
    src/DatabaseManager.h
    src/NotesRepository.h
    src/NoteParser.h
    src/ReminderService.h
    src/ReminderPopup.h
    src/HamburgerMenu.h
    src/NotesEditor.h
    src/CallbackPanel.h
    src/CallbackData.h
)

add_executable(smartnotes ${SOURCES} ${HEADERS} ${RESOURCE_FILES})

target_link_libraries(smartnotes PRIVATE Qt6::Widgets Qt6::Sql)

if(WIN32)
    set_target_properties(smartnotes PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
