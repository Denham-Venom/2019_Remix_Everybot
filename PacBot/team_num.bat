
@rem set CMD_LINE_ARGS=%*

@ rem echo %* > test.txt
@copy .wpilib\wpilib_preferences.json .wpilib\wpilib_preferences.json.old 

@echo { > .wpilib\wpilib_preferences.json
@echo ^"enableCppIntellisense^": true,>> .wpilib\wpilib_preferences.json
@echo ^"currentLanguage^": ^"cpp^",>> .wpilib\wpilib_preferences.json
@echo ^"projectYear^": ^"2019^",>> .wpilib\wpilib_preferences.json
@echo ^"teamNumber^": %1>> .wpilib\wpilib_preferences.json
@echo } >> .wpilib\wpilib_preferences.json

