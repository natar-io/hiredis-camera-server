# Hiredis Camera Server

**C++** implementation of a camera server using **RedisImageHelper** library.
## Prerequisites

- [CMake](https://cmake.org)
- [Hiredis](https://github.com/redis/hiredis)
- [Libev](https://)
- [OpenCV](https://opencv.com) **/!\ Optional** (Used to display images in sample program)
- [RedisImageHelper](https://forge.pole-aquinetic.net/nectar-platform/redis-image-helper)
- [cxxopts](https://github.com/jarro2783/cxxopts) Used to manage commandline options.

## Compiling & Installing
Go into project folder and run the following:
```
mkdir build
cd build
cmake ..
make -j8
```
This should create two executables : **redis-camera-server** and **redis-camera-client**.

## Running the camera server
The camera server comes with a lot of command line options. You can check them all using the following command `./redis-camera-server -h`.
```
Usage:
  ./redis-camera-server [OPTION...]

      --redis-host arg         The host adress to which the redis client
                               should try to connect
      --redis-port arg         The port to which the redis client should try
                               to connect.
  -o, --output arg             The redis output key where data are going to
                               be published/set.
  -s, --stream                 Activate stream mode. In stream mode the
                               program will constantly process input data and
                               publish output data. By default stream mode is
                               enabled.
  -u, --unique                 Activate unique mode. In unique mode the
                               program will only read and output data one time.
      --tegra-camera           NVIDIA Tegra specific option. When this is set
                               the program will try to access tegra camera
                               using gstreamer command.
  -c, --camera-id arg          Set camera device id to get frames from. If
                               tegra-camera is set this will have no effect.
      --camera-parameters arg  The redis key where camera parameters are
                               going to be set.
  -v, --verbose                Enable verbose mode. This will print helpfull
                               process informations on the standard error
                               stream.
  -h, --help                   Print this help message.
```
### How to use 
Here are some example of the main uses of the camera-server:
- Run the camera server **one time** and output data to the redis key: `camera:output`<br></br>
`./redis-camera-server -u -o  camera:output`
- Run the camera server in **stream mode** and output data to the redis key: `camera:output`<br></br>
`./redis-camera-server -s -o camera:output`
- Run the camera-server and connect it to another redis server than the default one: <br></br>
`/redis-camera-server --redis-host 127.0.0.1 --redis-port 6379`
- Run the camera server using a different camera than the default one: <br></br>
`./redis-camera-server -c x` where `x` is the id of the camera.
- Run the camera server on a **NVIDIA Tegra** in stream mode output camera parameters to `tegra-camera:parameters`and output image data to `tegra-camera:output`: <br></br>
`./redis-camera-server --tegra-camera -s --camera-parameters tegra-camera:parameters -o tegra-camera:output`

**Note:** You can run the executable without any option. By default the server runs in **stream** mode and output data to the standard redis-server : **127.0.0.1:6379**.
**Note 2: ** Command line options support the following formats:
- `-o value`
- `-o=value`
- `-cvo value`. In this particular case `c` and `v` are options that do not takes arguments.

## Running the camera client
To test out the camera-server we provide you a simple client that display the server output.
Running `./redis-camera-client -h` to check out client's options.
```
Usage:
  ./redis-camera-client [OPTION...]

      --redis-host arg         The host adress to which the redis client
                               should try to connect
      --redis-port arg         The port to which the redis client should try
                               to connect.
  -i, --input arg              The redis input key where data are going to
                               arrive.
  -s, --stream                 Activate stream mode. In stream mode the
                               program will constantly process input data and
                               publish output data. By default stream mode is
                               enabled.
  -u, --unique                 Activate unique mode. In unique mode the
                               program will only read and output data one time.
  -v, --verbose                Enable verbose mode. This will print helpfull
                               process informations on the standard error
                               stream.
      --camera-parameters arg  The redis input key where camera-parameters
                               are going to arrive.
  -h, --help                   Print this help message.
```

This works exactly the same as the server but instead of the `-o, --output` option we provide a `-i, --input` options to read data from.
