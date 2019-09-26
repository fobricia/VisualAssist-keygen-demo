# VisualAssist-keygen

事实上，这个repo并不完整，在我的电脑里有一个名为 `VisualAssist-patcher` 的项目，而这个项目可以替换 `VA_X.dll` 里的官方公钥，从而使得 `VisualAssist-keygen` 生成的注册码可以起作用。

但是，我不想收到一份 DMCA takedown 通知，同时我也不想让 Visual Assist 的开发者 `Whole Tomato` 可以轻松地找到反patch的解决方案。

__所以，所有有关 `VisualAssist-patcher` 的内容，在这份repo里都会用 `*` 号隐去。__

## 1. 它是如何运作的？

这部分我不太想写，因为好累啊。🤪

如果这个项目有了100个stars，我就告诉你。

## 2. 如何编译？

见[这里](doc/how-to-build.zh-CN.md)。

## 3. 如何使用？

1. \*\*\* \* \*\*\*\*\* \*\* `********`.

   ```
   ******
       ************************************************

           ****************************************************
                                      *********************************
   ```

2. 生成序列号。

   ```
   Usage:
       VisualAssist-keygen.exe <username> <license count> <expire date>

           [-renew]           Generate renew-key.
                              This parameter is optional.

           <username>         The username licensed to.
                              This parameter must be specified.

           <license count>    The maximum number of copies that can be run under the newly-generated license.
                              The value should be in 1 ~ 65536.
                              This parameter must be specified.

           <expire date>      The date when the newly-generated license expires.
                              The format must be one of the following:
                                  1. "yyyy/mm/dd"
                                  2. "yyyy-mm-dd"
                                  3. "yyyy.mm.dd"
                              This parameter must be specified.
   ```

   __例如：__

   ```console
   $ VisualAssist-keygen.exe DoubleLabyrinth 1 2050/1/1
   DoubleLabyrinth (1-user license) Support ends 2050.1.1
   00001M-8E5KE8-2W995Z-KJJCYP-JAZRJC-47EZ53-BE599Q-CFCYV4-UVFZ5Z-9KGX20-2Q6XQD

   $ VisualAssist-keygen.exe -renew DoubleLabyrinth 1 2050/1/1
   DoubleLabyrinth (1-user license) Support ends 2050.1.1
   00001M-8E5KE8-2W99XH-QYANMV-ZXRNQ4-6ETZVQ-WH1AGZ-9RJ7HB-F3JX6J-V8WGFG-0EY3R6
   ```
