@echo off

forfiles /s /m *.dll /c "cmd /c echo @path && pdb2mdb @path"