# Steam Achievement Tracker (WIP)

This is a simple program that can be used to get fully customizable notifications for your steam achievements.<br>
If you want, you can use this already, however it is expected that there are countless bugs. If you find bugs, i would
appreciate any report (Just open an issue).

## Building

Since this is still wip, there aren't any releases yet, but you can build it yourself if you want to try it already.

### Requirements:

- [MSVC Compiler + Libraries](https://aka.ms/vs/17/release/vs_buildtools.exe). This might also work with other
  compilers, but i use those, so they will work best. Select the workload "Desktop Development with C++" in the
  installer.
- [Git](https://github.com/git-for-windows/git/releases/download/v2.52.0.windows.1/Git-2.52.0-64-bit.exe)

### Build Process:

- Open a commandline and run the following commands (replace <...> variables accordingly, see file explorer if unsure):
    - Powershell:
      ```powershell
      $build_dir = "${env:userprofile}\Downloads\steam-achievement-tracker" 
      git clone --recursive "https://github.com/LunaticWasTaken/steam-achievement-tracker" "${build_dir}"; cd "${build_dir}"
      .\build.bat
      ```
    - Command Prompt:
      ```cmd
      set "build_dir=%userprofile%\Downloads\steam-achievement-tracker"
      git clone --recursive "https://github.com/LunaticWasTaken/steam-achievement-tracker" "%build_dir%" 66 cd "%build_dir%"
      .\build.bat
      ```
- Afterward, you can find a release at ``<build_dir>\build\release\release-win.zip``

## Usage:

- Extract the built release whereever you want.
- Run the included ``setup_dependencies.bat`` to install QT dependencies. It doesn't take long. Read at the end of the
  readme to see why they aren't included.
- Run the executable. If you want it to autostart with windows (recommended), press ctrl-c on the exe-file, go to
  ``C:\Users\<username>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup``, Right-Click and click
  ``Paste shortcut``. It will then autostart whenever Windows starts up.
- Once running, you will see a cmd window with the log. This can be minimized, just don't close it. It will be removed
  in future versions, so the app isn't too intrusive.
- At startup, the app will check for your steam install path, Parse required files, download icons etc. Once This is
  done, it will tell you by a notification (Defaults to the bottom right of your primary screen).
- From there on, everything is automatic. Play your games, and if you unlock an achievement, you will get a
  notification. That's it.

## Themes:

- The app allows you to completely customize the looks of the Notification via custom themes.
- To create a theme, go to ``C:\Users\<username>\.config\steam-achievement-tracker\themes`` and create a new folder with
  the name of your theme. Choose a name that can be easily typed, preferably without spaces or special characters.
- In that folder, create a ``theme.yml`` file. In that file, you can customize tons of things. See below for the current
  specification of the file.
- To use that theme now, open ``.config\steam-achievement-tracker\config.yml`` (create it if it doesn't exist), and set
  ``active_theme`` to the same name you gave the folder. e.g:

````yaml 
active_theme: MyCustomTheme
````

- After starting/restarting the app, it will use that theme.

### Current Spec (With default values):

````yaml
theme:
  # Settings for the root overlay (the banner itself)
  root:
    # The anchor. one of: [topleft, topright, bottomleft, bottomright] - Case-Insensitive.
    anchor: bottomright
    # The X-offset to apply to the anchor. Positive=Right, Negative=Left
    offset_x: 0
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up
    offset_y: 0
    # The Width.
    width: 282
    # The Height
    height: 70
    # The background of the overlay. Can be a raw color (e.g. "white"), an rgb/rgba string, or even a url to a file.
    # See https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/background for reference.
    # NOTE: This expose a "{THEME_DIR}" variable that automatically expands to your theme dir. Useful if using files.
    background: rgba(25,25,25,1.0)
    # The corner-rounding. [0 > 255]
    rounding: 0
  # Settings for the icon.
  icon:
    # If the icon should be enabled or not.
    # If disabled, anything below is ignored.
    enabled: true
    # The anchor. See above
    anchor: topleft
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up    
    offset_x: 11
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up
    offset_y: 11
    # The Width.
    width: 48
    # The Height
    height: 48
    # The corner-rounding. [0 > 255]
    rounding: 0
  title:
    # If the title should be enabled or not.
    # If disabled, anything below is ignored.
    enabled: true
    # The anchor. See above
    anchor: topleft
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up    
    offset_x: 68
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up
    offset_y: 12
    # The font to use. Should be either the name of a file installed on the os (e.g. "Arial") or the path to a ttf-file.
    # NOTE: This expose a "{THEME_DIR}" variable that automatically expands to your theme dir. Useful if using files.
    font: Arial
    # If the title should be bold
    font_bold: true
    # If the title should be italic
    font_italic: false
    # The font size
    font_size: 10
    # The font color
    font_color: white
    # If the text should wrap to the next line when no space is left.
    font_wrapping: false
  description:
    # If the title should be enabled or not.
    # If disabled, anything below is ignored.
    enabled: true
    # The anchor. See above
    anchor: topleft
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up    
    offset_x: 68
    # The Y-offset to apply to the anchor. Positive=Down, Negative=Up
    offset_y: 28
    # The font to use. Should be either the name of a file installed on the os (e.g. "Arial") or the path to a ttf-file.
    # NOTE: This expose a "{THEME_DIR}" variable that automatically expands to your theme dir. Useful if using files.
    font: Arial
    # If the title should be bold
    font_bold: false
    # If the title should be italic
    font_italic: false
    # The font size
    font_size: 10
    # The font color
    font_color: white
    # If the text should wrap to the next line when no space is left.
    font_wrapping: true
  misc:
    # If you are crazy and the above isn't enough for your styling-needs, you can use this to manually apply any QSS (QT Stylesheet) rules to the overlay.
    # For reference, the tree is formed like this:
    # - Invisible Fullscreen Overlay (No selector, this should not be changed)
    #   - Root (#root)
    #     - Icon (#icon)
    #     - Title (#title)
    #     - Description (#description)
    #
    # NOTE: While QSS uses the same syntax as css, not all rules are supported (e.g. Animations being a big no-no).
    # See https://doc.qt.io/qt-6/stylesheet-reference.html#list-of-properties for a reference.
    # Since YML is awesome, you can use a neat multiline string here. Example:
    # raw_qss: |
    #   root {
    #     name: value;
    #     ...
    #   }
    #   ...
    raw_qss: """
````

For any questions regarding this spec, just ask away!

### FAQ

- Why is QT not simply included?
    - TLDR: Licensing issues.
    - Long version: QT is licensed under either GPL or LGPL. Including it would require me to release the source code of
      the QT version i use, for as long as this app is distributed. <br>
      While in theory that is easy, in practice it a lot of flaws:
        - If i include the source in the release, you suddenly have a 1gb package attached to my 30MB package, since the
          QT source code is hillariously big.
        - If i post it on github, I have to 1: create multiple repositories, and B:
          I am breaking the license as soon as e.g. my github account gets terminated.
        - If i link to the official QT repository/website, i am breaking the license as soon as they take it down, or
          e.g their servers break
        - If i store the source on my own PC, i break the license as soon as e.g. my house burns down.
        - If i store it on some webserver, i have to pay tons of money.

      It would end up being a "Let's hope it goes fine" situation, since this is not a "Best effort" thing. Sorry, but
      no. <br>
      If you want to replace the QT libraries, you totally can. You can also just get the source code of QT yourself,
      it's not hard: https://github.com/qt/qtbase/tree/6.10.1.
    - My License also clearly states that i don't impact your freedom in any way. I literally cannot give you more
      freedom than i already do.
- Help, i get double achievements
    - You likely still get the achievement notifications from steam. If you don't want them as well, you need to disable
      steam banner notifications entirely, because steam is dumb. Go to
      ``Steam > Settings > Notifications > Show Toast Notifications`` and set it to ``Never``.
      ``When not in-game`` from what i know does not seem to work.
- I don't get any Notifications at all
    - Check the log for any warnings or errors.
    - If you used Steam Achievement Manager to lock achievements again, you need to restart Steam afterward, otherwise
      it still keeps them cached as unlocked.
- I get an error "failed to find Qt6Something.dll" when starting the app
    - Run the ``setup_dependencies.bat`` script before starting the program.

### LICENSE

Lunatic Code License

Copyright (c) 2025 Lunatic

You can use my code however you want, no restrictions. Giving me credit is appreciated, but not required.