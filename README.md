# How to run

## Prerequisites
- Ensure you use Windows
- Ensure you have Python installed (I used and tested this with version 3.11.9).
- (Optional) MSVC compiler only if you want to rebuild the C++ frontend
- Ensure you have a Hugging Face account, and from it a free API key that you can place it in a file in the project root directory titled: .env

## Steps
1. Open a terminal in the project root directory.
2. Type this command to install Python dependencies: 
    pip install -r requirements.txt
3. Type this command to start the backend server:
    Run: python npc_server.py
4. Simply open main.exe to play

# How to play
Move the player with arrow keys.
Type text using your keyboard and press Enter to send a message to a nearby NPC.
Your text appears above the player and NPC replies will appear above them.

# About the app
## Overview
C++ SFML frontend for rendering and player interaction.
Python Flask backend for handling AI responses.
OpenAI gpt-oss-20b model (via Hugging Face) for generating NPC dialogue.

Could be used in a game to give non player characters infinite dialogue options while staying in character.

## Issues
Special characters may not show correctly because of the font. Currently the AI has been told not to use special characters.
Sometimes text wraps when it does not need to, an issue with the frontend.

## Future features
Basic fallback dialogue for if the server cannot connect.
Shopkeeper that can sell items but only if the user as enough money.
Save NPC histories to database so that NPCs will not forget past conversations if the backend server is reset.

## Things to note
My laptop is not powerful enough to run gpt-oss-20b locally, I used huggingface and an API key to use it remotely for free with a limited number of requests. 
You must keep your API key hidden to prevent misuse.

To compile and create main.exe I used MSVC and have it installed on my computer at "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.31.31103\\bin\\Hostx64\\x64\\cl.exe".

Command if you want to compile it again using MSVC: 
cl /EHsc /std:c++17 main.cpp ^
  /Iexternal\httplib ^
  /Iexternal\nlohmann ^
  /Iexternal\SFML-3.0.0\include ^
  /link ^
  /LIBPATH:external\SFML-3.0.0\lib ^
  sfml-graphics.lib sfml-window.lib sfml-system.lib ws2_32.lib

  I ran that command in the x64 Native Tools Command Prompt for VS 2022 in the project root directory.

# Learning materials
For Python backend AI
https://huggingface.co/openai/gpt-oss-20b

For frontend C++ SFML code
https://www.sfml-dev.org/documentation/3.0.0/