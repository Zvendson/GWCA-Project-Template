# GWCA-Project-Template

### **Requirements**
+ CMake v3.19 or higher. Download the latest version from https://cmake.org/download/.
+ Visual Studio. You can download Visual Studio Community for free here: https://visualstudio.microsoft.com/vs/community/. You will also need the "Desktop development with C++" package.
+ Git. https://git-scm.com/

### **Build**
1. Navigate to your favourit development directory. E.g.: `D:\dev\cpp\gw\`
2. Open `Git Bash` by rightclicking in the empty space.
3. Clone repo with: ```git clone https://github.com/Zvendson/GWCA-Project-Template```
4. Navigate to the new folder: `cd GWCA-Project-Template`
5. Build Project: ```cmake -A Win32 -B build -DProjectName=YourProjectName```<br>
(No spaces, no quotes)
6. Open the project: ```cmake --open```
7. Build the project: ```cmake --build build```
8. Build release with: ```cmake --build build --config Release```

Have fun.