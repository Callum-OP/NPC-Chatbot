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

# Things to note
My laptop is not powerful enough to run gpt-oss-20b locally, so will use huggingface to use it remotely for free with a limited number of requests. The API key needs to be hidden so it can not be misused.

I have MSVC installed on my computer at "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.31.31103\\bin\\Hostx64\\x64\\cl.exe"
to be able to compile it.