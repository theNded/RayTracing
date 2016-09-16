# RayTracing
This is a naive GPU-based ray tracing project for volume rendering on regular volume data. 

Currently, this project can run on **Ubuntu 14.04 LTS** and **Mac OS X**. It is not guaranteed to be stable, as it is still in progress. Moreover, it is only a personal repo that is not intended to be widely used, so is not considerably robust.

## Requirements
### Graphics 
- [GLFW3](http://www.glfw.org/)
- [GLEW](http://glew.sourceforge.net/)
- [glm](glm.g-truc.net/)

### Parallel Computation
- OpenCL (I don't have Nvidia card on my Mac...)

### Others
- [RapidJson](https://github.com/miloyip/rapidjson)


## Data Source
### Volume Data
Volume data are stored in the raw format. Typically, they can be directly read from the file and fill into `uchar [dims.x * dims.y * dims.z * unit_size]`. 

Data can be downloaded from the [Volume Library](http://www9.informatik.uni-erlangen.de/External/vollib/). These files are in `.pvm` format, which can be converted to raw format by the tool `pvm2raw` in [V^3](http://www.stereofx.org/volume.html) provided by the same generous author.

### Transfer Function
I'm still confused about how transfer function indeed works. At current I followed this [tutorial](// http://graphicsrunner.blogspot.jp/2009/01/volume-rendering-102-transfer-functions.html). Hopefully, I will update this part later.

### Configuration
All configuration are stored in json files for simplicity. I use RapidJson, a small but fast library to read them.

A typical Json file holds the configuration in the form:

```json
{
	 "file name" : "your-file-name",
	 "volume data path" : "your-volume-data-path",
	 "transfer function path" : "your-transfer-function-path",
	 "dims" : {
	 	"x" : volume-data-x-size,
	 	"y" : volume-data-y-size,
	 	"z" : volume-data-z-size
	 },
	 "scale" : {
	 	"x" : scale-x.f,
	 	"y" : scale-y.f,
	 	"z" : scale-z.f
	},
	"unit size" : unit-of-volume-data
}
```


