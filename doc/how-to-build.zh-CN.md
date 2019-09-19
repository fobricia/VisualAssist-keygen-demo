# VisualAssist-keygen - 如何编译？

## 1. 前提条件

1. 请确保你有 __Visual Studio 2019__ 或者更高版本。因为这是一个VS2019项目。

2. 请确保你安装了 `vcpkg` 以及下面几个库：

   * `********************************`
   * `***********************`
   * `mpir:x86-windows-static`

   你可以通过下面的命令来安装它们：

   ```console
   $ **********************************************
   $ *************************************
   $ vcpkg install mpir:x86-windows-static
   ```

3. 你的 `vcpkg` 已经和你的 __Visual Studio__ 集成了，即你曾成功运行了：

   ```console
   $ vcpkg integrate install
   ```

## 2. 编译

1. 在 __Visual Studio__ 打开这个项目。

2. 选择 `Release` 配置。

3. 选择 `x86` 平台。

   __PS:__

   * \*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*

   * `VisualAssist-keygen` 可以在 `x86` 和 `x64` 两种模式下编译。

     如果要编译 `x64` 的 `VisualAssist-keygen`，需要在 `vcpkg` 中增加 `mpir:x64-windows-static` 库。

4. 选择 __生成 > 生成解决方案__。

生成完成后，你会在 `bin/` 文件夹下看到编译后的patcher和keygen。

