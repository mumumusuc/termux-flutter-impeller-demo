## Impeller3D on Termux

<p align="middle" float="left">
<img src="https://raw.githubusercontent.com/mumumusuc/termux-flutter-impeller-demo/main/preview.webp" width="60%"/>
</p>


## Build

1. checkout master branch of `flutter` and then install this [patch](https://github.com/mumumusuc/termux-flutter)
    ```
    $ git clone https://github.com/flutter/flutter -b 3.15.0-12.0.pre
    $ ./install.py -d dart-sdk.tar.gz -e engine.tar.gz flutter
    ```
    > ⚠️ NOTE: use **[Pre-release](https://github.com/mumumusuc/termux-flutter/releases/tag/3.15.0-12.0.pre)** assets.


2. install dependencies
    ```
    $ apt install clang make glfw libglvnd-dev libc++ termux-x11
    ```


3. clone & make
    ```
    $ git clone https://github.com/mumumusuc/termux-flutter-impeller-demo
    $ cd termux-flutter-impeller-demo
    $ export DISPLAY=:0
    $ termux-x11 :0 &
    $ make run
    ```


## Reference

- [bdero/flutter-scene-example](https://github.com/bdero/flutter-scene-example)

- [engine/examples/glfw](https://github.com/flutter/engine/tree/e4749e77e12bd7945dfeaa0072ebdfd964c74775/examples/glfw)

