# VisualAssist-keygen - How to build?

[中文版](how-to-build.zh-CN.md)

## 1. Prerequisites

1. Please make sure that you have __Visual Studio 2019__ or the higher. Because this is a VS2019 project.

2. Please make sure you have installed `vcpkg` and the following libraries:

   * `********************************`
   * `***********************`
   * `mpir:x86-windows-static`

   is installed.

   You can install them by:

   ```console
   $ **********************************************
   $ *************************************
   $ vcpkg install mpir:x86-windows-static
   ```

3. Your `vcpkg` has been integrated into your __Visual Studio__, which means you have run 

   ```console
   $ vcpkg integrate install
   ```

   successfully.

## 2. Build

1. Open this project in __Visual Studio__.

2. Select `Release` configuration. 

3. Select `x86` platform. 

   __PS:__

   * \*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*

   * `VisualAssist-keygen` can be compiled in both `x86` and `x64` mode.

     If you want to get `VisualAssist-keygen` in `x64` mode, you must install `mpir:x64-windows-static` in `vcpkg`.

4. Select __Build > Build Solution__.

You will see executable files in `bin/` directory. 

