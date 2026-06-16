# 开发指南

## 开发环境设置

### 1. 安装依赖

```bash
# Ubuntu 18.04 / 20.04
sudo apt-get update
sudo apt-get install -y build-essential cmake git
sudo apt-get install -y libopencv-dev libeigen3-dev libboost-all-dev
sudo apt-get install -y libpcl-dev
sudo apt-get install -y freeglut3-dev libglfw3-dev libglew-dev libyaml-cpp-dev
```

### 2. 克隆仓库

```bash
git clone https://github.com/huangjunhao0920-ux/dianyun.git
cd dianyun
```

### 3. 编译项目

```bash
mkdir build
cd build
cmake ..
make -j4
```

## 代码规范

### 命名规范

- **类名**：PascalCase (例如: `PointCloudProcessor`)
- **函数名**：snake_case (例如: `process_pointcloud`)
- **变量名**：snake_case (例如: `point_count`)
- **常量名**：UPPER_CASE (例如: `MAX_POINTS`)
- **成员变量**：以下划线结尾 (例如: `data_`)

### 代码风格

- 使用4个空格进行缩进
- 单行最多80个字符
- 使用大括号的K&R风格

```cpp
void process_data() {
    if (condition) {
        do_something();
    } else {
        do_something_else();
    }
}
```

### 文件结构

```
src/
├── module_name/
│   ├── module_name.h        # 头文件
│   └── module_name.cpp      # 实现文件
```

## 模块开发指南

### 添加新模块

1. 在 `src/` 下创建新目录
2. 编写头文件 (.h)
3. 编写实现文件 (.cpp)
4. 更新 CMakeLists.txt
5. 编写单元测试

### 模块模板

```cpp
// module_name.h
#ifndef DIANYUN_MODULE_NAME_H
#define DIANYUN_MODULE_NAME_H

namespace dianyun {

class ModuleName {
public:
    ModuleName();
    ~ModuleName();
    
    void init();
    void process();
    
private:
    // Private members
};

} // namespace dianyun

#endif
```

## 测试

### 运行测试

```bash
cd build
ctest
```

### 编写单元测试

```cpp
// test/test_module.cpp
#include <gtest/gtest.h>
#include "module_name/module_name.h"

using namespace dianyun;

TEST(ModuleNameTest, BasicTest) {
    ModuleName module;
    module.init();
    EXPECT_TRUE(module.is_initialized());
}
```

## 提交规范

### Commit 消息格式

```
<type>: <subject>

<body>

<footer>
```

### 类型

- `feat`: 新功能
- `fix`: 修复Bug
- `docs`: 文档修改
- `style`: 代码风格修改
- `refactor`: 代码重构
- `perf`: 性能优化
- `test`: 测试相关

### 示例

```
feat: add IMU pose estimation

Implement IMU data processing for pose estimation using
quaternion representation. Support roll, pitch, yaw calculation.

Closes #123
```

## 性能优化

### 内存优化

- 使用 std::move 进行转移语义
- 在循环中避免频繁分配
- 使用 std::vector 预分配大小

### CPU优化

- 使用 SIMD 指令集 (AVX, SSE)
- 并行化处理
- 减少数据复制

### 显示优化

- 点云降采样
- GPU加速
- 多线程渲染

## 调试

### 编译Debug版本

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4
```

### 使用GDB

```bash
gdb ./bin/mdc_pointcloud_app
(gdb) run
(gdb) bt  # 查看堆栈
(gdb) p variable_name  # 打印变量
```

### 日志输出

```cpp
LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARN("Warning message");
LOG_ERROR("Error message");
```

## 常见问题

### 编译错误

**Q: undefined reference to ...**
A: 检查CMakeLists.txt中的链接库配置

**Q: fatal error: header.h: No such file**
A: 检查include路径和文件是否存在

### 运行时错误

**Q: Segmentation fault**
A: 使用valgrind检查内存泄漏

```bash
valgrind --leak-check=full ./bin/mdc_pointcloud_app
```

## 资源链接

- [CMake官方文档](https://cmake.org/documentation/)
- [C++参考](https://en.cppreference.com/w/)
- [Eigen用户手册](https://eigen.tuxfamily.org/dox/)
- [PCL教程](http://pointclouds.org/documentation/tutorials/)
