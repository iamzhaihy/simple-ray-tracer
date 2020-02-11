# Simple Ray Tracer

The project is based on the first assignment of CS-GY 6533. In the process of developing, I realized that building a scene to ray-trace can be very tedious. Therefore, I augmented the project so that the ray tracer can read a scene from a JSON file (more on that later).

![Example Scene](./results/example-scene.png)



## Usage

This project uses [CMake](https://cmake.org/) to manage the build process. Theoretically, it should be very portable and able to compile on all major platforms. To build the project, you need to install `CMake` on your system and add it to `PATH`. To run, type the following commands:

```bash
git clone --recursive https://github.com/iamzhaihy/simple-ray-tracer.git
cd simple-ray-tracer
mkdir build && cd build
cmake ..
cmake --build .
```

It is important to include `--recursive` when cloning, otherwise the submodule will not be locally available. The ray-tracing process can take a long time. To make it run faster, you can switch to Release mode by typing `cmake --build . --config Release`.

After the steps listed above, you will see an executable called `simple-ray-tracer`. To run the program, type the command:

```bash
./simple-ray-tracer [<path-to-JSON-file>]
```

The command line argument is optional. If not specified, the scene in `data\sphere-and-plane.json` will be rendered. There are several sample JSON files in the `data` folder, and the result images are in the `results` folder.



## Implemented Features

- Sphere
- Triangle Mesh
    - `.off` files
    - `.obj` files
- Phong Shading Model
- Shadows
- Mirror Reflection



## Reference

- [An Introduction to Ray Tracing](http://www.realtimerendering.com/raytracing/An-Introduction-to-Ray-Tracing-The-Morgan-Kaufmann-Series-in-Computer-Graphics-.pdf)
- [Fundamentals of Computer Graphics](https://www.oreilly.com/library/view/fundamentals-of-computer/9781482229417/)



## Dependencies

- [GLM](https://glm.g-truc.net/0.9.9/index.html) for linear algebra
- [stb](https://github.com/nothings/stb) for writing images
- [RapisJson](https://rapidjson.org/) for parsing JSON files



## Acknowledgments

I borrowed many JSON files made by Professor [Alec Jacobson](https://github.com/alecjacobson). Big thanks to him and the original repository can be find [here](https://github.com/alecjacobson/computer-graphics-ray-tracing).
