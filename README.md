# 3D-rasterizer
---

![](https://github.com/BjorneEk/rasterizer/blob/images/teapot1.png?raw=true)
**A program that reads obj files and renders them in a window.**

---
![](https://github.com/BjorneEk/rasterizer/blob/images/skull.png?raw=true)
---
the goal with this project was to improve a previous 3d-renderer: [3D-graphics](https://github.com/BjorneEk/3d-graphics) with a proper rasterization algorithm and smooth shading.

![](https://github.com/BjorneEk/rasterizer/blob/images/ship1.png?raw=true)
![](https://github.com/BjorneEk/rasterizer/blob/images/ship2.png?raw=true)
## **Usage**

```
$ ./rasterizer <filename.obj>
$ ./rasterizer <filename.obj> -v for verbose output
$ ./rasterizer --help for help
```
if problems occure try running with verbose output this is due to a bug in the code when reading the obj file.
### **controlls**

| Key              | Action, enable/dissable |
|:----------------:|:-----------------------:|
|SPACE                   smooth shading      |
|W                 |       wireframe         |
|MOUSE             |       translate         |
|MOUSE + SHIFT     |        rotate           |
| scroll wheel     | zoom/translate in depth |
---
![](https://github.com/BjorneEk/rasterizer/blob/images/teapot2.png?raw=true)
---
<img alt="lang" src="https://img.shields.io/github/languages/top/bjorneek/rasterizer"/>
<img alt="size" src="https://img.shields.io/github/repo-size/bjorneek/rasterizer"/>
<img alt="lines" src="https://img.shields.io/tokei/lines/github/bjorneek/rasterizer"/>
