# Basic Web Browser using C
A simple command-line web browser simulation written in C, featuring tab management, browsing history, bookmarks, URL suggestions, and theme switching. This project demonstrates advanced data structures and file-based persistence in C.
## Features
- Tab management (open, switch, and view tabs)
- Browsing history (add, search, clear)
- Bookmarks (add, view)
- URL suggestions based on history
- Theme switching (Default, Dark, Light)
- Simulated web content and external browser launching
- Data persistence via `browser_data.txt`

## How It Works
- The browser uses linked lists, stacks, hash tables, and binary search trees for managing tabs, history, cache, and bookmarks.
- Data is saved and loaded from `browser_data.txt`.
- URLs are opened in your default browser (Edge/Chrome supported on Windows).

## Usage
1. **Compile:**
   ```sh
   gcc webbrowser.c -o webbrowser.exe
   ```
2. **Run:**
   ```sh
   ./webbrowser.exe
   ```
3. **Navigate:**
   Use the menu to enter URLs, switch tabs, manage history/bookmarks, and change themes.

## Menu Options
1. Enter New URL
2. Refresh Page
3. Go Back
4. Go Home
5. New Tab
6. Switch Tab
7. Set Browser
8. Clear History
9. Search History
10. Change Theme
11. Add Bookmark
12. View Bookmarks
13. Exit Browser

## Requirements
- GCC (MinGW recommended for Windows)
- Windows, macOS, or Linux (browser launching supported per OS)

## Notes
- This is a simulated browser; actual web content is not fetched.
- All data is stored locally in `browser_data.txt`.

## Author
Pranav (hyprpranav)
---
Feel free to fork, modify, and contribute!
