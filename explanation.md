# C++ 會議室預約系統 - 完整初學者指南

本文件提供了此會議室預約系統（OA 系統）中所使用的 C++ 概念的完整說明。如果你是 C++ 的完全新手，本指南將逐步帶你了解每個概念。

---

## 目錄

1. [專案結構概覽](#1-專案結構概覽)
2. [標頭檔與原始碼檔](#2-標頭檔與原始碼檔)
3. [預處理器指令](#3-預處理器指令)
4. [命名空間](#4-命名空間)
5. [類別與物件](#5-類別與物件)
6. [存取修飾符](#6-存取修飾符)
7. [成員變數與方法](#7-成員變數與方法)
8. [建構函式](#8-建構函式)
9. [靜態成員](#9-靜態成員)
10. [友元類別](#10-友元類別)
11. [參考](#11-參考)
12. [單例設計模式](#12-單例設計模式)
13. [STL 容器（vector）](#13-stl-容器vector)
14. [輸入輸出串流](#14-輸入輸出串流)
15. [流程控制語句](#15-流程控制語句)
16. [main() 函式](#16-main-函式)
17. [範圍解析運算子](#17-範圍解析運算子)
18. [範圍式 for 迴圈](#18-範圍式-for-迴圈)
19. [錯誤處理模式](#19-錯誤處理模式)
20. [完整程式碼解析](#20-完整程式碼解析)

---

## 1. 專案結構概覽

```
meeting_room/
├── include/           <- 標頭檔 (.h) - 宣告
│   ├── OAUtils.h
│   ├── OASystemUI.h
│   ├── OAUser.h
│   ├── OAAdmin.h
│   └── OADataCenter.h
└── src/               <- 原始碼檔 (.cpp) - 實作
    ├── main.cpp
    ├── OAUtils.cpp
    ├── OASystemUI.cpp
    ├── OAUser.cpp
    ├── OAAdmin.cpp
    └── OADataCenter.cpp
```

### 為什麼採用這種結構？

- **關注點分離**：宣告（函式/類別的存在）與實作（如何運作）分開
- **加快編譯速度**：當你修改實作時，不需要重新編譯只使用標頭檔的檔案
- **更好的組織**：大型專案變得容易管理
- **資訊隱藏**：使用你程式碼的人只需要看標頭檔

---

## 2. 標頭檔與原始碼檔

### 標頭檔（.h）

標頭檔包含**宣告** - 它們告訴編譯器什麼東西存在，但不說明如何運作。

**來自 `OAUtils.h` 的範例：**
```cpp
#pragma once

class OAUtils {
   public:
    static int inputNumber();  // 宣告：「這個函式存在」
};
```

### 原始碼檔（.cpp）

原始碼檔包含**定義** - 事物如何運作的實際實作。

**來自 `OAUtils.cpp` 的範例：**
```cpp
#include "OAUtils.h"
#include <iostream>
using namespace std;

int OAUtils::inputNumber() {  // 定義：「這就是函式如何運作」
    int choice = -1;
    cin >> choice;
    while (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> choice;
    }
    return choice;
}
```

### 主要差異

| 標頭檔（.h） | 原始碼檔（.cpp） |
|--------------|------------------|
| 包含宣告 | 包含定義 |
| 說明「什麼存在」 | 說明「如何運作」 |
| 被其他檔案引入 | 直接編譯 |
| 使用 `#pragma once` | 使用 `#include` |

---

## 3. 預處理器指令

預處理器指令是在**編譯之前**處理的指令。它們都以 `#` 開頭。

### 3.1 `#pragma once`

```cpp
#pragma once
```

**用途**：防止標頭檔被多次引入。

**為什麼需要這個？** 想像這個情境：
- `FileA.h` 引入 `OAUser.h`
- `FileB.h` 引入 `OAUser.h`
- `main.cpp` 同時引入 `FileA.h` 和 `FileB.h`

沒有 `#pragma once`，編譯器會看到 `OAUser.h` 兩次，導致「重複定義」錯誤。

**替代方案（較舊的寫法）：**
```cpp
#ifndef OAUSER_H
#define OAUSER_H
// ... 標頭內容 ...
#endif
```

### 3.2 `#include`

```cpp
#include <iostream>    // 系統/標準函式庫標頭
#include "OAUtils.h"   // 使用者定義的標頭
```

**兩種形式：**
- `<filename>` - 先在系統/標準目錄中搜尋
- `"filename"` - 先在當前目錄中搜尋

**它做什麼**：字面上將被引入檔案的內容複製到當前檔案中。

### 3.3 `#define`

```cpp
#define FLAG_ADMIN 1
#define FLAG_USER 0
```

**用途**：建立巨集 - 一個文字替換規則。

程式碼中出現 `FLAG_ADMIN` 的地方，在編譯前都會被替換成 `1`。

**現代 C++ 替代方案**（較推薦）：
```cpp
const int FLAG_ADMIN = 1;
constexpr int FLAG_USER = 0;
```

---

## 4. 命名空間

### 什麼是命名空間？

命名空間是一個容器，用來容納一組識別符號（名稱）。它可以防止命名衝突。

### `std` 命名空間

```cpp
#include <iostream>
using namespace std;
```

標準函式庫將所有東西都放在 `std` 命名空間中。沒有 `using namespace std;`，你需要這樣寫：

```cpp
std::cout << "Hello" << std::endl;
std::string name;
std::vector<int> numbers;
```

有了 `using namespace std;`：

```cpp
cout << "Hello" << endl;
string name;
vector<int> numbers;
```

### 優缺點

**`using namespace std;` 的優點：**
- 打字較少
- 程式碼看起來更簡潔

**缺點：**
- 可能造成命名衝突
- 不建議在標頭檔中使用（會污染全域命名空間）

**生產環境的最佳實踐：**
```cpp
// 在標頭檔中，始終使用完整限定：
std::string name;

// 或使用特定引入：
using std::cout;
using std::endl;
```

---

## 5. 類別與物件

### 什麼是類別？

**類別**是建立物件的藍圖。它定義：
- **屬性**（資料/特性）
- **方法**（函式/行為）

### 什麼是物件？

**物件**是類別的實例 - 藍圖的具體實現。

### 程式碼中的範例

**類別定義（OAUser.h）：**
```cpp
class OAUser {
   private:
    string _username;      // 屬性
    string _password;      // 屬性
    string _department;    // 屬性

   public:
    OAUser();              // 方法（建構函式）
    OAUser(string username, string password, string department);  // 方法
};
```

**建立物件（OADataCenter.cpp）：**
```cpp
// 建立 OAUser 物件
users.push_back(OAUser("zhangsan", "123123", "行政部"));
users.push_back(OAUser("lisi", "123456", "人力資源部"));
```

### 類別與物件的類比

| 類別 | 物件 |
|------|------|
| 房子的藍圖 | 實際的房子 |
| 餅乾模具 | 一塊餅乾 |
| OAUser 類別 | 特定的使用者如「zhangsan」 |

---

## 6. 存取修飾符

存取修飾符控制誰可以存取類別成員。

### 三種存取級別

```cpp
class Example {
   public:      // 任何地方都可存取
    int publicVar;

   private:     // 只有類別內部可存取
    int privateVar;

   protected:   // 類別內部和衍生類別可存取
    int protectedVar;
};
```

### OAAdmin.h 的實際範例

```cpp
class OAAdmin {
    friend class OADataCenter;  // 特殊存取權限（稍後說明）

   private:
    string _username;   // 只有 OAAdmin 的方法可以存取
    string _password;   // 只有 OAAdmin 的方法可以存取

   public:
    OAAdmin();          // 任何人都可以呼叫這個建構函式
    OAAdmin(string username, string password);
    static OAAdmin ERROR_ADMIN;  // 任何人都可以存取
    string username();  // 任何人都可以呼叫這個方法
};
```

### 為什麼使用 Private？

**封裝** - 隱藏內部細節：

```cpp
class BankAccount {
   private:
    double balance;  // 無法從外部直接修改

   public:
    void deposit(double amount) {
        if (amount > 0) {  // 驗證！
            balance += amount;
        }
    }
};
```

沒有 private，有人可能會這樣做：
```cpp
account.balance = -1000000;  // 災難！
```

---

## 7. 成員變數與方法

### 成員變數（資料成員）

屬於類別的變數。

```cpp
class OAUser {
   private:
    string _username;    // 成員變數
    string _password;    // 成員變數
    string _department;  // 成員變數
};
```

**命名慣例**：底線前綴（`_username`）表示「這是一個私有成員」。這是一個常見的慣例，不是語言要求。

### 成員方法（成員函式）

屬於類別的函式。

**在標頭檔中宣告：**
```cpp
class OAAdmin {
   public:
    string username();  // 方法宣告
};
```

**在原始碼檔中定義：**
```cpp
string OAAdmin::username() {  // 方法定義
    return _username;
}
```

### Getter 和 Setter 方法

```cpp
class OAAdmin {
   private:
    string _username;

   public:
    // Getter - 回傳值
    string username() { return _username; }

    // Setter - 設定值（目前程式碼中沒有，但這是常見模式）
    void setUsername(string name) { _username = name; }
};
```

---

## 8. 建構函式

### 什麼是建構函式？

建構函式是一個特殊的方法，在物件建立時自動呼叫。它用來初始化物件。

### 建構函式的特徵

- 與類別同名
- 沒有回傳型別（連 `void` 都沒有）
- 在物件建立時自動呼叫
- 可以多載（多個版本）

### 本程式碼中的建構函式類型

**1. 預設建構函式（無參數）：**
```cpp
// 宣告
OAAdmin();

// 定義
OAAdmin::OAAdmin() {}
```

**2. 帶參數的建構函式：**
```cpp
// 宣告
OAAdmin(string username, string password);

// 定義
OAAdmin::OAAdmin(string username, string password) {
    _username = username;
    _password = password;
}
```

### 建構函式多載

擁有多個不同參數的建構函式：

```cpp
class OAAdmin {
   public:
    OAAdmin();                                    // 版本 1：無參數
    OAAdmin(string username, string password);   // 版本 2：兩個參數
};

// 使用方式：
OAAdmin admin1;                        // 呼叫版本 1
OAAdmin admin2("admin", "password");   // 呼叫版本 2
```

### 初始化列表（替代語法）

一種更有效率的初始化成員方式：

```cpp
// 目前的寫法：
OAAdmin::OAAdmin(string username, string password) {
    _username = username;  // 賦值
    _password = password;  // 賦值
}

// 更好的寫法（初始化列表）：
OAAdmin::OAAdmin(string username, string password)
    : _username(username), _password(password) {
    // 建構函式本體（在這個例子中是空的）
}
```

---

## 9. 靜態成員

### `static` 是什麼意思？

`static` 成員屬於**類別本身**，而不是任何特定的物件。

### 靜態變數

```cpp
class OAAdmin {
   public:
    static OAAdmin ERROR_ADMIN;  // 所有 OAAdmin 物件共享
};

// 必須在類別外部定義（在 .cpp 檔案中）：
OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();
```

**重點：**
- 只存在一個副本，所有實例共享
- 可以在不建立物件的情況下存取
- 必須在類別外部初始化

### 靜態方法

```cpp
class OAUtils {
   public:
    static int inputNumber();  // 可以在不建立物件的情況下呼叫
};

// 呼叫靜態方法：
int choice = OAUtils::inputNumber();  // 不需要物件！
```

### 為什麼使用 Static？

1. **工具函式**：不需要物件狀態的函式
   ```cpp
   OAUtils::inputNumber();  // 工具函式
   ```

2. **共享資料**：對所有物件應該相同的資料
   ```cpp
   OAAdmin::ERROR_ADMIN;  // 「找不到管理員」的標記值
   ```

3. **單例模式**：確保只存在一個實例
   ```cpp
   OADataCenter::getInstance();  // 回傳唯一的實例
   ```

### 靜態 vs 非靜態

| 靜態 | 非靜態 |
|------|--------|
| 屬於類別 | 屬於物件 |
| 總共只有一個副本 | 每個物件一個副本 |
| 透過 `ClassName::` 呼叫 | 透過 `object.` 呼叫 |
| 無法存取 `this` | 可以存取 `this` |
| 無法直接存取非靜態成員 | 可以存取所有成員 |

---

## 10. 友元類別

### 什麼是友元？

`friend` 類別可以存取另一個類別的私有和保護成員。

### 程式碼中的範例

```cpp
class OAAdmin {
    friend class OADataCenter;  // OADataCenter 可以存取私有成員

   private:
    string _username;
    string _password;
};
```

現在在 `OADataCenter` 中：

```cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        // 可以直接存取 admin._username 和 admin._password！
        if (admin._username == username && admin._password == password) {
            return admin;
        }
    }
    return OAAdmin::ERROR_ADMIN;
}
```

### 何時使用友元？

- 當兩個類別需要緊密合作時
- 當提供 getter/setter 方法太冗長時
- 用於運算子多載（常見用例）

### 友元的特性

- 友元關係**不是對稱的**：如果 A 是 B 的友元，B 不會自動成為 A 的友元
- 友元關係**不是傳遞的**：如果 A 是 B 的友元，B 是 C 的友元，A 不會自動成為 C 的友元
- 友元關係**不能被繼承**

---

## 11. 參考

### 什麼是參考？

參考是現有變數的別名（另一個名稱）。它使用 `&` 宣告。

### 參考宣告

```cpp
int x = 10;
int& ref = x;  // ref 現在是 x 的另一個名稱

ref = 20;      // 這會將 x 改為 20！
cout << x;     // 輸出：20
```

### 本程式碼中的參考

**回傳參考：**
```cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password);
```

這回傳對現有 `OAAdmin` 物件的參考，而不是副本。

**範圍式 for 迴圈中的參考：**
```cpp
for (OAAdmin& admin : admins) {  // admin 是每個元素的參考
    // ...
}
```

### 參考 vs 指標 vs 值

```cpp
void byValue(int x) { x = 10; }      // 修改副本
void byPointer(int* x) { *x = 10; }  // 透過指標修改原始值
void byReference(int& x) { x = 10; } // 透過參考修改原始值

int num = 5;
byValue(num);     // num 仍然是 5
byPointer(&num);  // num 現在是 10
byReference(num); // num 現在是 10
```

### 為什麼使用參考？

1. **避免複製**：大型物件的複製代價很高
   ```cpp
   void process(const vector<int>& data);  // 不會製作副本
   ```

2. **修改原始值**：允許函式修改呼叫者的變數
   ```cpp
   void increment(int& value) { value++; }
   ```

3. **回傳存取權**：回傳對內部資料的存取權
   ```cpp
   OAAdmin& loginAdmin(...);  // 回傳實際的管理員，不是副本
   ```

---

## 12. 單例設計模式

### 什麼是單例？

**單例**確保一個類別只有一個實例，並提供對它的全域存取點。

### OADataCenter 中的實作

**標頭檔（OADataCenter.h）：**
```cpp
class OADataCenter {
   private:
    static OADataCenter _instance;  // 唯一的實例
    OADataCenter();                  // 私有建構函式！

   public:
    static OADataCenter& getInstance();  // 存取點
};
```

**原始碼檔（OADataCenter.cpp）：**
```cpp
// 初始化唯一的實例
OADataCenter OADataCenter::_instance = OADataCenter();

// 回傳實例的參考
OADataCenter& OADataCenter::getInstance() {
    return _instance;
}
```

### 單例如何運作

1. **私有建構函式**：沒有人可以建立新實例
2. **靜態實例**：一個實例儲存在類別本身中
3. **公開存取器**：`getInstance()` 提供對該實例的存取

### 使用單例

```cpp
// 取得實例
OADataCenter& dataCenter = OADataCenter::getInstance();

// 使用它
OAAdmin& admin = dataCenter.loginAdmin("admin", "admin");
```

### 為什麼使用單例？

- **資料庫連線**：所有人使用一個連線池
- **設定管理器**：唯一的真實來源
- **日誌記錄**：整個應用程式使用一個日誌記錄器
- **本專案**：一個資料中心管理所有使用者/管理員

---

## 13. STL 容器（vector）

### 什麼是 STL？

STL（標準模板函式庫）提供現成的資料結構和演算法。

### 什麼是 Vector？

`vector` 是一個可以動態增長或縮小的動態陣列。

```cpp
#include <vector>

vector<int> numbers;           // 空的整數向量
vector<string> names;          // 空的字串向量
vector<OAAdmin> admins;        // 空的 OAAdmin 物件向量
```

### 本程式碼中的 Vector

```cpp
class OADataCenter {
   private:
    vector<OAAdmin> admins;  // 管理員的動態陣列
    vector<OAUser> users;    // 使用者的動態陣列
};
```

### 常見的 Vector 操作

```cpp
vector<int> v;

v.push_back(10);    // 加到尾端：[10]
v.push_back(20);    // 加到尾端：[10, 20]
v.push_back(30);    // 加到尾端：[10, 20, 30]

v.size();           // 回傳 3
v[0];               // 存取第一個元素：10
v.at(1);            // 帶邊界檢查的存取：20
v.front();          // 第一個元素：10
v.back();           // 最後一個元素：30
v.pop_back();       // 移除最後一個：[10, 20]
v.clear();          // 移除所有：[]
v.empty();          // 檢查是否為空：true
```

### Vector vs 陣列

| vector | 陣列 |
|--------|------|
| 動態大小 | 固定大小 |
| 可以增長/縮小 | 無法改變大小 |
| 自動記憶體管理 | 手動（對於動態陣列） |
| 有許多工具方法 | 只有基本存取 |

---

## 14. 輸入輸出串流

### 串流物件

```cpp
#include <iostream>

cout  // 標準輸出（主控台輸出）
cin   // 標準輸入（鍵盤輸入）
cerr  // 標準錯誤輸出
endl  // 結束行並清空緩衝區
```

### 用 `cout` 輸出

```cpp
cout << "Hello World" << endl;
cout << "Number: " << 42 << endl;
cout << "Name: " << name << ", Age: " << age << endl;
```

`<<` 運算子稱為「插入運算子」 - 它將資料插入串流中。

### 用 `cin` 輸入

```cpp
int number;
string name;

cout << "Enter a number: ";
cin >> number;  // 讀取一個整數

cout << "Enter your name: ";
cin >> name;    // 讀取一個詞（遇到空白停止）
```

`>>` 運算子稱為「提取運算子」 - 它從串流中提取資料。

### 輸入驗證（來自 OAUtils.cpp）

```cpp
int OAUtils::inputNumber() {
    int choice = -1;
    cin >> choice;

    while (cin.fail()) {         // 檢查輸入是否失敗（例如使用者輸入字母）
        cin.clear();             // 清除錯誤旗標
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // 丟棄錯誤的輸入
        cin >> choice;           // 再試一次
    }

    return choice;
}
```

**這做了什麼：**
1. 嘗試讀取一個數字
2. 如果使用者輸入無效的輸入（如「abc」），`cin.fail()` 為 true
3. 用 `cin.clear()` 清除錯誤狀態
4. 丟棄緩衝區中直到換行符的所有字元
5. 再試一次

### ANSI 跳脫碼

```cpp
cout << "\033[2J\033[H";
```

這是一個終端機控制序列：
- `\033[2J` - 清除整個螢幕
- `\033[H` - 將游標移到起始位置（左上角）

---

## 15. 流程控制語句

### 15.1 While 迴圈

```cpp
while (condition) {
    // 當條件為 true 時重複執行的程式碼
}
```

**程式碼中的範例：**
```cpp
while (1) {  // 無限迴圈（1 永遠為 true）
    // 顯示選單
    // 取得使用者輸入
    // 處理選擇
}
```

### 15.2 Switch 語句

```cpp
switch (variable) {
    case value1:
        // value1 的程式碼
        break;
    case value2:
        // value2 的程式碼
        break;
    default:
        // 沒有 case 匹配時的程式碼
}
```

**程式碼中的範例：**
```cpp
switch (choice) {
    case 1:
        showLogin(FLAG_ADMIN);
        break;
    case 2:
        showLogin(FLAG_USER);
        break;
    case 3:
        goto end;  // 跳到標籤 'end'
}
```

### 15.3 If-Else 語句

```cpp
if (condition) {
    // 條件為 true 時的程式碼
} else {
    // 條件為 false 時的程式碼
}
```

**程式碼中的範例：**
```cpp
if (flag == FLAG_ADMIN) {
    cout << "Admin Login" << endl;
} else {
    cout << "User Login" << endl;
}
```

### 15.4 Goto 語句

```cpp
goto label_name;
// ...
label_name:
    // 標籤處的程式碼
```

**程式碼中的範例：**
```cpp
while (1) {
    // ...
    case 3:
        goto end;  // 跳出迴圈
}
end:
    cout << "Thanks and see you next time!" << endl;
```

**注意**：在現代 C++ 中通常不建議使用 `goto`。更好的替代方案：
```cpp
// 使用旗標
bool running = true;
while (running) {
    // ...
    case 3:
        running = false;
        break;
}

// 或重新組織程式碼結構
```

---

## 16. main() 函式

### 程式進入點

每個 C++ 程式都從 `main()` 開始執行。

```cpp
int main() {
    OASystemUI::displayMainPage();
    return EXIT_SUCCESS;
}
```

### 回傳值

- `return 0;` 或 `return EXIT_SUCCESS;` - 程式成功完成
- `return 1;` 或任何非零值 - 程式遇到錯誤

作業系統會接收這個值：
```bash
./program
echo $?  # 顯示回傳值（成功為 0）
```

### 其他 main() 簽名

```cpp
// 基本形式
int main() { }

// 帶命令列參數
int main(int argc, char* argv[]) {
    // argc = 參數數量
    // argv = 參數值
}

// 或等效地
int main(int argc, char** argv) { }
```

---

## 17. 範圍解析運算子（::）

### 什麼是 `::`？

範圍解析運算子 `::` 指定一個名稱屬於哪個範圍。

### 本程式碼中的用法

**1. 在類別外部定義類別方法：**
```cpp
// 在標頭檔中
class OAAdmin {
    string username();
};

// 在原始碼檔中 - 必須指定這屬於哪個類別
string OAAdmin::username() {
    return _username;
}
```

**2. 存取靜態成員：**
```cpp
OAUtils::inputNumber();           // 靜態方法
OADataCenter::getInstance();      // 靜態方法
OAAdmin::ERROR_ADMIN;             // 靜態變數
```

**3. 命名空間存取：**
```cpp
std::cout << "Hello";             // std 命名空間中的 cout
std::vector<int> numbers;         // std 命名空間中的 vector
```

### 不需要範圍解析的情況

如果你在類別內部，不需要 `::`：
```cpp
class OAAdmin {
    string username() {
        return _username;  // 不需要 OAAdmin::_username
    }
};
```

---

## 18. 範圍式 for 迴圈

### 語法

```cpp
for (type element : container) {
    // 使用 element
}
```

### 程式碼中的範例

```cpp
for (OAAdmin& admin : admins) {
    if (admin._username == username && admin._password == password) {
        return admin;
    }
}
```

這等效於：
```cpp
for (int i = 0; i < admins.size(); i++) {
    OAAdmin& admin = admins[i];
    if (admin._username == username && admin._password == password) {
        return admin;
    }
}
```

### 變化形式

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

// 傳值（複製每個元素）
for (int n : numbers) { }

// 傳參考（不複製，可修改）
for (int& n : numbers) { n *= 2; }

// 傳 const 參考（不複製，唯讀）
for (const int& n : numbers) { }

// 使用 auto（編譯器推導型別）
for (auto& n : numbers) { }
```

---

## 19. 錯誤處理模式

### 標記值模式

本程式碼使用**標記值**來表示錯誤。

```cpp
class OAAdmin {
   public:
    static OAAdmin ERROR_ADMIN;  // 特殊的「錯誤」物件
};

OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;  // 成功 - 回傳實際的管理員
        }
    }
    return OAAdmin::ERROR_ADMIN;  // 失敗 - 回傳標記值
}
```

**檢查錯誤：**
```cpp
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &(OAAdmin::ERROR_ADMIN)) {
    cout << "Login failed";
} else {
    cout << "Login success";
}
```

### 替代方法（現代 C++）

**1. 例外處理：**
```cpp
OAAdmin& loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username)
            return admin;
    }
    throw runtime_error("Admin not found");
}

// 使用方式：
try {
    OAAdmin& admin = loginAdmin("name", "pass");
} catch (const runtime_error& e) {
    cout << "Error: " << e.what();
}
```

**2. std::optional（C++17）：**
```cpp
#include <optional>

std::optional<OAAdmin> loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username)
            return admin;
    }
    return std::nullopt;
}

// 使用方式：
auto result = loginAdmin("name", "pass");
if (result.has_value()) {
    OAAdmin admin = result.value();
}
```

---

## 20. 完整程式碼解析

### 系統架構

```
┌─────────────┐
│   main()    │
└──────┬──────┘
       │ 呼叫
       ▼
┌─────────────────┐
│  OASystemUI     │  （使用者介面）
│  - displayMenu  │
│  - showLogin    │
└────────┬────────┘
         │ 使用
         ▼
┌─────────────────┐         ┌─────────────┐
│  OADataCenter   │◄────────│   OAUtils   │
│  （單例）        │         │  （工具類）  │
│  - admins[]     │         │  - input    │
│  - users[]      │         └─────────────┘
│  - loginAdmin() │
└────────┬────────┘
         │ 包含
         ▼
┌─────────────────┐    ┌─────────────────┐
│    OAAdmin      │    │     OAUser      │
│  - username     │    │  - username     │
│  - password     │    │  - password     │
└─────────────────┘    │  - department   │
                       └─────────────────┘
```

### 執行流程

```
1. 程式開始 → main()
2. main() 呼叫 OASystemUI::displayMainPage()
3. displayMainPage() 在無限迴圈中顯示選單
4. 使用者輸入選擇 → OAUtils::inputNumber()
5. 根據選擇：
   - 1: showLogin(FLAG_ADMIN) → 管理員登入流程
   - 2: showLogin(FLAG_USER) → 使用者登入流程
   - 3: goto end → 退出程式
6. 登入流程：
   - 從使用者取得帳號/密碼
   - OADataCenter::getInstance() 取得單例
   - loginAdmin() 搜尋匹配的憑證
   - 回傳管理員參考或 ERROR_ADMIN
7. 顯示成功/失敗訊息
8. 返回選單
```

### 逐檔案說明

#### main.cpp
```cpp
#include <iostream>
#include "OASystemUI.h"
using namespace std;

int main() {
    OASystemUI::displayMainPage();  // 啟動應用程式
    return EXIT_SUCCESS;            // 向作業系統回傳成功
};
```
**用途**：程式進入點，啟動使用者介面。

#### OASystemUI.h / OASystemUI.cpp
**用途**：處理所有使用者介面操作。

關鍵函式 `displayMainPage()`：
- 顯示主選單
- 取得使用者輸入
- 路由到適當的函式（管理員登入、使用者登入或退出）

關鍵函式 `showLogin(int flag)`：
- 顯示登入畫面
- 收集憑證
- 透過 OADataCenter 驗證
- 顯示結果

#### OADataCenter.h / OADataCenter.cpp
**用途**：中央資料管理（單例模式）。

儲存：
- 所有管理員的列表
- 所有使用者的列表

提供：
- `getInstance()` - 存取唯一的實例
- `loginAdmin()` - 憑證驗證

#### OAAdmin.h / OAAdmin.cpp
**用途**：代表管理員。

包含：
- 帳號和密碼（私有）
- 用於建立管理員的建構函式
- 用於登入失敗的 `ERROR_ADMIN` 標記值

#### OAUser.h / OAUser.cpp
**用途**：代表一般使用者。

包含：
- 帳號、密碼和部門
- 用於建立使用者的建構函式

#### OAUtils.h / OAUtils.cpp
**用途**：工具函式。

包含：
- `inputNumber()` - 帶驗證的安全整數輸入

---

## C++ 概念總結

| 概念 | 使用位置 |
|------|----------|
| 標頭/原始碼分離 | 所有檔案 |
| `#pragma once` | 所有標頭檔 |
| `#include` | 所有檔案 |
| `#define` 巨集 | OASystemUI.cpp |
| `using namespace` | 多個檔案 |
| 類別 | OAUser、OAAdmin、OADataCenter 等 |
| 存取修飾符 | 所有類別（public/private） |
| 建構函式 | OAUser、OAAdmin、OADataCenter |
| 建構函式多載 | OAAdmin（預設 + 帶參數） |
| 靜態成員 | OAUtils、OADataCenter、OAAdmin |
| 友元類別 | OADataCenter 是 OAAdmin 的友元 |
| 參考 | loginAdmin 回傳型別、for 迴圈 |
| 單例模式 | OADataCenter |
| STL vector | OADataCenter（admins、users） |
| cin/cout | 多個檔案 |
| 流程控制（while、switch、if） | OASystemUI.cpp |
| 範圍式 for | OADataCenter.cpp |
| 範圍解析（::） | 所有 .cpp 檔案 |
| 標記值錯誤處理 | OAAdmin::ERROR_ADMIN |

---

## 後續學習建議

1. **練習**：嘗試為這個系統添加新功能
2. **記憶體管理**：學習指標、`new`、`delete`、智慧指標
3. **繼承**：學習類別階層和虛擬函式
4. **模板**：學習泛型程式設計
5. **現代 C++**：探索 C++11/14/17/20 特性
6. **STL**：學習更多容器（map、set、list）和演算法

祝學習愉快！
