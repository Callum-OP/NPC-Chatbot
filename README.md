# How to run

Open main.exe to run

Command to compile: cl /EHsc /std:c++17 /I external/SFML-3.0.0/include main.cpp /link /LIBPATH:external/SFML-3.0.0/lib sfml-graphics.lib sfml-window.lib sfml-system.lib

cl /EHsc /std:c++17 main.cpp ^
  /Iexternal\httplib ^
  /Iexternal\nlohmann ^
  /Iexternal\SFML-3.0.0\include ^
  /link ^
  /LIBPATH:external\SFML-3.0.0\lib ^
  sfml-graphics.lib sfml-window.lib sfml-system.lib ws2_32.lib

