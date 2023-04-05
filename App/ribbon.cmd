call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
uicc ribbon.xml ribbon.bml /header:ids.h /res:ribbonui.rc

move ids.h src\
move ribbon.bml src\
move ribbonui.rc src\

pause
