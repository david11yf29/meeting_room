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
21. [指標（Pointer）](#21-指標pointer)
22. [箭頭運算子（->）](#22-箭頭運算子)
23. [迭代器（Iterator）](#23-迭代器iterator)
24. [auto 關鍵字](#24-auto-關鍵字)
25. [前向宣告（Forward Declaration）](#25-前向宣告forward-declaration)
26. [Facade 設計模式](#26-facade-設計模式)
27. [cin.get() 與 system() 呼叫](#27-cinge-與-system-呼叫)
28. [OAMeetingRoom 完整解析](#28-oameetingroom-完整解析)

---

## 1. 專案結構概覽

```
meeting_room/
├── include/           <- 標頭檔 (.h) - 宣告
│   ├── OAUtils.h
│   ├── OASystemUI.h
│   ├── OAUser.h
│   ├── OAAdmin.h
│   ├── OAMeetingRoom.h
│   └── OADataCenter.h
└── src/               <- 原始碼檔 (.cpp) - 實作
    ├── main.cpp
    ├── OAUtils.cpp
    ├── OASystemUI.cpp
    ├── OAUser.cpp
    ├── OAAdmin.cpp
    ├── OAMeetingRoom.cpp
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

**OAMeetingRoom.h 的類別定義：**
```cpp
class OAMeetingRoom {
   private:
    int _mid;       // 會議室編號
    int _capacity;  // 容納人數
    OAUser* _user;  // 指向預約者的指標（* 代表指標，稍後會詳細說明）

   public:
    OAMeetingRoom();
    OAMeetingRoom(int mid, int capacity);

    int mid();       // 回傳編號
    int capacity();  // 回傳人數
    OAUser* user();  // 回傳預約者（指標）
};
```

**建立物件（OADataCenter.cpp）：**
```cpp
// 建立 OAUser 物件
users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
users.push_back(OAUser("lisi", "123456", "HR Resource"));

// 建立 OAMeetingRoom 物件（編號, 容納人數）
rooms.push_back(OAMeetingRoom(1, 10));  // 1 號室，10 人
rooms.push_back(OAMeetingRoom(2, 6));   // 2 號室，6 人
rooms.push_back(OAMeetingRoom(3, 4));   // 3 號室，4 人
```

### 類別與物件的類比

| 類別 | 物件 |
|------|------|
| 房子的藍圖 | 實際的房子 |
| 餅乾模具 | 一塊餅乾 |
| OAUser 類別 | 特定的使用者如「zhangsan」 |
| OAMeetingRoom 類別 | 實際的 1 號會議室（10 人座） |

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
    string _username;    // 成員變數（字串）
    string _password;    // 成員變數（字串）
    string _department;  // 成員變數（字串）
};
```

OAMeetingRoom 有一個**指標型別**的成員變數，這比較特別：

```cpp
class OAMeetingRoom {
   private:
    int    _mid;      // 成員變數（整數）
    int    _capacity; // 成員變數（整數）
    OAUser* _user;    // 成員變數（指標！星號 * 代表「指向 OAUser 的指標」）
                      // 記錄「是哪個用戶預約了這間會議室」
                      // 如果沒人預約，指向特殊的 ERROR_USER 物件
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

**OAMeetingRoom 的建構函式多載（含指標初始化）：**

```cpp
// 宣告（OAMeetingRoom.h）
OAMeetingRoom();                      // 版本 1：無參數
OAMeetingRoom(int mid, int capacity); // 版本 2：兩個參數

// 定義（OAMeetingRoom.cpp）
OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
    _user = &OAUser::ERROR_USER;  // 指標初始化：指向「沒人預約」的標記物件
                                  // & 是「取地址」運算子，稍後會詳細說明
}

OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;  // 新建的會議室一定是空的，所以指向 ERROR_USER
}
```

這裡的重點：**所有新建的會議室，`_user` 指標都初始化為指向 `OAUser::ERROR_USER`**，代表「目前沒有人預約」。

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
    static OAAdmin ERROR_ADMIN;  // 所有 OAAdmin 物件共享的「登入失敗」標記
};

// 必須在類別外部定義（在 .cpp 檔案中）：
OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();
```

**OAUser 也有相同的靜態標記物件：**
```cpp
// OAUser.h
class OAUser {
   public:
    static OAUser ERROR_USER;  // 「找不到此用戶」或「會議室沒有人預約」的標記
};

// OAUser.cpp（雖然未在上面展示，但原理相同）
OAUser OAUser::ERROR_USER = OAUser();
```

`ERROR_USER` 有兩個用途：
1. `loginUser()` 找不到用戶時回傳它
2. `OAMeetingRoom::_user` 指向它，代表「這間會議室沒有人預約」

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
   OAUser::ERROR_USER;    // 「找不到用戶」或「會議室空著」的標記值
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

本專案有**三個**類別都宣告 `OADataCenter` 為友元：

**OAAdmin.h：**
```cpp
class OAAdmin {
    friend class OADataCenter;  // OADataCenter 可以存取私有成員

   private:
    string _username;
    string _password;
};
```

**OAUser.h：**
```cpp
class OAUser {
    friend class OADataCenter;  // OADataCenter 可以存取私有成員

   private:
    string _username;
    string _password;
    string _department;
};
```

**OAMeetingRoom.h：**
```cpp
class OAMeetingRoom {
    friend class OADataCenter;  // OADataCenter 可以直接存取 _mid、_capacity、_user

   private:
    int _mid;
    int _capacity;
    OAUser* _user;
};
```

現在在 `OADataCenter` 中，可以直接存取所有這些私有成員：

```cpp
// 直接讀 admin._username（因為 friend）
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;
        }
    }
    return OAAdmin::ERROR_ADMIN;
}

// 直接讀 user._username（因為 friend）
OAUser& OADataCenter::loginUser(string username, string password) {
    for (OAUser& user : users) {
        if (user._username == username && user._password == password) {
            return user;
        }
    }
    return OAUser::ERROR_USER;
}

// 直接寫 room._user（因為 friend）
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 直接修改私有指標成員
    }
}

// 直接讀 room._mid（因為 friend）
bool OADataCenter::addMeetingRoom(int mid, int capacity) {
    for (OAMeetingRoom& room : rooms) {
        if (room._mid == mid) {  // 直接存取私有 _mid
            return false;
        }
    }
    rooms.push_back(OAMeetingRoom(mid, capacity));
    return true;
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
   OAUser&  loginUser(...);   // 回傳實際的用戶，不是副本
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
// 初始化唯一的實例（程式啟動時執行一次，呼叫私有建構函式）
OADataCenter OADataCenter::_instance = OADataCenter();

// 私有建構函式：預載入所有初始資料
OADataCenter::OADataCenter() {
    // 預設管理員
    admins.push_back(OAAdmin("admin", "admin"));

    // 預設用戶
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    // 預設會議室（ID, 容納人數）
    rooms.push_back(OAMeetingRoom(1, 10));
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));
}

// 回傳實例的參考
OADataCenter& OADataCenter::getInstance() {
    return _instance;
}
```

### 單例如何運作

1. **私有建構函式**：沒有人可以從外部建立新實例
2. **靜態實例**：一個實例儲存在類別本身中，程式啟動時自動建立
3. **公開存取器**：`getInstance()` 提供對該唯一實例的存取

### 使用單例

```cpp
// 取得實例（每次都回傳同一個物件）
OADataCenter& dataCenter = OADataCenter::getInstance();

// 使用它
OAAdmin& admin = dataCenter.loginAdmin("admin", "admin");
OAUser&  user  = dataCenter.loginUser("zhangsan", "123123");
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
    vector<OAAdmin>       admins;  // 管理員的動態陣列
    vector<OAUser>        users;   // 使用者的動態陣列
    vector<OAMeetingRoom> rooms;   // 會議室的動態陣列
};
```

這三個 vector 在 `OADataCenter` 建構函式中被填入初始資料：

```cpp
OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));          // 加入 1 個管理員
    users.push_back(OAUser("zhangsan", "123123", "..."));  // 加入 2 個用戶
    users.push_back(OAUser("lisi", "123456", "..."));
    rooms.push_back(OAMeetingRoom(1, 10));                 // 加入 3 間會議室
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));
}
```

**回傳 vector 的副本（getUsers / getRooms）：**

```cpp
vector<OAUser> OADataCenter::getUsers() {
    vector<OAUser> users_back(users);  // 用複製建構函式建立一份副本
    return users_back;
}

vector<OAMeetingRoom> OADataCenter::getRooms() {
    return vector<OAMeetingRoom>(rooms);  // 直接回傳副本
}
```

為什麼回傳副本？因為這樣外部程式碼修改拿到的 vector 時，不會影響 `OADataCenter` 內部的真實資料。

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

**主選單的範例（OASystemUI.cpp）：**
```cpp
switch (choice) {
    case 1:
        showLogin(FLAG_ADMIN);   // 管理員登入
        break;
    case 2:
        showLogin(FLAG_USER);    // 一般用戶登入
        break;
    case 3:
        goto end;                // 跳到標籤 'end'（退出程式）
}
```

**管理員選單的範例（有 8 個選項）：**
```cpp
switch (c) {
    case 1:
        showAdminAddUserPage(admin);       // 新增用戶
        break;
    case 2:
        showAdminDeleteUserPage(admin);    // 刪除用戶
        break;
    case 3:
        showAdminListUsers(admin);         // 列出所有用戶
        break;
    case 4:
        showAdminAddMeetingRoomPage(admin); // 新增會議室
        break;
    case 5:   // 刪除會議室（未完整實作）
    case 6:   // 列出所有會議室
    case 7:   // 重置所有會議室狀態
    case 8:   // 登出
        goto end;   // 這幾個 case 目前都跳出（注意：沒有 break 就會 fall through！）
}
```

**注意 Fall-Through（貫穿）現象：**

當多個 `case` 沒有 `break`，執行到其中一個就會連續往下執行：
```cpp
case 5:    // 如果選 5
case 6:    // 或 6
case 7:    // 或 7
case 8:    // 或 8
    goto end;  // 全部都執行這個
```

這是 C++ switch 的特性——沒有 `break` 就會「貫穿」到下一個 case。

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

本程式碼使用**標記值**來表示錯誤狀態。這個模式在本專案中出現在**三個地方**。

**場景 1：管理員登入失敗（回傳參考）**
```cpp
class OAAdmin {
   public:
    static OAAdmin ERROR_ADMIN;  // 特殊的「登入失敗」物件
};

OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;           // 成功：回傳實際管理員的參考
        }
    }
    return OAAdmin::ERROR_ADMIN;   // 失敗：回傳標記物件的參考
}
```

**場景 2：一般用戶登入失敗（回傳參考）**
```cpp
class OAUser {
   public:
    static OAUser ERROR_USER;  // 特殊的「登入失敗」物件
};

OAUser& OADataCenter::loginUser(string username, string password) {
    for (OAUser& user : users) {
        if (user._username == username && user._password == password) {
            return user;           // 成功：回傳實際用戶的參考
        }
    }
    return OAUser::ERROR_USER;    // 失敗：回傳標記物件的參考
}
```

**場景 3：會議室的預約狀態（指標指向標記物件）**
```cpp
class OAMeetingRoom {
   private:
    OAUser* _user;  // 指向預約者；若指向 ERROR_USER 則代表沒人預約
};

// 建立會議室時，預設「沒人預約」
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _user = &OAUser::ERROR_USER;  // 指標指向 ERROR_USER 物件
}

// 清除所有預約
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 把指標重新指回 ERROR_USER
    }
}
```

**檢查錯誤——三種比較方式：**
```cpp
// 比較「管理員登入」的結果（比較參考的地址）
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &(OAAdmin::ERROR_ADMIN)) {
    cout << "Admin login failed";
} else {
    cout << "Admin login success: " << admin.username();
}

// 比較「用戶登入」的結果（比較參考的地址）
OAUser& user = dataCenter.loginUser(username, password);
if (&user == &(OAUser::ERROR_USER)) {
    cout << "User login failed";
} else {
    cout << "Welcome " << user.username();
}

// 比較「會議室是否有人預約」（比較指標的值）
if (m.user() == &(OAUser::ERROR_USER)) {
    cout << "reservable!";   // 指標指向 ERROR_USER -> 沒人預約
} else {
    cout << m.user()->username() << " reserved it";  // 有人預約
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
┌──────────────────────┐         ┌─────────────┐
│      OASystemUI      │────────▶│   OAUtils   │
│  - displayMainPage() │         │ - inputNumber│
│  - showLogin()       │         └─────────────┘
│  - showAdminMainPage │
│  - showAdminAddUser  │
│  - showAdminDelUser  │
│  - showAdminAddRoom  │
└──────────┬───────────┘
           │ 透過 OAAdmin 操作
           ▼
┌──────────────────────┐
│       OAAdmin        │  ← Facade（門面）
│  - username()        │
│  - addUser()         │  ── 委託 ──▶ OADataCenter
│  - deleteUser()      │
│  - showAllUsers()    │
│  - addMeetingRoom()  │
│  - deleteMeetingRoom │
│  - clearRoomStatus() │
│  - showAllRooms()    │
│  + ERROR_ADMIN       │
└──────────────────────┘
           │ getInstance() 直接呼叫
           ▼
┌──────────────────────┐
│    OADataCenter      │  ← Singleton（單例）
│  - admins: vector    │
│  - users: vector     │
│  - rooms: vector     │
│  + getInstance()     │
│  - loginAdmin()      │
│  - loginUser()       │
│  - addUser()         │
│  - deleteUser()      │
│  - addMeetingRoom()  │
│  - deleteMeetingRoom │
│  - clearRoomStatus() │
│  - getUsers()        │
│  - getRooms()        │
└──────┬───────────────┘
       │ 包含（vector）
       ▼
┌────────────┐   ┌──────────────┐   ┌──────────────────┐
│  OAAdmin   │   │   OAUser     │   │  OAMeetingRoom   │
│ _username  │   │ _username    │   │ _mid             │
│ _password  │   │ _password    │   │ _capacity        │
│ ERROR_ADMIN│   │ _department  │   │ _user: OAUser*──▶│
└────────────┘   │ ERROR_USER   │   └──────────────────┘
                 └──────────────┘
                        ▲
                        │（_user 指標指向此處）
                  OAUser::ERROR_USER
                  （表示「沒人預約」）
```

### 執行流程

```
1. 程式開始 → main()
2. main() 呼叫 OASystemUI::displayMainPage()
3. displayMainPage() 進入無限迴圈，顯示主選單：
   - 1. Admin Login
   - 2. User Login
   - 3. Quit
4. 使用者輸入選擇 → OAUtils::inputNumber()（帶驗證）

─── 選 1（管理員登入）───
5a. showLogin(FLAG_ADMIN)：
    - 顯示登入畫面，讀取帳號/密碼
    - OADataCenter::getInstance().loginAdmin() 驗證
    - 失敗：印出錯誤訊息，返回主選單
    - 成功：進入 showAdminMainPage()

6a. showAdminMainPage() 進入管理員選單（無限迴圈）：
    - 1. Add user    → showAdminAddUserPage()
                        → admin.addUser() → OADataCenter::addUser()
    - 2. Delete user → showAdminDeleteUserPage()
                        → admin.deleteUser() → OADataCenter::deleteUser()
    - 3. List users  → admin.showAllUsers() → OADataCenter::getUsers()
    - 4. Add room    → showAdminAddMeetingRoomPage()
                        → admin.addMeetingRoom() → OADataCenter::addMeetingRoom()
    - 5-8. goto end → 登出，返回主選單

─── 選 2（用戶登入）───
5b. showLogin(FLAG_USER)：
    - 顯示登入畫面，讀取帳號/密碼
    - OADataCenter::getInstance().loginUser() 驗證
    - 失敗：印出錯誤訊息
    - 成功：印出歡迎訊息（目前沒有更多操作）

─── 選 3（退出）───
5c. goto end → 印出告別訊息，程式結束
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
**用途**：處理所有使用者介面操作（View 層）。

| 函式 | 用途 |
|------|------|
| `displayMainPage()` | 主選單（無限迴圈）；轉發到 showLogin |
| `showLogin(int flag)` | 讀取帳號密碼，呼叫 OADataCenter 驗證 |
| `showAdminMainPage(OAAdmin&)` | 管理員選單（無限迴圈）；8 個操作 |
| `showAdminAddUserPage(OAAdmin&)` | 讀取資料，呼叫 admin.addUser() |
| `showAdminDeleteUserPage(OAAdmin&)` | 讀取用戶名，呼叫 admin.deleteUser() |
| `showAdminListUsers(OAAdmin&)` | 呼叫 admin.showAllUsers() |
| `showAdminAddMeetingRoomPage(OAAdmin&)` | 讀取 ID 和容量，呼叫 admin.addMeetingRoom() |

`#define FLAG_ADMIN 1` 和 `#define FLAG_USER 0` 也定義在這裡，用來區分登入類型。

#### OADataCenter.h / OADataCenter.cpp
**用途**：中央資料管理（單例模式）；所有真實的資料操作都在這裡。

儲存：
- `vector<OAAdmin> admins` - 所有管理員
- `vector<OAUser> users` - 所有用戶
- `vector<OAMeetingRoom> rooms` - 所有會議室

提供的方法：
| 方法 | 用途 | 回傳值 |
|------|------|--------|
| `getInstance()` | 取得單例實例 | `OADataCenter&` |
| `loginAdmin(user, pw)` | 驗證管理員 | `OAAdmin&`（失敗回傳 ERROR_ADMIN） |
| `loginUser(user, pw)` | 驗證用戶 | `OAUser&`（失敗回傳 ERROR_USER） |
| `addUser(user, pw, dept)` | 新增用戶（檢查重複） | `bool` |
| `deleteUser(user)` | 刪除用戶（用迭代器） | `bool` |
| `addMeetingRoom(id, cap)` | 新增會議室（檢查重複） | `bool` |
| `deleteMeetingRoom(id)` | 刪除會議室（用迭代器） | `bool` |
| `clearMeetingRoomStatus()` | 清除所有預約（指標重設為 ERROR_USER） | `void` |
| `getUsers()` | 取得用戶列表副本 | `vector<OAUser>` |
| `getRooms()` | 取得會議室列表副本 | `vector<OAMeetingRoom>` |

#### OAAdmin.h / OAAdmin.cpp
**用途**：代表管理員；同時也是 Facade（門面），包裝 OADataCenter 的操作。

包含：
- `_username`、`_password`（私有，friend OADataCenter 可直接讀取）
- `static OAAdmin ERROR_ADMIN` — 登入失敗的標記物件
- `username()` — 取得管理員名稱

所有管理操作都委託給 `OADataCenter`：
| 方法 | 實際呼叫 |
|------|---------|
| `addUser()` | `OADataCenter::getInstance().addUser()` |
| `deleteUser()` | `OADataCenter::getInstance().deleteUser()` |
| `showAllUsers()` | `OADataCenter::getInstance().getUsers()` 然後印出 |
| `addMeetingRoom()` | `OADataCenter::getInstance().addMeetingRoom()` |
| `deleteMeetingRoom()` | `OADataCenter::getInstance().deleteMeetingRoom()` |
| `clearMeetingRoomStatus()` | `OADataCenter::getInstance().clearMeetingRoomStatus()` |
| `showAllMeetingRooms()` | `OADataCenter::getInstance().getRooms()` 然後印出，用 `->` 存取預約者資料 |

#### OAUser.h / OAUser.cpp
**用途**：代表一般使用者（純資料類別）。

包含：
- `_username`、`_password`、`_department`（私有，friend OADataCenter 可直接讀取）
- `static OAUser ERROR_USER` — 兩種用途：登入失敗的標記、會議室「空房」的標記
- `username()`、`department()` — getter 方法

#### OAMeetingRoom.h / OAMeetingRoom.cpp
**用途**：代表一間會議室（純資料類別）。

包含：
- `_mid`（整數）— 會議室編號，friend OADataCenter 可直接讀取
- `_capacity`（整數）— 容納人數
- `_user`（`OAUser*` 指標）— 指向預約者；`= &OAUser::ERROR_USER` 代表空房
- `mid()`、`capacity()`、`user()` — getter 方法（`user()` 回傳指標）

#### OAUtils.h / OAUtils.cpp
**用途**：工具函式（純靜態方法）。

包含：
- `inputNumber()` — 帶驗證的安全整數輸入：嘗試讀取整數，如果輸入非數字（如 "abc"）則清除錯誤、丟棄壞輸入、重試

---

## 21. 指標（Pointer）

### 什麼是指標？

想像你家的地址。**地址**本身不是你的家，它只是告訴你家在哪裡。**指標**就是記憶體地址——它不是物件本身，而是告訴你物件住在記憶體的哪個位置。

```
記憶體（想像成一排房子）：

地址 1000 → [OAUser "zhangsan"]
地址 1004 → [OAUser "lisi"]
地址 1008 → [OAUser "ERROR_USER"]

OAUser* _user = 1008;  // _user「指向」ERROR_USER 住的地址
```

### 宣告指標

```cpp
int x = 42;
int* p;   // p 是一個「指向 int 的指標」
          // 星號 * 放在型別後面表示「這是指標」
```

**本專案中的範例（OAMeetingRoom.h）：**
```cpp
class OAMeetingRoom {
   private:
    OAUser* _user;  // 指向 OAUser 物件的指標
                    // 代表「是誰預約了這個會議室」
};
```

### 取得地址（& 運算子）

`&` 放在變數前面，意思是「給我這個變數的記憶體地址」：

```cpp
int x = 42;
int* p = &x;  // p = x 的地址（比如 1000）

OAUser* _user = &OAUser::ERROR_USER;  // 指向 ERROR_USER 物件的地址
```

### 解參考（* 運算子）

`*` 放在指標前面，意思是「去那個地址，把那裡的東西拿給我」：

```cpp
int x = 42;
int* p = &x;   // p 存著 x 的地址
int y = *p;    // *p = 「去 p 的地址，取得那裡的值」= 42
```

### 指標在本專案中的用途

**OAMeetingRoom.cpp 中的初始化：**
```cpp
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;  // 房間剛建立，沒有人預約
                                  // 讓 _user 指向「錯誤用戶」代表「空房間」
}
```

**OADataCenter.cpp 中的清除預約：**
```cpp
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 把每間房間都設為「沒人預約」
    }
}
```

### 指標比較

```cpp
// 這是本專案中檢查「房間是否空著」的方式：
if (m.user() == &(OAUser::ERROR_USER)) {
    cout << "reservable!" << endl;  // 指標指向 ERROR_USER -> 沒人預約
} else {
    cout << "reserved!" << endl;    // 指標指向真實用戶 -> 有人預約了
}
```

**解釋：** `m.user()` 回傳一個 `OAUser*` 指標。我們把它和 `&OAUser::ERROR_USER`（ERROR_USER 的地址）比較。如果兩個地址相同，代表這個房間「指向」ERROR_USER，也就是「空房間」。

### 登入驗證中的指標比較

```cpp
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &(OAAdmin::ERROR_ADMIN)) {
    cout << "Login failed";
}
```

- `loginAdmin()` 回傳 `OAAdmin&`（參考）
- `&admin` 取得這個參考所指向的物件的地址
- 如果地址等於 `ERROR_ADMIN` 的地址，代表登入失敗

### 指標 vs 參考 vs 值

| 特性 | 值 | 參考（&） | 指標（*） |
|------|----|-----------|---------|
| 可以為空 | 不行 | 不行 | 可以（nullptr） |
| 可以重新指向 | — | 不行（一旦綁定就固定） | 可以 |
| 語法 | `OAUser u` | `OAUser& r = u` | `OAUser* p = &u` |
| 存取成員 | `u.method()` | `r.method()` | `p->method()` |

---

## 22. 箭頭運算子（->）

### 什麼是 -> ？

當你有一個**指標**，而你想呼叫它所指向物件的方法，你有兩種方式：

```cpp
OAUser* p = &someUser;

// 方法一：先解參考再用點運算子（麻煩）
(*p).username();

// 方法二：用箭頭運算子（簡潔）
p->username();
```

`p->method()` 就是 `(*p).method()` 的簡寫。

### 本專案中的範例

**OAAdmin.cpp 中的 showAllMeetingRooms()：**
```cpp
void OAAdmin::showAllMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& m : rooms) {
        if (m.user() == &(OAUser::ERROR_USER)) {
            cout << ", reservable!" << endl;
        } else {
            // m.user() 回傳 OAUser*（指標）
            // 所以用 -> 來呼叫指標所指物件的方法
            cout << m.user()->department()   // OAUser* -> department()
                 << m.user()->username()     // OAUser* -> username()
                 << endl;
        }
    }
}
```

### 記憶口訣

- **點運算子 `.`** → 直接使用物件或參考時用
- **箭頭運算子 `->`** → 透過指標存取物件時用

```cpp
OAUser  u;   // 物件（值）
OAUser& r = u;  // 參考
OAUser* p = &u; // 指標

u.username();   // 用點
r.username();   // 用點（參考和值一樣）
p->username();  // 用箭頭（指標）
```

---

## 23. 迭代器（Iterator）

### 什麼是迭代器？

迭代器是一個「位置標記」，告訴你現在在容器的哪個位置。你可以把它想像成書籤——它記住你讀到哪裡了，而且可以向前翻頁。

```
vector<OAUser>: [zhangsan] [lisi] [wangwu]
                    ↑
                  迭代器（目前指向 zhangsan）
```

### 宣告迭代器

```cpp
vector<OAUser> users;

// 方法一：明確寫出型別
vector<OAUser>::iterator it = users.begin();

// 方法二：用 auto（編譯器自動推導型別，更簡潔）
auto it = users.begin();
```

### begin() 和 end()

```cpp
vector<int> v = {10, 20, 30};
//               ↑           ↑
//           begin()       end()（指向最後一個元素之後的位置！）
```

**重要**：`end()` 不指向最後一個元素，而是指向「超出末尾」的位置。這是 C++ 的慣例，用來表示「已經到達終點」。

### 用迭代器遍歷

```cpp
vector<OAUser> users;
for (auto it = users.begin(); it != users.end(); it++) {
    // *it 取得目前位置的元素
    cout << (*it).username() << endl;
    // 或
    cout << it->username() << endl;  // 迭代器也可以用 ->
}
```

### 本專案中的迭代器用法

**OADataCenter.cpp 中的 deleteUser()：**
```cpp
bool OADataCenter::deleteUser(string username) {
    auto it = users.begin();       // 從頭開始
    for (; it != users.end(); it++) {  // 一個一個往後走
        if (it->username() == username) {  // 找到了！
            break;                 // 停下來，it 現在指向要刪除的元素
        }
    }

    if (it == users.end()) {       // 走完了都沒找到
        return false;
    }

    users.erase(it);               // 刪除 it 指向的元素
    return true;
}
```

**deleteMeetingRoom() 的另一種寫法（明確型別）：**
```cpp
bool OADataCenter::deleteMeetingRoom(int mid) {
    vector<OAMeetingRoom>::iterator it = rooms.begin();  // 明確寫出迭代器型別
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }
    if (it == rooms.end()) return false;
    rooms.erase(it);
    return true;
}
```

### erase() 方法

`erase()` 接受一個迭代器，刪除它所指向的元素，並自動把後面的元素往前移：

```cpp
vector<int> v = {1, 2, 3, 4, 5};
auto it = v.begin() + 2;  // 指向 3
v.erase(it);              // 刪除 3
// v 現在是 {1, 2, 4, 5}
```

### 範圍式 for vs 迭代器 for

```cpp
// 範圍式 for（簡單遍歷，不能刪除）
for (OAUser& u : users) {
    cout << u.username();
}

// 迭代器 for（可以搭配 erase 刪除元素）
for (auto it = users.begin(); it != users.end(); it++) {
    if (it->username() == "delete_me") {
        users.erase(it);  // 刪除後 it 就失效了，所以要 break
        break;
    }
}
```

---

## 24. auto 關鍵字

### 什麼是 auto？

`auto` 告訴編譯器：「你自己去推斷這個變數是什麼型別，我懶得寫。」

```cpp
auto x = 42;           // 編譯器知道 x 是 int
auto name = "Alice";   // 編譯器知道 name 是 const char*
auto it = v.begin();   // 編譯器知道 it 是 vector<OAUser>::iterator
```

### 為什麼需要 auto？

有些型別名字很長，很難寫：

```cpp
// 不用 auto：很長
vector<OAMeetingRoom>::iterator it = rooms.begin();

// 用 auto：簡潔
auto it = rooms.begin();
```

兩者完全等效，但 `auto` 更簡潔，也更不容易打錯。

### 本專案中的 auto

**OADataCenter.cpp 中的 deleteUser()：**
```cpp
auto it = users.begin();
// 等同於：
// vector<OAUser>::iterator it = users.begin();
```

### auto 的限制

- `auto` **必須**在宣告時就初始化（因為編譯器需要看右邊的值才能推斷型別）
- 不能用 `auto` 宣告函式參數（在 C++20 之前）

```cpp
auto x;        // 錯誤！沒有初始值，編譯器不知道型別
auto x = 5;    // 正確！推斷為 int
```

---

## 25. 前向宣告（Forward Declaration）

### 什麼是前向宣告？

有時候兩個類別互相需要對方，這會造成「循環引入」的問題。前向宣告是一種說「我保證這個類別存在，但詳細定義在別的地方」的方式。

### 問題：循環依賴

```
OADataCenter.h 引入了 OAAdmin.h
OAAdmin.h 又引入了 OADataCenter.h  ← 循環！
```

### 解決方案：前向宣告

**OADataCenter.h 中的寫法：**
```cpp
#pragma once
#include <vector>
#include "OAAdmin.h"
#include "OAUser.h"
#include "OAMeetingRoom.h"

// 前向宣告：「我保證這些類別存在」
class OAAdmin;
class OAUser;
class OAMeetingRoom;

class OADataCenter {
   private:
    vector<OAAdmin> admins;   // 編譯器現在知道 OAAdmin 存在，可以用
    vector<OAUser> users;
    // ...
};
```

**OAMeetingRoom.h 中的寫法：**
```cpp
#pragma once
#include "OAUser.h"
#include "OADataCenter.h"

class OAUser;  // 前向宣告：「OAUser 這個類別存在」

class OAMeetingRoom {
   private:
    OAUser* _user;  // 可以宣告指向 OAUser 的指標（因為指標大小固定）
};
```

### 為什麼指標可以用前向宣告，值不行？

```cpp
class Foo;  // 前向宣告

Foo f;      // 錯誤！編譯器需要知道 Foo 有多大才能分配空間
Foo* p;     // 正確！指標大小固定（通常 8 bytes），不需要知道 Foo 的細節
```

這就是為什麼 `OAMeetingRoom` 用 `OAUser*`（指標）而不是 `OAUser`（值）的原因之一。

---

## 26. Facade 設計模式

### 什麼是 Facade（門面）模式？

Facade 模式就像飯店的前台接待員。客人不需要知道「廚房、清潔部、保安部」的細節，只要跟接待員說「我要一杯咖啡」，接待員會幫你協調好一切。

### OAAdmin 就是 Facade

`OAAdmin` 是一個「前台」，幫 `OASystemUI` 跟 `OADataCenter` 溝通。UI 不需要直接跟資料中心打交道：

```
OASystemUI （客人）
    ↓
OAAdmin （接待員 / Facade）
    ↓
OADataCenter （後台）
```

**OAAdmin.cpp 的實作：**
```cpp
// OAAdmin 自己什麼都沒做，全部委託給 OADataCenter
bool OAAdmin::addUser(string username, string password, string department) {
    return OADataCenter::getInstance().addUser(username, password, department);
    //     ^^^^^^^^^^^^^^^^^^^^^^^^^^^ 委託給資料中心做實際工作
}

bool OAAdmin::deleteUser(string username) {
    return OADataCenter::getInstance().deleteUser(username);
}

void OAAdmin::showAllUsers() {
    vector<OAUser> users = OADataCenter::getInstance().getUsers();
    // 然後顯示資料
    for (OAUser& u : users) {
        cout << "User: " << u.username() << ", Department: " << u.department() << endl;
    }
}
```

### Facade 的好處

1. **簡化介面**：`OASystemUI` 只需要呼叫 `admin.addUser()`，不需要知道資料如何儲存
2. **降低耦合**：如果未來 `OADataCenter` 改了，只需要更新 `OAAdmin`，不用改 UI
3. **更好的組織**：每個類別只負責一件事

---

## 27. cin.get() 與 system() 呼叫

### cin.get() — 等待使用者按 Enter

```cpp
cout << "Press Enter to continue...";
cin.get();  // 讀取一個字元（等使用者按 Enter）
```

**為什麼需要這個？**

程式執行太快，使用者來不及看輸出。`cin.get()` 讓程式暫停，等使用者按下 Enter 再繼續。

**一個常見的陷阱：**

```cpp
cin >> choice;    // 讀取數字後，Enter 鍵還留在緩衝區
cin.get();        // 第一次呼叫只是消耗掉那個 Enter
cin.get();        // 第二次才真的等使用者
```

這是為什麼有些地方用了 `cin.ignore()` 加 `cin.get()`。

### system("clear") — 清除螢幕

```cpp
system("clear");  // 在 macOS/Linux 上清除終端機螢幕
```

**system() 是什麼？**

`system()` 讓你的 C++ 程式執行一個作業系統指令。就像你在終端機輸入指令一樣。

```cpp
system("clear");  // 相當於在終端機輸入 clear
system("ls");     // 相當於在終端機輸入 ls（列出檔案）
```

**缺點和注意事項：**
- `system("clear")` 只在 macOS/Linux 有效；Windows 要用 `system("cls")`
- 這是「平台依賴」的程式碼——在不同作業系統可能無法運作
- 本專案也用了 ANSI 跳脫碼的替代方式：`cout << "\033[2J\033[H";`

### \033[2J\033[H — ANSI 跳脫碼清除螢幕

```cpp
cout << "\033[2J\033[H";
// \033  = ESC 字元（ASCII 27）
// [2J   = 清除整個螢幕
// [H    = 把游標移到左上角
```

這是另一種清除螢幕的方式，比 `system("clear")` 更「純 C++」，不需要呼叫作業系統指令。

---

## 28. OAMeetingRoom 完整解析

### 類別結構

**OAMeetingRoom.h：**
```cpp
class OAMeetingRoom {
    friend class OADataCenter;  // 允許 OADataCenter 存取私有成員

   private:
    int _mid;       // 會議室編號（Meeting Room ID）
    int _capacity;  // 容納人數
    OAUser* _user;  // 指向預約者的指標
                    // 如果指向 ERROR_USER → 沒人預約（空房）
                    // 如果指向真實 OAUser → 有人預約了

   public:
    OAMeetingRoom();                      // 預設建構函式
    OAMeetingRoom(int mid, int capacity); // 帶參數建構函式

    int mid();       // 回傳房間編號
    int capacity();  // 回傳容納人數
    OAUser* user();  // 回傳預約者的指標（注意：回傳指標）
};
```

### 為什麼用指標（OAUser*）而不是值（OAUser）？

1. **表達「沒有人」的狀態**：指標可以指向特殊的 `ERROR_USER` 來表示「空房間」
2. **避免複製**：如果用值，每次 OAUser 修改資料，OAMeetingRoom 裡的副本就過時了
3. **前向宣告需求**：OAMeetingRoom.h 需要在知道 OAUser 完整定義前就使用它

### 建構函式實作

**OAMeetingRoom.cpp：**
```cpp
OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
    _user = &OAUser::ERROR_USER;  // 預設：沒有人預約
}

OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;  // 新建的房間：沒有人預約
}
```

### 預約狀態的設計

```
OAMeetingRoom._user 的兩種狀態：

狀態一：沒人預約
  _user ──→ OAUser::ERROR_USER（靜態「錯誤」物件）

狀態二：有人預約
  _user ──→ OAUser("zhangsan", "123123", "Admin Resource")
```

**判斷是否有人預約（OAAdmin.cpp）：**
```cpp
if (m.user() == &(OAUser::ERROR_USER)) {
    cout << ", reservable!" << endl;   // 空房
} else {
    cout << m.user()->department()     // 有人，顯示預約者資訊
         << "'s " << m.user()->username() << endl;
}
```

### 初始資料（OADataCenter.cpp）

系統啟動時自動建立 3 間會議室：

```cpp
OADataCenter::OADataCenter() {
    // 用戶資料
    admins.push_back(OAAdmin("admin", "admin"));
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    // 會議室資料
    rooms.push_back(OAMeetingRoom(1, 10));  // 1 號室，10 人
    rooms.push_back(OAMeetingRoom(2, 6));   // 2 號室，6 人
    rooms.push_back(OAMeetingRoom(3, 4));   // 3 號室，4 人
}
```

### getUsers() 和 getRooms() — 回傳副本

```cpp
vector<OAUser> OADataCenter::getUsers() {
    vector<OAUser> users_back(users);  // 複製整個 vector
    return users_back;                 // 回傳副本
}

vector<OAMeetingRoom> OADataCenter::getRooms() {
    return vector<OAMeetingRoom>(rooms);  // 同樣回傳副本
}
```

**為什麼回傳副本而不是參考？**

為了安全！如果回傳參考，外部程式碼可能會修改內部資料，繞過 `OADataCenter` 的保護。回傳副本就像「影印機」——你拿到的是複印件，不是原始文件。

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
| 標記值錯誤處理 | OAAdmin::ERROR_ADMIN、OAUser::ERROR_USER |
| 指標（`*`） | OAMeetingRoom._user（`OAUser*`） |
| 取地址（`&`） | `&OAUser::ERROR_USER`、`&admin` |
| 箭頭運算子（`->`） | `m.user()->username()` |
| 迭代器 | `deleteUser()`、`deleteMeetingRoom()` 中 |
| `auto` 關鍵字 | `auto it = users.begin()` |
| 前向宣告 | `OADataCenter.h`、`OAMeetingRoom.h` |
| Facade 設計模式 | OAAdmin 委託 OADataCenter |
| `cin.get()` | 暫停等待使用者按 Enter |
| `system("clear")` | 清除終端機螢幕 |

---

## 後續學習建議

1. **練習**：嘗試實作「用戶預約會議室」的功能（目前只有顯示，沒有實際預約邏輯）
2. **指標進階**：學習 `new`/`delete` 動態分配記憶體、`nullptr`、智慧指標（`unique_ptr`、`shared_ptr`）
3. **繼承**：學習類別階層（`OAAdmin` 和 `OAUser` 其實可以繼承同一個基底類別）和虛擬函式
4. **模板**：學習泛型程式設計（`vector<T>` 本身就是模板的應用）
5. **現代 C++**：探索 C++11/14/17/20 特性，包括 `std::optional`、`std::variant`
6. **STL**：學習更多容器（`map` 用來建立 ID 到物件的映射、`set`、`list`）和演算法
7. **例外處理**：`try`/`catch`/`throw` — 比本專案的標記值模式更現代的錯誤處理方式
8. **資料持久化**：使用檔案 I/O（`fstream`）儲存資料，讓重啟後資料不會消失

祝學習愉快！
