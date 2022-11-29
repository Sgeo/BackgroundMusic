# BackgroundMusic
Allow certain DirectSound games to play music in the background.

This was intended for, and tested with, the WebTV 2.5 viewer, but may work with other games.

## Usage
Put BackgroundMusic.dll into the same directory as the executable, and setdll somewhere where you can call it (same directory may be easiest)
Then in the command line: `.\setdll /d:BackgroundMusic.dll .\yourprogram.exe`

## Building
* Detours must be built somewhere.
* From an x86 native tools, adjusting paths to the Detours files as needed:
`cl D:\detours-4.0.1\lib.X86\detours.lib dsound.lib main.cpp /LD /I D:\detours-4.0.1\include /link /export:DetourFinishHelperProcess,@1,NONAME`
