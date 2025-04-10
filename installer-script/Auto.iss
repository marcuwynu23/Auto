; -- auto_installer.iss --
; This script packages the Auto.exe into an installer and appends the path to the user's environment variable.

[Setup]
AppName=Auto
AppVersion=2.0.2
WizardStyle=modern
DefaultDirName={commonpf}\Auto
DefaultGroupName=Auto
UninstallDisplayIcon={app}\auto.exe
Compression=lzma2
SolidCompression=yes

; Set the OutputDir initially to a temporary directory
OutputBaseFilename=AutoInstaller.2.0.2
OutputDir=D:\Projects\marcuwynu23\Auto\bin
; Set the author name and website (will be shown in the installer)
AppPublisher=Mark Wayne Menorca
AppPublisherURL=https://github.com/marcuwynu23

[Files]
; Add Auto.exe file to the installer
Source: "D:\Projects\marcuwynu23\Auto\bin\auto.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Create a Start Menu shortcut for the application
Name: "{group}\Auto"; Filename: "{app}\auto.exe"

[Registry]
; Append the installation path to the user's PATH environment variable
; The PATH registry key for the current user is located at:
; HKEY_CURRENT_USER\Environment\PATH

[Code]
function GetExeDir: String;
begin
  Result := ExpandConstant('{app}');  // Get the directory of auto.exe
end;

procedure AddToPath;
var
  InstallPath, CurrentPath, NewPath: String;
begin
  InstallPath := ExpandConstant('{app}');  // Get the installation path
  CurrentPath := GetEnv('PATH');           // Get current PATH variable
  if Pos(InstallPath, CurrentPath) = 0 then
  begin
    // Append the installation path to the PATH variable in the registry
    NewPath := CurrentPath + ';' + InstallPath;
    RegWriteStringValue(HKEY_CURRENT_USER, 'Environment', 'PATH', NewPath);
  end;
end;

[Run]
; Run AddToPath function after installation to modify the PATH environment variable
Filename: "{app}\auto.exe"; Parameters: ""; StatusMsg: "Appending Auto Application to PATH..."; AfterInstall: AddToPath;
