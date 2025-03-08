@echo off

echo.
echo Creating project files with all engines enabled (except specified ones)
echo.

create_project ..\.. --enable-all-engines --msvc --enable-fluidlite --disable-engine=titanic,wintermute,ags,glk,tsage,ultima,mm,sword25,hpl1

echo.
echo Project files created successfully.
echo.

pause
