# Document-Scanner
Document Scanner Using OpenCV . Scanning document from a Picture

Pull the project
Then for libraries needed to make that work follow below steps

Installation of OPENCV Windows
Step1. Go to https://github.com/opencv/opencv and download the Lastest Release
       Extract it Anyweher you want to extract 
       In My case C:\Users\vk560\Documents\
Step2. Add bin folder to the Environment Variables path
       In My case : C:\Users\vk560\Documents\opencv\build\x64\vc15\bin
       Restart Computer
Step3. Clone Repositry using Visual Studio 2019 .
Step4. Set the platform target to x64
       Add Directories by going to Project-Properties-Configuration Properties-
             VC++ Directories
                1. Add Build Directories: D:\opencv\build\include
                2. Add Library Directories: D:\opencv\build\x64\vc15\lib
             Linker Input 
                3. Add Linker input: opencv_world450d.lib
                   d for debug without d for release
