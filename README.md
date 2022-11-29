# BackgroundMusic
Allow certain DirectSound games to play music in the background.

This was intended for, and tested with, the WebTV 2.5 viewer, but may work with other games.

## Usage
Put BackgroundMusic.dll into the same directory as the executable, and setdll somewhere where you can call it (same directory may be easiest)
Then in the command line: `.\setdll /d:BackgroundMusic.dll .\yourprogram.exe`
