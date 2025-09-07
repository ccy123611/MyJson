# 用户自定义JSON解析和封装类

## 功能概述

`KJson` 类及其派生类 `KJsonObject` 和 `KJsonArray` 提供了一个灵活且高效的 JSON 数据结构和解析框架。该框架支持对 JSON 数据的动态创建、解析、查询和序列化操作，旨在为开发人员提供一个易于使用的接口以处理 JSON 格式的数据。

### 主要功能：

1. **JSON 数据解析**：
   - `KJson` 类支持从 JSON 文件中读取数据并将其解析为对应的 `KJsonObject` 或 `KJsonArray` 对象，方便开发者直接操作 JSON 内容。
2. **动态数据结构**：
   - `KJsonObject` 类实现了一个动态的键值对存储，允许开发者动态添加、删除和查询键值对。
   - `KJsonArray` 类提供了一个动态数组结构，支持对 JSON 数组的元素进行添加、删除和清空操作。
   - `KJsonParser` 类实现了对Json数据的解析及序列化
3. **线程安全**：
   - 在多线程环境中，`KJson` 及其派生类提供了内置的互斥机制，确保数据操作

  4.**数据序列化存储**：

​	  ○ 支持将对应的 `KJsonObject` 或 `KJsonArray` 对象序列化为Json格式或XML格式写入文件当中。

## API说明

### KJson ###

#### 概述

`KJson` 类是一个抽象基类，提供 JSON 数据解析和文件读取的基本功能。它的设计旨在支持派生类实现具体的 JSON 对象和数组操作。

#### 成员函数

```
virtual ~KJson() = default;
```

- **描述**: 虚析构函数，确保正确析构派生类对象。

```
std::shared_ptr<KJson> parseFromFile(const std::string& filePath);
```

**参数**:

- `filePath`: 要解析的 JSON 文件的路径。

- **返回**: 返回一个指向解析后 JSON 对象的 `shared_ptr<KJson>` 指针。
- **描述**: 读取指定路径的 JSON 文件并将其解析为 `KJson` 对象。该函数调用 `parse` 方法来执行具体的解析操作。

```
static std::string readFile(const std::string& filePath);
```

- **参数**:
  - `filePath`: 要读取的文件的路径。
- **返回**: 返回文件内容的字符串。
- **描述**: 读取指定路径的文件内容，并返回作为字符串。如果读取失败，可能会抛出异常。

#### 线程安全

- `KJson` 类本身不提供线程安全的功能。具体的线程安全管理应在派生类中实现，尤其是在对共享数据进行操作时。

### KJsonObject

#### 概述

`KJsonObject` 类是 `KJson` 的派生类，提供对 JSON 对象的操作，包括添加、删除键值对，以及序列化为 JSON 或 XML 格式。该类支持多线程访问，通过互斥锁来确保线程安全。

#### 成员函数

```
KJsonObject();
```

- **描述**: 默认构造函数，用于创建空的 JSON 对象。

```
~KJsonObject();
```

- **描述**: 默认析构函数，用于销毁 JSON 对象。

```
KJsonObject(const KJsonObject& other);
```

- **描述**: 拷贝构造函数，创建一个新的 `KJsonObject`，并复制另一个对象的内容。操作时使用互斥锁以确保线程安全。

```
KJsonObject& operator=(const KJsonObject& other);
```

- **描述**: 拷贝赋值运算符，复制另一个 `KJsonObject` 的内容。操作时使用互斥锁以确保线程安全。

```
KJsonObject(KJsonObject&& other) noexcept;
```

- **描述**: 移动构造函数，移动另一个 `KJsonObject` 的内容到当前对象。

```
KJsonObject& operator=(KJsonObject&& other) noexcept;
```

- **描述**: 移动赋值运算符，将另一个 `KJsonObject` 的内容移动到当前对象。使用互斥锁以确保线程安全。

```
bool serializeToJsonFile(const std::string& filePath) const;
```

- **参数**:
  - `filePath`: 要写入的文件路径。
- **返回**: 返回 `true` 表示序列化成功，`false` 表示失败。
- **描述**: 将当前 JSON 对象序列化为 JSON 字符串并写入指定文件。

```
bool serializeToXMLFile(const std::string& filePath) const;
```

- **参数**:
  - `filePath`: 要写入的文件路径。
- **返回**: 返回 `true` 表示序列化成功，`false` 表示失败。
- **描述**: 将当前 JSON 对象序列化为 XML 格式并写入指定文件。

```
void add(const std::string& key, const std::string& value);
```

- **参数**:
  - `key`: 要添加的键名。
  - `value`: 对应的键值。
- **描述**: 动态添加键值对到 JSON 对象中。

```
bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonObject& currentJson) const;
```

- **参数**:
  - `keys`: 查询的键路径。
  - `startIndex`: 查询的起始索引。
  - `currentJson`: 当前 JSON 对象。
- **返回**: 返回 `true` 表示查询成功，`false` 表示失败。
- **描述**: 查询 JSON 对象中的指定键。

```
void remove(const std::string& key);
```

- **参数**:
  - `key`: 要删除的键名。
- **描述**: 删除指定的键及其对应的值。

```
JsonValue& operator[](const std::string& key);
```

- **参数**:
  - `key`: 要访问的键名。
- **返回**: 返回对应键的 `JsonValue` 引用。
- **描述**: 处理嵌套对象和数组，返回对应键的值。也用于动态添加键值

```
void lock();
```

- **描述**: 上锁方法，用于确保线程安全。

```
void unlock();
```

- **描述**: 解锁方法，用于释放锁。

#### 线程安全

- `KJsonObject` 通过 `std::mutex` 确保对共享数据的安全访问。所有可能修改对象状态的方法均采用互斥锁以防止数据竞争。

### KJsonArray

#### 概述

`KJsonArray` 类是 `KJson` 的派生类，提供对 JSON 数组的操作，包括添加、删除元素，以及序列化为 JSON 或 XML 格式。该类支持多线程访问，通过互斥锁来确保线程安全。

#### 成员函数

```
KJsonArray();
```

- **描述**: 默认构造函数，用于创建空的 JSON 数组。

```
~KJsonArray();
```

- **描述**: 默认析构函数，用于销毁 JSON 数组。

```
KJsonArray(const KJsonArray& other);
```

- **描述**: 拷贝构造函数，创建一个新的 `KJsonArray`，并复制另一个对象的内容。操作时使用互斥锁以确保线程安全。

```
KJsonArray& operator=(const KJsonArray& other);
```

- **描述**: 拷贝赋值运算符，复制另一个 `KJsonArray` 的内容。操作时使用互斥锁以确保线程安全。

```
KJsonArray(KJsonArray&& other) noexcept;
```

- **描述**: 移动构造函数，移动另一个 `KJsonArray` 的内容到当前对象。

```
KJsonArray& operator=(KJsonArray&& other) noexcept;
```

- **描述**: 移动赋值运算符，将另一个 `KJsonArray` 的内容移动到当前对象。使用互斥锁以确保线程安全。

```
void add(std::shared_ptr<JsonValue> value);
```

- 参数:
  - `value`: 要添加的 `JsonValue` 对象的共享指针。
- **描述**: 添加元素到 JSON 数组中，并在操作时使用互斥锁确保线程安全。

```
void clear();
```

- **描述**: 清空 JSON 数组中的所有元素，并在操作时使用互斥锁确保线程安全。

```
void remove(int index);
```

- 参数:
  - `index`: 要移除的元素索引。
- **描述**: 移除指定索引的元素，并在操作时使用互斥锁确保线程安全。如果索引不合法，将输出错误信息。

```
bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonArray& currentJson) const;
```

- 参数:
  - `keys`: 查询的键路径。
  - `startIndex`: 查询的起始索引。
  - `currentJson`: 当前 JSON 数组。
- **返回**: 返回 `true` 表示查询成功，`false` 表示失败。
- **描述**: 查询 JSON 数组中的指定键。

```
void interactiveQuery();
```

- **描述**: 用于处理嵌套逻辑的查询函数。

```
size_t size() const;
```

- **返回**: 返回 JSON 数组中元素的数量。

```
bool serializeToJsonFile(const std::string& filePath) const;
```

- 参数:
  - `filePath`: 要写入的文件路径。
- **返回**: 返回 `true` 表示序列化成功，`false` 表示失败。
- **描述**: 将当前 JSON 数组序列化为 JSON 字符串并写入指定文件。

```
bool serializeToXMLFile(const std::string& filePath) const;
```

- 参数:
  - `filePath`: 要写入的文件路径。
- **返回**: 返回 `true` 表示序列化成功，`false` 表示失败。
- **描述**: 将当前 JSON 数组序列化为 XML 格式并写入指定文件。

```
using iterator = std::vector<std::shared_ptr<JsonValue>>::iterator;
```

- **描述**: 定义迭代器类型，以便遍历 JSON 数组元素。

```
using const_iterator = std::vector<std::shared_ptr<JsonValue>>::const_iterator;
```

- **描述**: 定义常量迭代器类型，以便遍历 JSON 数组元素。

```
iterator begin();
```

- **返回**: 返回指向数组第一个元素的迭代器。

```
iterator end();
```

- **返回**: 返回指向数组最后一个元素后的位置的迭代器。

```
const_iterator begin() const;
```

- **返回**: 返回指向数组第一个元素的常量迭代器。

```
const_iterator end() const;
```

- **返回**: 返回指向数组最后一个元素后的位置的常量迭代器。

```
void lock();
```

- **描述**: 上锁方法，用于确保线程安全。

```
void unlock();
```

- **描述**: 解锁方法，用于释放锁。

#### 线程安全

- `KJsonArray` 通过 `std::mutex` 确保对共享数据的安全访问。所有可能修改对象状态的方法均采用互斥锁以防止数据竞争。

### KJsonParser

#### 概述

`KJsonParser` 类用于解析 JSON 字符串并返回相应的 `KJson` 对象。该类提供了一系列函数以处理 JSON 数据的不同组成部分，如对象、数组和基本值类型。

#### 成员函数

```
KJsonParser(const char* json, int length);
```

- 参数:
  - `json`: 指向 JSON 字符串的指针。
  - `length`: JSON 字符串的长度。
- **描述**: 构造函数，用于初始化 `KJsonParser` 对象，并接收要解析的 JSON 字符串。

```
std::shared_ptr<KJson> parse();
```

- **返回**: 返回一个指向解析后的 `KJson` 对象的共享指针。
- **描述**: 解析 JSON 字符串，生成对应的 `KJson` 对象。

```
void skipBOM();
```

- **描述**: 跳过字节序标记（BOM），如果存在的话。

```
bool skipSpace();
```

- **返回**: 返回 `true` 表示成功跳过空白字符，`false` 表示失败。
- **描述**: 跳过 JSON 字符串中的空白字符，以便获取下一个有效的 token。

```
char nextToken();
```

- **返回**: 返回下一个有效的 token。
- **描述**: 获取下一个有效的 token，用于解析过程中的状态转换。

```
bool parseObject(KJsonObject& jsonObject);
```

- 参数:
  - `jsonObject`: 要填充的 `KJsonObject` 对象。
- **返回**: 返回 `true` 表示解析成功，`false` 表示失败。
- **描述**: 解析 JSON 对象，并将结果存储到给定的 `KJsonObject` 中。

```
bool parseArray(KJsonArray& jsonArray);
```

- 参数:
  - `jsonArray`: 要填充的 `KJsonArray` 对象。
- **返回**: 返回 `true` 表示解析成功，`false` 表示失败。
- **描述**: 解析 JSON 数组，并将结果存储到给定的 `KJsonArray` 中。

```
bool parseMember(KJsonObject& jsonObject);
```

- 参数:
  - `jsonObject`: 要填充的 `KJsonObject` 对象。
- **返回**: 返回 `true` 表示解析成功，`false` 表示失败。
- **描述**: 解析 JSON 对象中的一个键值对成员。

```
bool parseString(std::string& outString);
```

- 参数:
  - `outString`: 输出解析的字符串。
- **返回**: 返回 `true` 表示解析成功，`false` 表示失败。
- **描述**: 解析 JSON 字符串并将结果存储到指定的输出变量中。

```
bool parseValue(JsonValue& outValue);
```

- 参数:
  - `outValue`: 输出解析的值（支持各种数据类型）。
- **返回**: 返回 `true` 表示解析成功，`false` 表示失败。
- **描述**: 解析 JSON 值（如字符串、数字、布尔值等）。

```
int parseNumber(double& outNumber);
```

- 参数:
  - `outNumber`: 输出解析的数字。
- **返回**: 返回解析的数字的字符数量。
- **描述**: 解析 JSON 中的数字并将结果存储到指定的输出变量中。

#### 错误处理

- `KJsonParser` 使用 `lastError` 字符串存储解析过程中出现的错误信息，以便后续使用（虽然当前未实现）。

### main函数测试方式

    std::cout << "请选择一个操作:" << std::endl;
    std::cout << "0. 退出" << std::endl;
    std::cout << "1. 选择待解析文件" << std::endl;
    std::cout << "2. 查询" << std::endl;
    std::cout << "3. 更新" << std::endl;
    std::cout << "4. 输出为 JSON 文件" << std::endl;
    std::cout << "5. 输出为 XML 文件" << std::endl;

**注意对于文件测试，请输入完整的测试路径并保证文件存在，在本地开发测试过程中，本人使用Notepad将输入文件和输出文件的编码方式都改为了UTF-8进行测试，此方法下的测试能够使输出文件的中文字符正确显示。**

**路径示例：**

```
filePath = "D:\\Visual Studio Code\\KJSON-file\\input.json";
```

## 代码思路设计

### 类的设计

​	在初步开发过程中，没有考虑到Json文件中包含的是一个数组的情况，因此只设计了一个KJson类，用做Json对象的存储与管理。在定义好了基本的功能后发现对于Json文件的解析包含的函数太多，且并不属于Json类本身应该有的性质或功能，因此新建了一个KJsonParser类用于Json对象的解析，在构建Json解析函数的时候，对代码进行了两次重构。分别是了解到了varient库函数的使用，因此新建了一个JsonValue类，用于存储可变数据，及其初始化，由于多是默认函数，在API接口中不做过多介绍；以及了解到Json文件当中可能存储的是数组，新建了KJsonObject类以及KJsonArray类，将原有KJson类中与Json对象相关的函数迁移到KJsonObject类当中，并添加了对应的功能函数到KJsonArray类当中，将KJson类作为两者的父类，重构完毕。

### 解析函数代码思路

在 `KJsonParser` 类中，解析函数负责将输入的 JSON 字符串转换为相应的 JSON 对象（`KJsonObject`）或数组（`KJsonArray`）。整个解析过程遵循递归下降解析的方法，具体设计思路如下：

1. **结构与组织**:
   - 解析器分为多个函数，每个函数负责解析特定的 JSON 结构（对象、数组、值等），使得代码清晰易于维护。
   - 使用枚举类型定义常量，以提高可读性和减少硬编码的风险。
2. **输入处理**:
   - `KJsonParser` 的构造函数接收 JSON 字符串及其长度，并将其存储为指针（`json`），以便在解析过程中进行遍历。
   - 提供了 `skipBOM()` 函数来跳过 UTF-8 字节序标记（BOM），以确保正确解析数据。
3. **跳过空白字符**:
   - `skipSpace()` 函数用于跳过空格、制表符和换行符，确保在解析有效数据时不会受到无关字符的干扰。
4. **字符处理**:
   - `nextToken()` 函数返回下一个有效的字符，并处理注释（单行和多行），以确保在解析时不会受到注释内容的影响。
   - 该函数在遇到注释时会跳过相关内容，继续寻找有效的 JSON 数据。
5. **对象和数组解析**:
   - `parseObject()` 和 `parseArray()` 函数分别负责解析 JSON 对象和数组。每个函数会递归调用相关的解析函数，处理嵌套结构。
   - 在解析过程中，跟踪嵌套层级，确保不超过预设的嵌套限制。
6. **键值对解析**:
   - `parseMember()` 函数用于解析对象的键值对，首先解析键（字符串），然后解析值，并将其添加到相应的 JSON 对象中。
7. **值解析**:
   - `parseValue()` 函数用于解析不同类型的值，包括字符串、数字、布尔值、数组和对象。根据当前字符判断值的类型并调用相应的解析函数。
   - 处理数字的解析通过 `parseNumber()` 函数实现，支持整数和浮点数格式。
8. **字符串解析**:
   - `parseString()` 函数解析 JSON 字符串，并处理转义字符，确保正确构建结果字符串。
   - 使用 UTF-8 和 ASCII 的识别逻辑，以支持更广泛的字符集。
9. **错误处理**:
   - 在每个解析步骤中，检查是否存在错误并记录具体的错误信息（如“缺少分隔符”、“非法字符”等），以便在调用解析函数时能够获取详细的错误反馈。
10. **最终结果**:
    - `parse()` 函数是解析的入口点，决定是解析对象还是数组，并在完成解析后返回相应的 JSON 对象或数组。
    - 最终确保没有多余的数据出现在解析结果后，以保证解析的有效性。

### 其他函数代码思路

​	对于其他函数，占据主要码量的设计是KJsonArray类以及KJsonObject类的序列化和查询函数，二者设计思路与解析函数的设计思路类似，都是通过递归判断获取到的值的类型，对于获取到的类型进行对应的处理。对于序列化函数，则额外存储嵌套函数的缩进量保证格式的正确性。

## 总结

# MyJson
