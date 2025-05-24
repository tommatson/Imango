# 🖼️ Imango
Imango is a pure-C image processing library focused on manual/hybrid content-based image recognition. Built entirely from scratch with no external dependencies, it implements core computer vision algorithms with full control and transparency.

> **Note:** ⚠️ Disclaimer
This is a demonstration project. It is not optimized for production use and should not be relied on for performance-critical applications.
Imango supports only BMP image files, and no other formats will be added.  

## ✨ Features
### Canny Edge Detection ###
Implements the full pipeline:

Grayscale conversion

Gaussian blur

Sobel edge detection

Non-maximum suppression

Hysteresis thresholding

```c
applyCanny("image.bmp");
```
### Shi-Tomasi Corner Detection ###
Detects strong corners using the structure tensor method:

```c
applyCorner("image.bmp");
```

### Blob Detection (Difference of Gaussians) ###
Detects image regions of interest based on scale-space extrema:

```c
applyBlob("image.bmp");
```

> **Note:** Each function accepts a const char* filename (must be a .bmp file).

## 🧠 Usage Guide ##
### 🔧 Build ###
```bash
make
```
This will compile the library and place the executable in the build/ directory.

▶️ Running the Demo
To test the algorithms:

```bash
cd build
./main "relative/path/to/image.bmp" 3 1
```
Arguments:

"image.bmp" — relative path to a BMP image file.

3 — Gaussian kernel width (must be odd).

1 — Standard deviation for Gaussian blur.

You can change the default kernel parameters to tune image smoothing and feature detection sensitivity.

## 🧰 Additional Functions ##
All header files in the include/ directory expose lower-level functions for:

### Custom kernel generation ###

### Pixel-level operations ###

### Manual access to image buffers and intermediate stages ###

You can use these to construct your own image processing pipeline or customize behavior beyond the default demos.

## 🎯 Use Cases ##
Hands-on learning of image processing algorithms in raw C

Demonstrating algorithmic concepts in academic or low-level systems
