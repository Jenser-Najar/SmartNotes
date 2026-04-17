# Smart Notes

Minimal Qt Widgets note-taking app with smart callback reminders.

## Getting Started

### Cloning the Repository (for New GitHub Users)

If you're new to GitHub and want to download this project to your computer, follow these simple steps:

1. **Install Git**: If you don't have Git installed, download and install it from [git-scm.com](https://git-scm.com/). Follow the instructions for your operating system.

2. **Open a Terminal**: 
   - On Windows: Open Command Prompt or PowerShell.
   - On macOS: Open Terminal.
   - On Linux: Open your terminal application.

3. **Navigate to a Folder**: Choose where you want to save the project. For example, go to your Desktop:
   ```
   cd Desktop
   ```

4. **Find the Repository URL**: Go to the GitHub page for this project (usually something like `https://github.com/username/repo-name`). Click the green "Code" button and copy the URL (it should end with `.git`).

5. **Clone the Repository**: Run this command in your terminal, replacing `<repository-url>` with the URL you copied:
   ```
   git clone <repository-url>
   ```
   For example:
   ```
   git clone https://github.com/jense/Notas-inteligentes.git
   ```

6. **Enter the Project Folder**: After cloning, navigate into the new folder:
   ```
   cd Notas-inteligentes
   ```

Now you have the project on your computer! Continue to the build steps below.

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
