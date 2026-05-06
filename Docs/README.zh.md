# Inquire

[English](../README.md) | 中文

一个生产级的 C++11 单头交互式命令行库,灵感来自 Rust 的
[inquire](https://github.com/mikaelmello/inquire)。

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

int main() {
    auto fruit = Inquire::Select("选个水果", {"苹果", "香蕉", "樱桃"})
        .page_size(5)
        .prompt();
}
```

## 特性

| 组件             | 返回类型                      | 关键能力                                     |
|------------------|-------------------------------|--------------------------------------------|
| `Select`         | `std::string` / `int`         | 模糊筛选、分页滚动、默认项、自定义匹配函数     |
| `MultiSelect`    | `std::vector<std::string>`    | 最少/最多勾选、默认项、分页                   |
| `Text`           | `std::string`                 | 行内光标编辑、默认值、占位符、验证器          |
| `Password`       | `std::string`                 | 字符遮蔽、可选显示输入、验证器                |
| `Confirm`        | `bool`                        | y/n 键、可选默认、回车应用默认                |

通用能力:

- **类型化错误**:`CancelledError`、`ValidationError`、`EmptyOptionsError`、
  `InterruptedError`、`IoError`,均派生自 `InquireError` 并带有 `ErrorCode`。
- **非异常 API**:每个组件都有 `try_prompt()` 返回 `Result<T>`。
- **UTF-8 感知**:Linux 与 Windows 都正确读取多字节输入;CJK 全宽字符按
  显示宽度截断而非字节截断。
- **信号安全**:SIGINT/SIGTERM/SIGSEGV 会先恢复终端再重新触发,不会留下
  "瞎掉的 shell"。
- **跨平台 ANSI**:Windows 10+ 自动启用 VT 模式,渲染逻辑统一。
- **链式构造器**:`Prompt(...).foo().bar().prompt()`。

## 集成

把 `include/inquire.hpp` 拷到你的项目里。**恰好一个**翻译单元里这样写:

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"
```

其他翻译单元只 `#include "inquire.hpp"` 即可。

也可以从源码构建为静态库:

```shell
make lib       # 产出 build/libinquire.a
make demo      # 构建演示程序
make test      # 跑 46 个单元测试
make example   # 构建并运行交互示例
```

## 上手

```cpp
#define INQUIRE_IMPLEMENTATION
#include "inquire.hpp"

int main() {
    using namespace Inquire;
    try {
        std::string fruit = Select("选个水果", {
                "苹果", "香蕉", "樱桃", "榴莲", "接骨木莓"
            })
            .page_size(3)
            .default_index(1)
            .help_message("输入字符筛选,↑↓ 移动,回车确认")
            .prompt();

        std::vector<std::string> toppings = MultiSelect("选配料", {
                "芝士", "橄榄", "蘑菇", "培根", "菠菜"
            })
            .min_selected(1)
            .max_selected(3)
            .prompt();

        std::string name = Text("你叫?")
            .default_value("匿名")
            .add_validator(validators::min_length(2))
            .prompt();

        std::string pwd = Password("设置密码")
            .add_validator(validators::all_of({
                validators::min_length(8),
                validators::matches(".*[0-9].*", "至少包含一个数字")
            }))
            .prompt();

        bool ok = Confirm("确认提交?").default_value(true).prompt();
    } catch (const Inquire::CancelledError&) {
        return 1;
    }
}
```

## 错误模型

所有 prompt 在用户取消时会抛异常。基类:

```cpp
class InquireError : public std::runtime_error {
public:
    ErrorCode code() const noexcept;
};
```

具体子类:

| 类型                 | 触发时机                                    |
|----------------------|--------------------------------------------|
| `CancelledError`     | 用户按 ESC 或 Ctrl+C                        |
| `InterruptedError`   | 用户按 Ctrl+D                               |
| `ValidationError`    | 验证器拒绝(在 prompt 内部捕获并重新提示)     |
| `EmptyOptionsError`  | `Select`/`MultiSelect` 传入空选项列表        |
| `IoError`            | 终端 I/O 失败                               |

不想用异常?

```cpp
auto r = Inquire::Select("pick", {"a", "b"}).try_prompt();
if (r.ok()) {
    std::cout << r.value() << "\n";
} else if (r.error_code() == Inquire::ErrorCode::Cancelled) {
    std::cout << "用户取消\n";
}
```

## 验证器

`Inquire::validators` 内置:

```cpp
validators::non_empty()
validators::min_length(n)
validators::max_length(n)
validators::length_between(lo, hi)
validators::matches("[a-z0-9]+")
validators::integer()
validators::all_of({a, b, c})
```

验证器就是 `std::function<void(const std::string&)>`,自定义只需在不通过时
抛 `ValidationError`:

```cpp
Inquire::Text("端口?")
    .add_validator([](const std::string& s) {
        if (std::stoi(s) > 65535) throw Inquire::ValidationError("> 65535");
    })
    .prompt();
```

校验失败不会让 `prompt()` 抛异常 — 会显示错误并让用户重新输入。

## 各组件参考

### `Select`

```cpp
Select(std::string question, std::vector<std::string> options);
Select& page_size(int n);             // 默认 7
Select& default_index(int i);
Select& help_message(std::string m);
Select& case_sensitive(bool on);      // 默认大小写不敏感
Select& filter(std::function<bool(const std::string& option,
                                  const std::string& input)> fn);

std::string             prompt();
int                     prompt_index();
Result<std::string>     try_prompt();
```

按键:`↑/↓` 移动,`PgUp/PgDn`,`Home/End`,直接输入字符模糊筛选,
`Enter` 确认,`Esc` 或 `Ctrl+C` 取消,`Ctrl+U` 清空筛选关键字。

### `MultiSelect`

```cpp
MultiSelect(std::string question, std::vector<std::string> options);
MultiSelect& page_size(int n);
MultiSelect& default_indices(std::vector<int> idx);
MultiSelect& min_selected(int n);
MultiSelect& max_selected(int n);
MultiSelect& help_message(std::string m);
MultiSelect& case_sensitive(bool on);

std::vector<std::string>             prompt();
std::vector<int>                     prompt_indices();
Result<std::vector<std::string>>     try_prompt();
```

按键:`↑/↓`、`PgUp/PgDn`、`Home/End`、`Space` 切换选中、`Enter` 确认。

### `Text`

```cpp
Text(std::string question);
Text& default_value(std::string v);
Text& placeholder(std::string p);
Text& help_message(std::string m);
Text& add_validator(Validator v);
```

按键:`←/→` 移动光标,`Home/End`(或 `Ctrl+A`/`Ctrl+E`),`Backspace`/`Delete`,
`Ctrl+U` 清空,`Enter` 提交。

### `Password`

```cpp
Password(std::string question);
Password& mask_char(char c);          // 默认 '*'
Password& show_typing(bool on);
Password& help_message(std::string m);
Password& add_validator(Validator v);
```

### `Confirm`

```cpp
Confirm(std::string question);
Confirm& default_value(bool b);
Confirm& help_message(std::string m);
```

按键:`y/Y` → true,`n/N` → false,`Enter` 应用默认(若设置)。

## 仓库结构

```
include/inquire.hpp        合并后的单头文件(make single-header 重新生成)
src/Inquire/
├── inquire.hpp / .cpp     公共 API 与各 prompt 实现
├── error.hpp              错误层级与 Result<T>
├── validator.hpp          验证器(纯头文件)
├── utils/
│   ├── colorful.hpp/.cpp  ANSI 着色
│   ├── console.hpp/.cpp   Terminal 抽象(尺寸、光标、ANSI)
│   ├── encode.hpp/.cpp    UTF-8 控制台编码切换
│   ├── func.h/.cpp        按键捕获与 UTF-8 工具
│   ├── renderer.hpp/.cpp  多行稳定重绘、宽度截断
│   └── tty_guard.hpp/.cpp Linux 端 RAII 原始模式 + 信号安全恢复
example/example.cpp        跑全部 5 个 prompt
tests/test.cpp             46 个非交互单元测试
tools/amalgamate.sh        从模块化源码生成 include/inquire.hpp
```

## 平台

- Linux:任何支持 ANSI 的终端。
- Windows 10+:自动启用 VT 模式。
- macOS:与 Linux 共用 POSIX termios 路径。

## 版本

`0.x` 阶段允许小版本之间破坏兼容;单头使用者请固定到某个 tag。

## 许可证

MIT。见 `LICENSE`。
