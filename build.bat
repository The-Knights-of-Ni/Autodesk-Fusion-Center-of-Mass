@echo off

if "release" == "debug" (
echo debug build

cl /nologo /c /I"C:\Users\Kyler\AppData\Roaming/Autodesk/Autodesk Fusion 360/API/CPP/include" /Zi /W3 /WX- /Od /D WIN32 /D _DEBUG /D _WINDOWS /D _USRDLL /D SIMPLE_EXPORTS /D _WINDLL /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fo"Debug\\" /Fd"Debug\vc120.pdb" /Gd /TP /errorReport:prompt "Center of Mass.cpp"

link "/OUT:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Debug\Center of Mass.dll" /INCREMENTAL:NO "/LIBPATH:C:\Users\Kyler\AppData\Roaming/Autodesk/Autodesk Fusion 360/API/CPP/lib" core.lib fusion.lib cam.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST "/MANIFESTUAC:level='asInvoker' uiAccess='false'" /manifest:embed /DEBUG "/PDB:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Debug\Center of Mass.pdb" /SUBSYSTEM:WINDOWS /TLBID:1 /DYNAMICBASE /NXCOMPAT "/IMPLIB:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Debug\Center of Mass.lib" /MACHINE:X64 /DLL "Debug\Center of Mass.obj"

xcopy /R /Y ".\Debug\Center of Mass.dll" ".\"

) else (

echo release build

cl /c /I"C:\Users\Kyler\AppData\Roaming/Autodesk/Autodesk Fusion 360/API/CPP/include" /Zi /W3 /WX- /O2 /Oi /GL /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D SIMPLE_EXPORTS /D _WINDLL /D _UNICODE /D UNICODE /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Fo"Release\\" /Fd"Release\vc120.pdb" /Gd /TP /errorReport:prompt "Center of Mass.cpp"

link "/OUT:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Release\Center of Mass.dll" /INCREMENTAL:NO "/LIBPATH:C:\Users\Kyler\AppData\Roaming/Autodesk/Autodesk Fusion 360/API/CPP/lib" core.lib fusion.lib cam.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST "/MANIFESTUAC:level='asInvoker' uiAccess='false'" /manifest:embed /DEBUG "/PDB:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Release\Center of Mass.pdb" /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT "/IMPLIB:C:\Users\Kyler\AppData\Roaming\Autodesk\Autodesk Fusion 360\API\AddIns\Center of Mass\Release\Center of Mass.lib" /MACHINE:X64 /DLL "Release\Center of Mass.obj" 

xcopy /R /Y ".\Release\Center of Mass.dll" ".\"
)
