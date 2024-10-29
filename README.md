# Assignment 3
## Info
Name: Gabriel Olivotto    
Email: golivott@uwo.ca  
Student #: 251150929

## Submission
### Exercise1.cpp
This is my source code for the spline tool. Once built I ran the binary as follows: `.\Exercise1.exe 1000 1000`

### Exercise1.gif
This is a clip of my source code running on my machine.

# Windows Enviroment
I used windows to build my code since I was having some graphics driver issues building with WSL2. My setup is as follows:
1. Install MSYS2 (https://www.msys2.org/)
2. Open MSYS2

    a. Install pacman: `pacman -Syu`
    
    b. Install MinGW: `pacman -S mingw-w64-x86_64-toolchain`

    c. Install Freegult: `pacman -S mingw-w64-x86_64-freeglut`

    d. Install Glew: `pacman -S mingw-w64-x86_64-glew`

    e. Install GLFW `pacman -S mingw-w64-x86_64-glfw`

3. Add `C:/msys64/mingw64/bin` to user PATH variable 
4. Add `C:/msys64/mingw64/include/**` to your includePath in editor

### VS Code Setup
I use VS Code as my editor to get that working properly I had to install the C/C++ extension and update my config files as follows:
#### .vscode/task.json
```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe build active file",
            "command": "C:\\msys64\\mingw64\\bin\\g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe",
                "-lfreeglut",
                "-lglew32",
                "-lopengl32",
                "-lglfw3",
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Task generated by Debugger."
        }
    ],
    "version": "2.0.0"
}
```

#### .vscode/c_cpp_properties.json
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include/**"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.22621.0",
            "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x86"
        }
    ],
    "version": 4
}
```