# How to run

In the root directory
Run: python npc_server.py
Then open main.exe to run

Command to compile it again: 
cl /EHsc /std:c++17 main.cpp ^
  /Iexternal\httplib ^
  /Iexternal\nlohmann ^
  /Iexternal\SFML-3.0.0\include ^
  /link ^
  /LIBPATH:external\SFML-3.0.0\lib ^
  sfml-graphics.lib sfml-window.lib sfml-system.lib ws2_32.lib

# How to play
You can move the player with arrow keys.
Type in text and hit enter to send a message to an NPC, you will see the text appear above the player as you type, the NPC will respond with a message above them.

# About the app
A chatbot feature designed with C++ SFML and Python Flask using Open AI gpt-oss-20b to give non player characters (NPCs) advanced dialogue options, making them able to respond to the player in real time with new and unique responses to what the player types/asks the NPC.

Issues:
Special characters may not show correctly because of the font. (They look fine when printing to console). Currently the AI has been told not to use special characters and seems to be complying.
Sometimes text wraps when it does not need to.

Features to consider:
Multiple NPCs.
Basic fallback dialogue for if the server cannot connect.
Shopkeeper that can sell items but only if the user as enough money.

# Things to note
My laptop is not powerful enough to run gpt-oss-20b locally, so will use huggingface to use it remotely for free with a limited number of requests. The API key needs to be hidden so it can not be misused.

I have MSVC installed on my computer at "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.31.31103\\bin\\Hostx64\\x64\\cl.exe"
to be able to compile it.

# Learning materials
For Python backend AI
https://huggingface.co/openai/gpt-oss-20b

For frontend C++ SFML code
https://www.sfml-dev.org/documentation/3.0.0/