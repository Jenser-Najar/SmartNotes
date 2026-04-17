# Smart Notes

Minimal Qt Widgets note-taking app with smart callback reminders.

## Build steps (Windows)

1. Install Qt 6 with Qt Creator or MSVC toolchain.
2. Open a Qt command prompt or use a terminal with Qt tools in PATH.
3. In the workspace root:

   ```sh
   mkdir build
   cd build
   cmake -G "NMake Makefiles" ..
   cmake --build .
   ```

4. Run `smartnotes.exe` from the `build` folder.

## Features

- Plain text note editor
- Callback parser for commands like `callback 2 horas`, `callback tomorrow 10am`
- SQLite storage for notes and reminders
- Active callback list in the sidebar
- Lightweight dark Qt Widgets UI
- Local reminder popup with open/snooze/done actions
