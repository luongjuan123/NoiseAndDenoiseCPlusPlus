##  Features

### 1. Noise Generation (Add Noise)

Simulate real-world image degradation by adding different types of noise:

* **Gaussian Noise:** Simulates sensor noise or low-light conditions. Adjustable *Mean* and *Variance*.
* **Salt & Pepper Noise:** Simulates "dead pixels" or data transmission errors. Adjustable *Amount*.
* **Speckle Noise:** Multiplicative noise often seen in radar or medical imaging.
* **Poisson Noise:** Simulates photon shot noise (sensor noise).

### 2. Denoising Algorithms (Remove Noise)

Restore images using various filtering techniques:

* **Gaussian Blur:** Basic smoothing (good for reducing high-frequency noise).
* **Median Blur:** The best solution for removing *Salt & Pepper* noise without blurring edges too much.
* **Bilateral Filter:** Advanced smoothing that preserves edges while reducing noise (excellent for Gaussian noise).
* **Non-Local Means (NL Means) Denoising:** High-end algorithm that removes noise by comparing similar patches across the image.

### 3. User Interface

* **Side-by-Side Comparison:** View the Original image and the Result image simultaneously.
* **Real-time Tuning:** Adjust parameters (Sigma, Kernel Size, h-value, etc.) using sliders and spinboxes.
* **Optimization:** Automatic image resizing for large files (4K+) to ensure smooth performance.
* **Interactive:** Smart GUI that enables/disables specific controls based on the selected algorithm.

---

##  Technologies Used

* **Language:** C++ (Standard C++17)
* **GUI Framework:** Qt 5 (Widgets)
* **Computer Vision:** OpenCV 4.x (core, imgproc, photo)
* **Build System:** CMake

---

##  Installation & Build Instructions

### Prerequisites (Linux/Ubuntu/Debian)

Ensure you have the necessary development packages installed:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install qtbase5-dev qt5-default
sudo apt-get install libopencv-dev
```

### Build Steps

1. Clone or download this repository to your local machine.
2. Open a terminal in the project directory.
3. Run the following commands:

```bash
# 1. Create a build directory
mkdir -p build
cd build

# 2. Configure the project with CMake
cmake ..

# 3. Compile the application
# (-j uses all CPU cores for faster compilation)
make -j$(nproc)
```

### Run the Application

After a successful build, an executable file will be created in the build folder:

```bash
./ImageProcessingApp
```

###  How to Use

1. **Open Image:** Click the "Open Image" button and select a file (.jpg, .png, .bmp).

2. **Add Noise:**

   * Select a noise type (e.g., Salt & Pepper) from the combo box.
   * Adjust the sliders (e.g., Amount) to see the image degrade.

3. **Apply Denoising:**

   * Select a filter method (e.g., Median Blur).
   * Adjust the Strength or Kernel Size slider.

> **Tip:** Try Median Blur for Salt & Pepper noise, and NL Means or Bilateral for Gaussian noise.

4. **Save:** Click "Save Result" to export the restored image to your computer.

###  Project Structure

* `main.cpp`: Entry point of the application.
* `imageprocessing.h`: Header file defining the MainWindow class and member variables.
* `imageprocessing.cpp`: Contains the core logic, UI setup, OpenCV algorithm implementations, and event handling.
* `CMakeLists.txt`: Configuration file for the CMake build system.

###  Contributing

Feel free to submit issues or pull requests if you want to improve the algorithms or add new features (like frequency domain filtering).

###  License

This project is open-source and available for educational purposes.
