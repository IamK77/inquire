# Inquire

[English](../README.md) | 中文

Inquire是一个简单的C++命令行交互式三方库，方便开发者在创建交互式命令行应用程序。

## 背景

本人在撰写一些简单脚本时, 意欲使用交互式命令行方便使用, 但在github查阅时, 发现Rust有`inquire`, python有`bullet`, 但C++却没有一个达到预期效果的库, 于是便有了这个项目。

本库的效果模仿了[inquire](https://github.com/mikaelmello/inquire)

## 特性

- [`Select`](#select) 从一个选项列表中选择一个选项
- [`Text`](#test) 输入文本
- [`Confirm`](#confirm) 确认
- [`Password`](#password) 输入密码

## 示例运行

```shell
make example
```

## 使用

将`include`中的`inquire.hpp`放入你的项目中, 然后使用即可。

在你存放第三方库的目录下创建一个`inquire`文件夹, 根据自身情况选择命令运行。

```shell
mkdir inquire
cd inquire
```

通过wget下载`inquire.hpp`文件

```shell
wget -O inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

或者通过curl下载`inquire.hpp`文件

```shell
curl -L -o inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

curl代理下载`inquire.hpp`文件, 将命令中的`7890`替代为你的代理端口

```shell
curl -x 127.0.0.1:7890 -L -o inquire.hpp https://github.com/IamK77/inquire/releases/download/v0.1.0/inquire.hpp
```

代码示例如下:

```cpp
#include "inquire.hpp"

int main() {
    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Option 4"};

    std::string result = Inquire::Select("Select an option", options).prompt();
    std::cout << "You selected: " << result << std::endl;

    std::string text = Inquire::Text("Enter some text").prompt();
    std::cout << "You entered: " << text << std::endl;

    std::string password = Inquire::Password("Enter a password").prompt();
    std::cout << "You entered: " << password << std::endl;

    bool confirm = Inquire::Confirm("Are you suuuuuure?").prompt();
    std::cout << "You confirmed: " << confirm << std::endl;

    return 0;
}
```

在代码上使用与`inquire`类似

## Select

在`Select`中, 通过方向键选择选项, 输入关键词进行筛选, 按回车键确认, ESC键用于取消选择。

```cpp
std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Option 4"};

std::string result = Inquire::Select("Select an option", options).prompt();

if (result != "") {
    siwtch (result) {
        case "Option 1":
            // do something
            break;
        case "Option 2":
            // do something
            break;
        case "Option 3":
            // do something
            break;
        case "Option 4":
            // do something
            break;
    }
}
```

*class Select*
----------------
- `Select(param_question, param_options)` 构造函数
    - `param_question` : std::string 问题
    - `param_options` : std::vector\<std::string> 选项列表

- `prompt()` : std::string 显示问题并返回用户选择的选项
    - `return` : std::string 用户选择的选项


## Text

在`Text`中, 输入文本后按回车键确认, ESC键用于取消输入。

```cpp
std::string text = Inquire::Text("Enter some text").prompt();
```

*class Text*
----------------
- `Text(param_question)` 构造函数
    - `param_question` : std::string 问题

- `prompt()` : std::string 显示问题并返回用户输入的文本
    - `return` : std::string 用户输入的文本


## Confirm

在`Confirm`中, 按回车键确认, ESC键用于取消选择。

```cpp
bool confirm = Inquire::Confirm("Are you suuuuuure?").prompt();
```

*class Confirm*
----------------
- `Confirm(param_question)` 构造函数
    - `param_question` : std::string 问题

- `prompt()` : bool 显示问题并返回用户选择的选项
    - `return` : bool 用户选择的选项


## Password

在`Password`中, 输入密码后按回车键确认, ESC键用于取消输入。

```cpp
std::string password = Inquire::Password("Enter a password").prompt();
```

*class Password*
----------------
- `Password(param_question)` 构造函数
    - `param_question` : std::string 问题

- `prompt()` : std::string 显示问题并返回用户输入的密码
    - `return` : std::string 用户输入的密码


## 相关仓库

- [inquire](https://github.com/mikaelmello/inquire) - Rust