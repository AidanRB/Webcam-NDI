# Webcam-NDI
Run an NDI stream of a single webcam
## Building
* [Compile OpenCV into `opencv-build/`](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
    * Only the core modules are necessary, opencv_contrib can be ignored
* Extract the NDI SDK into `ndi_sdk/`
* Build manually using cmake, or just use VS Code
    * Manually:
        * `cmake .`
        * `cmake --build .`
    * VS Code:
        * Install [this](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
        * Open this folder and initialize the project or whatever it brings up
        * Click the play button on the bottom of the window