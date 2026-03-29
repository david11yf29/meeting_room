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
27. [cin.get() 與 system() 呼叫](#27-cinget-與-system-呼叫)
28. [OAMeetingRoom 完整解析](#28-oameetingroom-完整解析)
29. [this 指標](#29-this-指標)
30. [new 關鍵字與動態記憶體](#30-new-關鍵字與動態記憶體)

---

## 1. 專案結構概覽

```
meeting_room/
├── include/           <- 標頭檔 (.h) - 宣告
│   ├── OAUtils.h          工具函式 + 預約狀態代碼
│   ├── OASystemUI.h       使用者介面（UI）
│   ├── OAUser.h           一般使用者類別
│   ├── OAAdmin.h          管理員類別
│   ├── OAMeetingRoom.h    會議室類別
│   └── OADataCenter.h     資料中心（單例）
└── src/               <- 原始碼檔 (.cpp) - 實作
    ├── main.cpp           程式進入點
    ├── OAUtils.cpp        工具函式實作
    ├── OASystemUI.cpp     UI 函式實作（所有畫面）
    ├── OAUser.cpp         使用者實作（含預約功能）
    ├── OAAdmin.cpp        管理員實作（含門面方法）
    ├── OAMeetingRoom.cpp  會議室實作
    └── OADataCenter.cpp   資料中心實作（含預約/取消邏輯）
```

**資料流向（把它想成一個公司的組織架構）：**

```
main.cpp
   ↓ 啟動
OASystemUI.cpp  ← 前台接待員（負責所有畫面顯示）
   ↓ 透過
OAAdmin / OAUser  ← 部門主管（負責傳達命令）
   ↓ 委託給
OADataCenter.cpp  ← 人事/資料庫部門（負責實際管理資料）
   ↓ 操作
OAAdmin / OAUser / OAMeetingRoom  ← 各種資料物件
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

把它想像成**菜單**——你看到「漢堡」這個名字，但菜單不告訴你漢堡是怎麼做的。

**來自 `OAUtils.h` 的範例：**
```cpp
#pragma once

#define MEETING_ROOM_BOOK_SUCCESS 0    // 宣告：這個數字代表預約成功
#define MEETING_ROOM_BOOKED 1          // 宣告：這個數字代表已被人預約
#define MEETING_ROOM_NOT_FOUND -1      // 宣告：這個數字代表找不到會議室

class OAUtils {
   public:
    static int inputNumber();  // 宣告：「這個函式存在，會回傳整數」
};
```

**來自 `OAUser.h` 的範例：**
```cpp
#pragma once
#include <iostream>
#include "OADataCenter.h"
using namespace std;

class OAUser {
    friend class OADataCenter;

   private:
    string _username;
    string _password;
    string _department;

   public:
    OAUser();
    OAUser(string username, string password, string department);
    static OAUser ERROR_USER;

    string username();
    string department();

    int bookMeetingRoom(int mid);         // 宣告：「預約會議室」方法
    int cancelBookMeetingRoom(int mid);   // 宣告：「取消預約」方法
    void showAllBookedMeetingRooms();     // 宣告：「顯示所有已預約的會議室」方法
};
```

### 原始碼檔（.cpp）

原始碼檔包含**定義** - 事物如何運作的實際實作。這就像廚房——真正的烹飪在這裡發生。

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

**來自 `OAUser.cpp` 的範例（新增的預約方法）：**
```cpp
#include "OAUser.h"

// 預約會議室：委託給 OADataCenter，把「自己」(*this) 傳過去
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
    //                                                  ^^^^^ *this = 「我自己這個物件」
}

// 取消預約：同樣委託給 OADataCenter
int OAUser::cancelBookMeetingRoom(int mid) {
    return OADataCenter::getInstance().cancelBookMeetingRoom(*this, mid);
}

// 顯示所有「我」預約的會議室
void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {  // 如果這間會議室是「我」預約的
            cout << "Meeting room No." << r.mid()
                 << ", meeting room capacity: " << r.capacity() << ", "
                 << this->_department << "'s " << this->_username
                 << " already booked!" << endl;
        }
    }
}
```

### 主要差異

| 標頭檔（.h） | 原始碼檔（.cpp） |
|--------------|------------------|
| 包含宣告 | 包含定義 |
| 說明「什麼存在」 | 說明「如何運作」 |
| 被其他檔案引入 | 直接編譯 |
| 使用 `#pragma once` | 使用 `#include` |
| 像菜單 | 像廚房 |

---

## 3. 預處理器指令

預處理器指令是在**編譯之前**處理的指令。它們都以 `#` 開頭。想像預處理器是一個「助手」，在編譯器開始工作之前先幫你整理程式碼。

### 3.1 `#pragma once`

```cpp
#pragma once
```

**用途**：防止標頭檔被多次引入。

**為什麼需要這個？** 想像這個情境：
- `FileA.h` 引入 `OAUser.h`
- `FileB.h` 引入 `OAUser.h`
- `main.cpp` 同時引入 `FileA.h` 和 `FileB.h`

沒有 `#pragma once`，編譯器會看到 `OAUser.h` 兩次，導致「重複定義」錯誤。就像把同一份食譜影印兩次，廚師看到兩份一模一樣的，會困惑「要做幾份？」

**替代方案（較舊的寫法）：**
```cpp
#ifndef OAUSER_H
#define OAUSER_H
// ... 標頭內容 ...
#endif
```

### 3.2 `#include`

```cpp
#include <iostream>    // 系統/標準函式庫標頭（用尖括號）
#include "OAUtils.h"   // 使用者定義的標頭（用雙引號）
```

**兩種形式：**
- `<filename>` - 先在系統/標準目錄中搜尋
- `"filename"` - 先在當前目錄中搜尋

**它做什麼**：字面上將被引入檔案的內容**複製貼上**到當前檔案中。就像 Ctrl+C、Ctrl+V。

### 3.3 `#define`

`#define` 是一個**文字替換**工具。程式碼裡看到這個名字的地方，在編譯前都會被換成對應的值。

**OASystemUI.cpp 中的登入旗標：**
```cpp
#define FLAG_ADMIN 1    // 之後 FLAG_ADMIN 就等於 1
#define FLAG_USER 0     // 之後 FLAG_USER 就等於 0
```

這樣在程式碼裡可以寫：
```cpp
showLogin(FLAG_ADMIN);  // 比寫 showLogin(1) 更清楚！
showLogin(FLAG_USER);   // 你一眼就知道這是用戶登入
```

**OAUtils.h 中的預約狀態代碼（非常重要！）：**
```cpp
// 預約會議室的結果代碼
#define MEETING_ROOM_BOOK_SUCCESS 0    // 預約成功！
#define MEETING_ROOM_BOOKED 1          // 預約失敗：這間已被人預約
#define MEETING_ROOM_NOT_FOUND -1      // 預約失敗：找不到這間會議室

// 取消預約的結果代碼
#define CANCEL_MEETING_ROOM_SUCCESS 0      // 取消成功！
#define CANCEL_MEETING_ROOM_NOT_FOUND -1   // 取消失敗：找不到這間會議室
#define CANCEL_MEETING_ROOM_FAIL 1         // 取消失敗：這間不是你預約的
```

這些代碼被 `OADataCenter` 的預約函式回傳，然後 `OASystemUI` 根據代碼顯示對應的訊息：

```cpp
// OASystemUI.cpp 中如何使用這些代碼：
int res = user.bookMeetingRoom(mid);
if (res == MEETING_ROOM_BOOK_SUCCESS) {       // res == 0
    cout << "Meeting room booked successfully!" << endl;
} else if (res == MEETING_ROOM_BOOKED) {      // res == 1
    cout << "Booked fail, this meeting room already be booked!" << endl;
} else if (res == MEETING_ROOM_NOT_FOUND) {   // res == -1
    cout << "Booked fail, this meeting room do not exist!" << endl;
}
```

**為什麼用 #define 而不是直接寫數字？**

```cpp
// 不好的寫法（「魔法數字」）：
if (res == 0) { ... }   // 0 是什麼意思？誰知道！

// 好的寫法（有意義的名字）：
if (res == MEETING_ROOM_BOOK_SUCCESS) { ... }  // 一眼就懂
```

**現代 C++ 替代方案**（較推薦）：
```cpp
const int FLAG_ADMIN = 1;
constexpr int FLAG_USER = 0;

// 或者用 enum（列舉）更好：
enum BookResult {
    BOOK_SUCCESS = 0,
    ALREADY_BOOKED = 1,
    NOT_FOUND = -1
};
```

---

## 4. 命名空間

### 什麼是命名空間？

命名空間是一個容器，用來容納一組識別符號（名稱）。它可以防止命名衝突。

想像你公司裡有兩個叫「大明」的人——一個在財務部，一個在技術部。你說「大明來一下」，大家不知道叫誰。但說「財務部的大明來一下」就清楚了。命名空間就像「部門名稱」。

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

### 本專案的使用方式

**在 .cpp 檔案中使用（正確的做法）：**
```cpp
// OAUser.cpp
#include "OAUser.h"
// 注意：OAUser.h 已經有 using namespace std;
// 所以 .cpp 裡可以直接用 cout、string 等

void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = ...;  // 直接用 vector，不需要 std::vector
    cout << "already booked!" << endl; // 直接用 cout
}
```

### 優缺點

**`using namespace std;` 的優點：**
- 打字較少
- 程式碼看起來更簡潔

**缺點：**
- 可能造成命名衝突
- 不建議在標頭檔中使用（會污染所有引入該標頭檔的地方）

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

**類別**是建立物件的藍圖。就像蓋房子前要有設計圖，有了設計圖（類別），就可以蓋出很多棟一樣規格的房子（物件）。

類別定義：
- **屬性**（資料/特性）：物件有什麼
- **方法**（函式/行為）：物件能做什麼

### 什麼是物件？

**物件**是類別的實例 - 藍圖的具體實現。

### 本程式碼中的所有類別

| 類別 | 職責 | 關鍵特性 |
|------|------|---------|
| `OAUser` | 一般使用者 | 可以預約/取消/查看會議室 |
| `OAAdmin` | 管理員 | 可以管理用戶和會議室 |
| `OAMeetingRoom` | 一間會議室 | 有編號、容量、預約者 |
| `OADataCenter` | 資料中心（單例） | 儲存所有資料，執行所有邏輯 |
| `OASystemUI` | 使用者介面 | 顯示所有畫面，接收輸入 |
| `OAUtils` | 工具箱 | 只有一個安全輸入方法 |

### 類別定義範例

**OAUser.h（使用者類別）：**
```cpp
class OAUser {
    friend class OADataCenter;  // 好朋友，可以看私人資料

   private:
    string _username;    // 帳號（只有自己能看）
    string _password;    // 密碼（只有自己能看）
    string _department;  // 部門（只有自己能看）

   public:
    OAUser();            // 建立空的使用者
    OAUser(string username, string password, string department);  // 建立有資料的使用者
    static OAUser ERROR_USER;  // 特殊的「找不到使用者」物件

    string username();   // 取得帳號
    string department(); // 取得部門

    // 使用者能做的事：
    int bookMeetingRoom(int mid);         // 預約會議室
    int cancelBookMeetingRoom(int mid);   // 取消預約
    void showAllBookedMeetingRooms();     // 查看我預約的所有會議室
};
```

**OAMeetingRoom.h（會議室類別）：**
```cpp
class OAMeetingRoom {
    friend class OADataCenter;

   private:
    int _mid;       // 會議室編號（Meeting Room ID）
    int _capacity;  // 容納人數
    OAUser* _user;  // 指向預約者的指標（* 代表指標，稍後詳細說明）
                    // 如果沒人預約，指向特殊的 ERROR_USER 物件

   public:
    OAMeetingRoom();
    OAMeetingRoom(int mid, int capacity);

    int mid();       // 回傳編號
    int capacity();  // 回傳容納人數
    OAUser* user();  // 回傳預約者（指標）
};
```

**建立物件（OADataCenter.cpp 建構函式）：**
```cpp
OADataCenter::OADataCenter() {
    // 建立管理員物件
    admins.push_back(OAAdmin("admin", "admin"));

    // 建立使用者物件
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    // 建立會議室物件（編號, 容納人數）
    rooms.push_back(OAMeetingRoom(1, 10));  // 1 號室，10 人，沒人預約
    rooms.push_back(OAMeetingRoom(2, 6));   // 2 號室，6 人，沒人預約
    rooms.push_back(OAMeetingRoom(3, 4));   // 3 號室，4 人，沒人預約

    // ★ 第 4 間會議室：用 new 在堆積建立使用者，並設為已預約狀態
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");  // 用 new 建立（堆積上）
    OAMeetingRoom r(4, 12);   // 4 號室，12 人
    r._user = user;           // 直接設定私有成員（因為 friend 關係可以這樣做）
    rooms.push_back(r);       // 加入 rooms vector
}
```

**注意** Room 4 的建立方式和前三間不一樣！它用了 `new` 關鍵字和直接設定私有成員，這些概念在後面的章節都會詳細解釋（第 29、30 節）。

### 類別與物件的類比

| 類別 | 物件範例 |
|------|----------|
| `OAUser`（使用者藍圖） | `OAUser("zhangsan", ...)` 這個具體的張三 |
| `OAAdmin`（管理員藍圖） | `OAAdmin("admin", "admin")` 這個具體的管理員 |
| `OAMeetingRoom`（會議室藍圖） | `OAMeetingRoom(1, 10)` 這間具體的 1 號 10 人會議室 |
| 房子設計圖 | 實際蓋好的房子 |
| 餅乾模具 | 一塊餅乾 |

---

## 6. 存取修飾符

存取修飾符控制誰可以存取類別成員。就像你家的門鎖——有些房間所有人都能進，有些房間只有你能進。

### 三種存取級別

```cpp
class Example {
   public:      // 任何地方都可存取（前門大開）
    int publicVar;

   private:     // 只有類別內部可存取（上鎖的臥室）
    int privateVar;

   protected:   // 類別內部和衍生類別可存取（家人才能進）
    int protectedVar;
};
```

### OAAdmin.h 的實際範例

```cpp
class OAAdmin {
    friend class OADataCenter;  // OADataCenter 有萬能鑰匙

   private:
    string _username;   // 上鎖的：只有 OAAdmin 的方法和 OADataCenter 可以存取
    string _password;   // 上鎖的：密碼當然要鎖好

   public:
    OAAdmin();          // 任何人都可以呼叫的建構函式
    OAAdmin(string username, string password);
    static OAAdmin ERROR_ADMIN;  // 任何人都可以存取
    string username();  // 公開的 getter，任何人都可以呼叫
    // ...（addUser、deleteUser 等管理方法）
};
```

### OAUser.h 的完整範例（含新方法）

```cpp
class OAUser {
    friend class OADataCenter;

   private:
    string _username;    // 私有：外部無法直接讀取
    string _password;    // 私有：密碼保護
    string _department;  // 私有：外部無法直接讀取

   public:
    // 公開方法：外部可以呼叫
    string username();
    string department();

    int bookMeetingRoom(int mid);          // 公開：任何地方都可以呼叫
    int cancelBookMeetingRoom(int mid);    // 公開：任何地方都可以呼叫
    void showAllBookedMeetingRooms();      // 公開：任何地方都可以呼叫
};
```

但在 `showAllBookedMeetingRooms()` 的**內部**，可以直接存取私有成員：
```cpp
void OAUser::showAllBookedMeetingRooms() {
    // 在類別的方法內部，可以存取私有成員：
    cout << this->_department << "'s " << this->_username << " already booked!";
    //          ^^^^^^^^^^^^^ 私有成員，但在類別方法裡面所以可以用
}
```

### 為什麼使用 Private？

**封裝** - 隱藏內部細節，防止外部亂改資料：

```cpp
class BankAccount {
   private:
    double balance;  // 無法從外部直接修改

   public:
    void deposit(double amount) {
        if (amount > 0) {  // 有驗證！
            balance += amount;
        }
    }
};
```

沒有 private，有人可能會這樣做：
```cpp
account.balance = -1000000;  // 災難！直接把餘額改成負的
```

---

## 7. 成員變數與方法

### 成員變數（資料成員）

屬於類別的變數，每個物件都有自己的一份。

```cpp
class OAUser {
   private:
    string _username;    // 成員變數（字串）
    string _password;    // 成員變數（字串）
    string _department;  // 成員變數（字串）
};
```

OAMeetingRoom 有一個**指標型別**的成員變數：

```cpp
class OAMeetingRoom {
   private:
    int    _mid;      // 成員變數（整數）
    int    _capacity; // 成員變數（整數）
    OAUser* _user;    // 成員變數（指標！）
                      // 星號 * 代表「指向 OAUser 的指標」
                      // 記錄「是哪個用戶預約了這間會議室」
                      // 如果沒人預約，指向特殊的 ERROR_USER 物件
};
```

**命名慣例**：底線前綴（`_username`）表示「這是一個私有成員」。這是一個常見的慣例，不是語言要求。

### 成員方法（成員函式）

屬於類別的函式。本專案中每個類別的方法職責：

**OAUser 的方法：**

| 方法 | 說明 |
|------|------|
| `username()` | 回傳使用者帳號（getter） |
| `department()` | 回傳部門名稱（getter） |
| `bookMeetingRoom(int mid)` | 預約指定編號的會議室，回傳結果代碼 |
| `cancelBookMeetingRoom(int mid)` | 取消預約，回傳結果代碼 |
| `showAllBookedMeetingRooms()` | 顯示所有「我」預約的會議室 |

**在標頭檔中宣告：**
```cpp
class OAUser {
   public:
    int bookMeetingRoom(int mid);       // 方法宣告
    int cancelBookMeetingRoom(int mid); // 方法宣告
    void showAllBookedMeetingRooms();   // 方法宣告
};
```

**在原始碼檔中定義：**
```cpp
// bookMeetingRoom 實作：把工作交給 OADataCenter
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
    //                                                  ^^^^^
    //                                          *this = 把「我自己」傳過去
    //                                          這樣 OADataCenter 就知道是誰在預約
}

// cancelBookMeetingRoom 實作：同樣委託給 OADataCenter
int OAUser::cancelBookMeetingRoom(int mid) {
    return OADataCenter::getInstance().cancelBookMeetingRoom(*this, mid);
}

// showAllBookedMeetingRooms 實作：找出所有「我」預約的房間
void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {  // r.user() 是指標，this 也是指標
                                 // 如果兩個指標相同，代表這間是我預約的
            cout << "Meeting room No." << r.mid()
                 << ", meeting room capacity: " << r.capacity() << ", "
                 << this->_department << "'s " << this->_username
                 << " already booked!" << endl;
        }
    }
}
```

### Getter 和 Setter 方法

```cpp
class OAAdmin {
   private:
    string _username;

   public:
    // Getter - 回傳值（只讀，不能改）
    string username() { return _username; }

    // Setter - 設定值（目前程式碼中沒有，但這是常見模式）
    void setUsername(string name) { _username = name; }
};
```

---

## 8. 建構函式

### 什麼是建構函式？

建構函式是一個特殊的方法，在物件**誕生時**自動呼叫。就像新生兒出生後自動填寫出生證明一樣，建構函式負責初始化物件的資料。

### 建構函式的特徵

- 與類別同名
- 沒有回傳型別（連 `void` 都沒有）
- 在物件建立時自動呼叫
- 可以多載（多個版本）

### 本程式碼中的建構函式類型

**1. 預設建構函式（無參數）：**
```cpp
// OAAdmin.h 宣告
OAAdmin();

// OAAdmin.cpp 定義
OAAdmin::OAAdmin() {}  // 什麼都不做，成員變數保持預設值（空字串）
```

**2. 帶參數的建構函式：**
```cpp
// OAAdmin.h 宣告
OAAdmin(string username, string password);

// OAAdmin.cpp 定義
OAAdmin::OAAdmin(string username, string password) {
    _username = username;  // 把傳進來的 username 存到私有成員 _username
    _password = password;
}
```

**3. OAMeetingRoom 的建構函式（含指標初始化）：**
```cpp
// 無參數版本：建立一個「空的」會議室
OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
    _user = &OAUser::ERROR_USER;  // 指標初始化：指向「沒人預約」的標記物件
                                  // & 是「取地址」運算子，稍後詳細說明
}

// 帶參數版本：建立有具體資料的會議室
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;  // 新建的會議室一定是空的
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

### Room 4 的特殊建構方式

`OADataCenter` 的建構函式還展示了一種**進階用法**：建立物件後直接修改私有成員。

```cpp
OADataCenter::OADataCenter() {
    // 前三間房間：用一般方式建立（_user 自動指向 ERROR_USER）
    rooms.push_back(OAMeetingRoom(1, 10));
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));

    // 第四間房間：需要一開始就設為「已被 xiaoxiao 預約」
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    //       ^^^^
    //       這是一個指標（OAUser*），指向在「堆積」上建立的 xiaoxiao 物件
    //       new 關鍵字把物件建在堆積上（第 30 節有詳細說明）

    OAMeetingRoom r(4, 12);  // 建立 4 號 12 人會議室
    r._user = user;           // 直接設定私有成員 _user！
    // 因為 OADataCenter 是 OAMeetingRoom 的 friend（友元）
    // 所以可以直接存取 r._user 這個私有成員

    rooms.push_back(r);  // 把設定好的房間加入 vector
}
```

**為什麼這樣做？** 因為 `OAMeetingRoom` 的建構函式只能把 `_user` 初始化為 `ERROR_USER`（空房間）。要建立一個「一開始就有人預約」的房間，必須在建立後手動設定 `_user`。

### 初始化列表（替代語法）

一種更有效率的初始化成員方式：

```cpp
// 目前的寫法（賦值）：
OAAdmin::OAAdmin(string username, string password) {
    _username = username;  // 先建立空字串，再賦值（兩個步驟）
    _password = password;
}

// 更好的寫法（初始化列表，直接初始化）：
OAAdmin::OAAdmin(string username, string password)
    : _username(username), _password(password) {
    // 建構函式本體（在這個例子中是空的）
}
```

初始化列表像是「出生時就填好的資料」，而賦值像是「出生後再補填」。

---

## 9. 靜態成員

### `static` 是什麼意思？

`static` 成員屬於**類別本身**，而不是任何特定的物件。

想像學校有個公告欄，上面的公告是全班同學共享的（static），不是每個同學各自複製一份。

### 靜態變數：ERROR 標記物件

**OAAdmin 的靜態標記：**
```cpp
// OAAdmin.h
class OAAdmin {
   public:
    static OAAdmin ERROR_ADMIN;  // 所有 OAAdmin 物件共享的「登入失敗」標記
};

// OAAdmin.cpp（必須在類別外部定義）
OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();  // 呼叫無參數建構函式建立空的 OAAdmin
```

**OAUser 的靜態標記（有兩種用途！）：**
```cpp
// OAUser.h
class OAUser {
   public:
    static OAUser ERROR_USER;  // 兩種用途：
                               // 1. loginUser() 找不到用戶時回傳它
                               // 2. OAMeetingRoom::_user 指向它，代表「空房間」
};

// OAUser.cpp
OAUser OAUser::ERROR_USER = OAUser();
```

`ERROR_USER` 有兩個用途：
1. `loginUser()` 找不到用戶時回傳它（登入失敗標記）
2. `OAMeetingRoom::_user` 指向它，代表「這間會議室沒有人預約」（空房間標記）
3. `bookMeetingRoom()` 中用來判斷房間是否可以預約：

```cpp
// OADataCenter.cpp 中的 bookMeetingRoom：
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    // ...找到 it 後：
    if (it->_user != &OAUser::ERROR_USER) {  // 如果不是指向 ERROR_USER
        return MEETING_ROOM_BOOKED;           // 代表有人預約了，拒絕！
    }
    it->_user = &user;                        // 讓指標指向這個使用者
    return MEETING_ROOM_BOOK_SUCCESS;
}
```

### 靜態方法

```cpp
class OAUtils {
   public:
    static int inputNumber();  // 可以在不建立物件的情況下呼叫
};

// 呼叫靜態方法：
int choice = OAUtils::inputNumber();  // 不需要物件！直接用類別名稱
```

**OADataCenter 的靜態方法 getInstance()：**
```cpp
class OADataCenter {
   private:
    static OADataCenter _instance;  // 唯一的資料中心實例

   public:
    static OADataCenter& getInstance();  // 取得那個唯一的實例
};

// 每次呼叫都回傳同一個物件：
OADataCenter& dc = OADataCenter::getInstance();
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

`friend` 類別可以存取另一個類別的私有和保護成員。就像是「我把備用鑰匙給了我的好朋友，他可以進我家的任何房間」。

**重要**：友元關係必須由被存取的類別**主動授予**，不能自己宣告自己是別人的友元。

### 程式碼中的範例

本專案有**三個**類別都宣告 `OADataCenter` 為友元：

**OAAdmin.h：**
```cpp
class OAAdmin {
    friend class OADataCenter;  // OADataCenter 可以存取 OAAdmin 的私有成員

   private:
    string _username;  // OADataCenter 可以直接讀這個
    string _password;  // OADataCenter 可以直接讀這個
};
```

**OAUser.h：**
```cpp
class OAUser {
    friend class OADataCenter;  // OADataCenter 可以存取 OAUser 的私有成員

   private:
    string _username;    // OADataCenter 可以直接讀
    string _password;    // OADataCenter 可以直接讀
    string _department;  // OADataCenter 可以直接讀
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

### 友元在本專案中的實際應用

**1. 登入驗證（讀取私有 _username 和 _password）：**
```cpp
// OADataCenter.cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            //       ^^^^^^^^^ 直接存取私有成員，因為 friend 關係
            return admin;
        }
    }
    return OAAdmin::ERROR_ADMIN;
}
```

**2. 清除所有預約（修改私有 _user 指標）：**
```cpp
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 直接修改私有指標成員
        //   ^^^^^ 因為 friend 關係才能這樣做
    }
}
```

**3. Room 4 的初始化（直接設定私有 _user）：**
```cpp
OAMeetingRoom r(4, 12);
r._user = user;  // 直接設定私有成員！只有 friend 才能這樣做
```

**4. 預約會議室（存取私有 _mid 和 _user）：**
```cpp
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {  // 直接讀取私有 _mid
            break;
        }
    }
    if (it->_user != &OAUser::ERROR_USER) {  // 直接讀取私有 _user
        return MEETING_ROOM_BOOKED;
    }
    it->_user = &user;  // 直接修改私有 _user！
    return MEETING_ROOM_BOOK_SUCCESS;
}
```

### 友元的特性

- 友元關係**不是對稱的**：如果 A 是 B 的友元，B 不會自動成為 A 的友元
- 友元關係**不是傳遞的**：如果 A 是 B 的友元，B 是 C 的友元，A 不會自動成為 C 的友元
- 友元關係**不能被繼承**
- **盡量少用**：太多友元會破壞封裝的好處

---

## 11. 參考

### 什麼是參考？

參考是現有變數的**別名**（另一個名稱）。就像你的外號——你本人只有一個，但別人可以用不同的名字叫你。

```cpp
int x = 10;
int& ref = x;  // ref 是 x 的另一個名字，它們指向同一個資料

ref = 20;      // 透過外號修改，原本的 x 也變了！
cout << x;     // 輸出：20
```

### 本程式碼中的參考

**1. 函式回傳參考（回傳「真實物件」而非副本）：**
```cpp
// 回傳 OAAdmin 的參考（不是副本）
OAAdmin& OADataCenter::loginAdmin(string username, string password);

// 為什麼回傳參考？
// 如果回傳值（副本），後面的位址比較就會失敗：
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &OAAdmin::ERROR_ADMIN) {  // 比較地址來判斷登入是否失敗
    cout << "Login failed";
}
// 如果是副本，每次都是新的物件，地址都不同，比較永遠失敗！
```

**2. 範圍式 for 迴圈中的參考（避免複製）：**
```cpp
// 有 & → 直接操作原本的元素，可以修改，而且不複製（快）
for (OAAdmin& admin : admins) {
    if (admin._username == username) { return admin; }
}

// 沒有 & → 每次都複製一份新的，修改不影響原本（慢）
for (OAAdmin admin : admins) { ... }  // 不好的寫法
```

**3. 函式參數中的參考（讓函式可以修改原始資料）：**
```cpp
// OADataCenter.cpp：bookMeetingRoom 接受 OAUser 的參考
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    //                             ^^^^^^^^^^^
    //                             user 是呼叫者傳入物件的別名
    //                             這讓我們可以取得它的地址存進 _user 指標

    it->_user = &user;  // & 取得 user（也就是原始使用者物件）的地址
    //           ^^^^^  存進指標，之後就能找回這個使用者
}
```

**參考 vs 指標 vs 值**

```cpp
void byValue(int x) { x = 10; }      // 修改副本，原始值不變
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
   void process(const vector<int>& data);  // 不複製
   ```

2. **修改原始值**：允許函式修改呼叫者的變數
   ```cpp
   void increment(int& value) { value++; }
   ```

3. **回傳存取權 + 支援地址比較**：
   ```cpp
   OAAdmin& loginAdmin(...);  // 回傳實際管理員的參考，地址可以比較
   OAUser&  loginUser(...);   // 回傳實際用戶的參考，地址可以比較
   ```

---

## 12. 單例設計模式

### 什麼是單例？

**單例**確保一個類別只有**一個**實例，並提供對它的全域存取點。

想像整個公司只有一個「資料庫伺服器」。不管哪個部門需要查資料，都連到同一台伺服器。你不可能讓每個人都各自建立一台伺服器！

### OADataCenter 中的實作

**標頭檔（OADataCenter.h）：**
```cpp
class OADataCenter {
   private:
    static OADataCenter _instance;  // 唯一的實例（藏在類別裡）
    OADataCenter();                  // 私有建構函式！外部不能用 new 建立新的

   public:
    static OADataCenter& getInstance();  // 唯一的取得方式
    // ... 所有資料操作方法
};
```

**原始碼檔（OADataCenter.cpp）：**
```cpp
// 程式啟動時自動建立唯一的實例（只執行一次）
OADataCenter OADataCenter::_instance = OADataCenter();

// 私有建構函式：預載入所有初始資料
OADataCenter::OADataCenter() {
    // 預設管理員
    admins.push_back(OAAdmin("admin", "admin"));

    // 預設用戶
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    // 預設會議室（前三間都是空的）
    rooms.push_back(OAMeetingRoom(1, 10));
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));

    // 第四間：已被 xiaoxiao 預約
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    OAMeetingRoom r(4, 12);
    r._user = user;      // friend 存取：直接設定私有 _user
    rooms.push_back(r);
}

// 回傳唯一實例的參考
OADataCenter& OADataCenter::getInstance() {
    return _instance;
}
```

### 單例如何運作

1. **私有建構函式**：沒有人可以從外部 `new OADataCenter()` 建立新實例
2. **靜態實例**：一個實例儲存在類別本身中，程式啟動時自動建立
3. **公開存取器**：`getInstance()` 提供對該唯一實例的存取

```cpp
// 正確的使用方式：
OADataCenter& dc = OADataCenter::getInstance();

// 錯誤的使用方式（無法編譯，因為建構函式是 private）：
OADataCenter dc2;              // 錯誤！
OADataCenter* dc3 = new OADataCenter();  // 錯誤！
```

### 使用單例

```cpp
// 每次都回傳同一個物件（同一個地址）
OADataCenter& dataCenter = OADataCenter::getInstance();
OAAdmin& admin = dataCenter.loginAdmin("admin", "admin");
OAUser& user = dataCenter.loginUser("zhangsan", "123123");

// 不同地方呼叫，都是同一個資料中心：
OAAdmin::addUser() 裡呼叫 OADataCenter::getInstance()
OAUser::bookMeetingRoom() 裡呼叫 OADataCenter::getInstance()
// → 兩個都操作的是同一份資料！
```

### 為什麼使用單例？

- **資料庫連線**：所有人使用一個連線池
- **設定管理器**：唯一的真實來源
- **日誌記錄**：整個應用程式使用一個日誌記錄器
- **本專案**：一個資料中心管理所有使用者/管理員/會議室

---

## 13. STL 容器（vector）

### 什麼是 STL？

STL（標準模板函式庫）提供現成的資料結構和演算法。就像工具箱，裡面有各種常用工具，不需要自己從頭打造。

### 什麼是 Vector？

`vector` 是一個可以**動態增長或縮小**的動態陣列。就像可伸縮的購物袋——可以一直往裡放東西，袋子會自動變大。

```cpp
#include <vector>

vector<int> numbers;           // 空的整數向量
vector<string> names;          // 空的字串向量
vector<OAAdmin> admins;        // 空的 OAAdmin 物件向量
```

### 本程式碼中的 Vector

**OADataCenter 儲存三種資料：**
```cpp
class OADataCenter {
   private:
    vector<OAAdmin>       admins;  // 管理員的動態陣列
    vector<OAUser>        users;   // 使用者的動態陣列
    vector<OAMeetingRoom> rooms;   // 會議室的動態陣列
};
```

**建構函式填入初始資料：**
```cpp
OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));       // 1 個管理員
    users.push_back(OAUser("zhangsan", "123123", ...)); // 2 個用戶
    users.push_back(OAUser("lisi", "123456", ...));
    rooms.push_back(OAMeetingRoom(1, 10));              // 4 間會議室
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));
    // 第 4 間用特殊方式（有預約狀態）
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    OAMeetingRoom r(4, 12);
    r._user = user;
    rooms.push_back(r);
}
```

**回傳 vector 的副本（安全機制）：**
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

**但這有個重要問題！** `OAMeetingRoom` 裡的 `_user` 是指標。當你複製 rooms vector，指標的**值**（地址）也被複製，仍然指向原來的 `OAUser` 物件。所以：
```cpp
// showAllBookedMeetingRooms 中：
vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();  // 副本
for (OAMeetingRoom& r : rooms) {
    if (r.user() == this) {  // 比較指標！
        // r.user() 雖然是副本裡的指標，但它仍然指向原始的 OAUser 物件
        // this 也指向同一個原始 OAUser 物件
        // 所以比較是正確的！
    }
}
```

### 常見的 Vector 操作

```cpp
vector<int> v;

v.push_back(10);    // 加到尾端：[10]
v.push_back(20);    // 加到尾端：[10, 20]
v.push_back(30);    // 加到尾端：[10, 20, 30]

v.size();           // 回傳 3（目前有幾個元素）
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

cout  // 標準輸出（主控台輸出）→ 用 << 運算子
cin   // 標準輸入（鍵盤輸入）  → 用 >> 運算子
cerr  // 標準錯誤輸出
endl  // 結束行（換行）並清空緩衝區
```

### 用 `cout` 輸出

```cpp
cout << "Hello World" << endl;
cout << "Number: " << 42 << endl;
cout << "Name: " << name << ", Age: " << age << endl;
```

`<<` 運算子稱為「插入運算子」 - 它將資料插入串流中。可以連續使用多個 `<<`。

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

### 本專案中的輸入場景

**管理員新增用戶（OASystemUI.cpp）：**
```cpp
void showAdminAddUserPage(OAAdmin& admin) {
    system("clear");
    string username, password, department;
    cout << "Please input username for add user: ";
    cin >> username;    // 讀取帳號
    cout << "Please input password for this user: ";
    cin >> password;    // 讀取密碼
    cout << "Please input department for add user: ";
    cin >> department;  // 讀取部門

    if (admin.addUser(username, password, department)) {
        cout << "Add user successfully" << endl;
    } else {
        cout << "Add user failed, this user already existed" << endl;
    }
    cout << "Press Enter to continue...";
    cin.get();  // 等待使用者按 Enter
}
```

**使用者預約會議室（OASystemUI.cpp）：**
```cpp
void showUserBookMeetingRoomPage(OAUser& user) {
    system("clear");
    cout << "Please input the No. of meeting room you want to reserve: " << endl;
    int mid = OAUtils::inputNumber();  // 安全輸入（帶驗證）

    int res = user.bookMeetingRoom(mid);  // 呼叫使用者的預約方法
    if (res == MEETING_ROOM_BOOK_SUCCESS) {
        cout << "Meeting room booked successfully!" << endl;
    } else if (res == MEETING_ROOM_BOOKED) {
        cout << "Booked fail, this meeting room already be booked!" << endl;
    } else if (res == MEETING_ROOM_NOT_FOUND) {
        cout << "Booked fail, this meeting room do not exist!" << endl;
    }
}
```

**使用者取消預約（OASystemUI.cpp）：**
```cpp
void showUserCancelBookMeetingRoom(OAUser& user) {
    system("clear");
    cout << "Please input the No. of meeting room you want to cancel: " << endl;
    int mid = OAUtils::inputNumber();

    int res = user.cancelBookMeetingRoom(mid);
    if (res == CANCEL_MEETING_ROOM_SUCCESS) {
        cout << "Cancel booked meeting room successfully!" << endl;
    } else if (res == CANCEL_MEETING_ROOM_FAIL) {
        cout << "Cancel fail, you do not book this meeting room!" << endl;
    } else if (res == CANCEL_MEETING_ROOM_NOT_FOUND) {
        cout << "Cancel fail, this meeting room do not exist!" << endl;
    }
}
```

### 輸入驗證（來自 OAUtils.cpp）

```cpp
int OAUtils::inputNumber() {
    int choice = -1;
    cin >> choice;

    while (cin.fail()) {         // 如果輸入失敗（例如使用者輸入字母）
        cin.clear();             // 清除錯誤旗標（讓 cin 可以重新使用）
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // 丟棄緩衝區中的壞資料
        cin >> choice;           // 再試一次
    }

    return choice;
}
```

**這做了什麼：**
1. 嘗試讀取一個數字
2. 如果使用者輸入「abc」，`cin.fail()` 為 true
3. `cin.clear()` 清除錯誤狀態
4. `cin.ignore(...)` 丟棄緩衝區中「abc\n」
5. 再次讀取，直到輸入合法數字

**`numeric_limits<streamsize>::max()`** 的意思是「忽略緩衝區中最多這麼多字元，直到碰到換行符 `\n`」——實際上就是「清空整行」。

### ANSI 跳脫碼

```cpp
cout << "\033[2J\033[H";
```

這是一個終端機控制序列：
- `\033` = ESC 字元（ASCII 27），告訴終端機「接下來是控制命令」
- `[2J` - 清除整個螢幕
- `[H` - 將游標移到起始位置（左上角）

---

## 15. 流程控制語句

### 15.1 While 迴圈

```cpp
while (condition) {
    // 當條件為 true 時重複執行
}
```

**本程式碼的無限迴圈模式：**
```cpp
while (1) {     // 1 永遠為 true，所以永遠執行
    // 顯示選單
    // 取得使用者輸入
    // 處理選擇
    // 直到使用者選 goto end 才會跳出
}
```

本專案中共有**三個** `while(1)` 無限迴圈：
1. `displayMainPage()` - 主選單
2. `showAdminMainPage()` - 管理員選單
3. `showUserMainPage()` - 使用者選單

### 15.2 Switch 語句

```cpp
switch (variable) {
    case value1:
        // 程式碼
        break;     // 重要！沒有 break 會「貫穿」到下一個 case
    case value2:
        // 程式碼
        break;
    default:
        // 沒有 case 匹配時
}
```

**主選單的 switch（OASystemUI.cpp）：**
```cpp
int choice = OAUtils::inputNumber();

switch (choice) {
    case 1:
        showLogin(FLAG_ADMIN);   // 管理員登入
        break;
    case 2:
        showLogin(FLAG_USER);    // 一般用戶登入
        break;
    case 3:
        goto end;                // 退出程式
}
```

**管理員選單的 switch（OASystemUI.cpp - 完整版）：**

```cpp
switch (c) {
    case 1:
        showAdminAddUserPage(admin);        // 新增用戶
        break;
    case 2:
        showAdminDeleteUserPage(admin);     // 刪除用戶
        break;
    case 3:
        showAdminListUsers(admin);          // 列出所有用戶
        break;
    case 4:
        showAdminAddMeetingRoomPage(admin); // 新增會議室
        break;
    case 5:
        showAdminDeleteMeetingRoomPage(admin); // 刪除會議室
        break;
    case 6:
        system("clear");
        admin.showAllMeetingRooms();        // 列出所有會議室
        break;
    case 7:
        admin.clearMeetingRoomStatus();     // 重置所有會議室預約狀態
        break;
    case 8:
        goto end;                           // 登出，返回主選單
}
```

**每個 case 都有自己的 `break`，沒有貫穿問題。只有 case 8 用 `goto end` 跳出迴圈。**

**使用者選單的 switch（OASystemUI.cpp）：**
```cpp
switch (c) {
    case 1:
        showUserBookMeetingRoomPage(user);     // 預約會議室
        break;
    case 2:
        showUserCancelBookMeetingRoom(user);   // 取消預約
        break;
    case 3:
        system("clear");
        user.showAllBookedMeetingRooms();      // 列出我預約的所有會議室
        break;
    case 4:
        goto end;                              // 登出
}
```

### 15.3 If-Else 語句

```cpp
if (condition) {
    // 條件為 true 時的程式碼
} else if (other_condition) {
    // 其他條件
} else {
    // 全不符合時的程式碼
}
```

**登入類型判斷：**
```cpp
if (flag == FLAG_ADMIN) {
    cout << "Admin Login" << endl;
    // 管理員登入流程
} else {
    cout << "User Login" << endl;
    // 用戶登入流程
}
```

**預約結果判斷（使用 #define 的代碼）：**
```cpp
int res = user.bookMeetingRoom(mid);
if (res == MEETING_ROOM_BOOK_SUCCESS) {       // res == 0
    cout << "Meeting room booked successfully!" << endl;
} else if (res == MEETING_ROOM_BOOKED) {      // res == 1
    cout << "Booked fail, this meeting room already be booked!" << endl;
} else if (res == MEETING_ROOM_NOT_FOUND) {   // res == -1
    cout << "Booked fail, this meeting room do not exist!" << endl;
}
```

### 15.4 Goto 語句

```cpp
goto label_name;  // 跳到標籤
// ...（這段程式碼被跳過）
label_name:       // 標籤在這裡
    // 繼續執行
```

**本專案中用來退出無限迴圈：**
```cpp
while (1) {
    // ...選單邏輯...
    case 3:
        goto end;  // 跳出 while 迴圈
}
end:
    cout << "Thanks and see you next time!" << endl;
```

**注意**：在現代 C++ 中通常不建議使用 `goto`。更好的替代方案：
```cpp
bool running = true;
while (running) {
    // ...
    case 3:
        running = false;
        break;
}
```

### 15.5 For 迴圈（一般版本）

除了範圍式 for（第 18 節），本專案也用了傳統 for 迴圈（在迭代器章節詳細說明）：

```cpp
// 從頭到尾掃描 vector，找特定元素
auto it = users.begin();
for (; it != users.end(); it++) {
    if (it->username() == username) {
        break;
    }
}
```

---

## 16. main() 函式

### 程式進入點

每個 C++ 程式都從 `main()` 開始執行。就像餐廳的大門——所有客人都從這裡進來。

```cpp
// main.cpp
#include <iostream>
#include "OASystemUI.h"
using namespace std;

int main() {
    OASystemUI::displayMainPage();  // 整個程式就這一行：啟動 UI
    return EXIT_SUCCESS;            // 告訴作業系統「我跑完了，而且成功」
};
```

### 為什麼只有這一行？

因為整個應用程式的邏輯都在 `OASystemUI::displayMainPage()` 裡。`main.cpp` 只是啟動按鈕。這是好的設計——讓 `main` 盡可能簡單。

### 回傳值

- `return 0;` 或 `return EXIT_SUCCESS;` - 程式成功完成
- `return 1;` 或任何非零值 - 程式遇到錯誤

作業系統會接收這個值：
```bash
./Main
echo $?  # 顯示回傳值（成功為 0）
```

### 其他 main() 簽名

```cpp
// 基本形式（本專案用這個）
int main() { }

// 帶命令列參數（./program arg1 arg2）
int main(int argc, char* argv[]) {
    // argc = 參數數量（包含程式本身名稱）
    // argv = 參數值陣列
    // 例如 ./Main admin → argc=2, argv[0]="./Main", argv[1]="admin"
}
```

---

## 17. 範圍解析運算子（::）

### 什麼是 `::`？

範圍解析運算子 `::` 指定一個名稱屬於哪個**範圍**（命名空間或類別）。就像說「財務部的大明」裡的「的」——它明確指出這個名字屬於哪個範疇。

### 本程式碼中的用法

**1. 在類別外部定義類別方法（告訴編譯器「這個函式屬於哪個類別」）：**
```cpp
// 在標頭檔中宣告
class OAUser {
    int bookMeetingRoom(int mid);
    void showAllBookedMeetingRooms();
};

// 在原始碼檔中定義 - 必須用 :: 指定這屬於 OAUser 類別
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
}

void OAUser::showAllBookedMeetingRooms() {
    // ...
}
```

**2. 存取靜態成員：**
```cpp
OAUtils::inputNumber();           // 靜態方法（工具）
OADataCenter::getInstance();      // 靜態方法（單例取得器）
OAAdmin::ERROR_ADMIN;             // 靜態變數（登入失敗標記）
OAUser::ERROR_USER;               // 靜態變數（找不到用戶/空房間標記）
```

**3. 初始化靜態成員（必須在類別外部做）：**
```cpp
OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();  // 初始化靜態成員
OAUser OAUser::ERROR_USER = OAUser();      // 初始化靜態成員
OADataCenter OADataCenter::_instance = OADataCenter();  // 初始化單例
```

**4. 命名空間存取：**
```cpp
std::cout << "Hello";             // std 命名空間中的 cout
std::vector<int> numbers;         // std 命名空間中的 vector
std::string name;                 // std 命名空間中的 string
```

### 不需要範圍解析的情況

如果你在類別**內部**，不需要 `::`：
```cpp
class OAUser {
    void showAllBookedMeetingRooms() {
        // 在類別方法內部，可以直接存取成員：
        cout << _username;           // 不需要 OAUser::_username
        cout << _department;         // 不需要 OAUser::_department
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

就像說「對 container 裡的每一個 element，做這件事」。

### 程式碼中的範例

**OADataCenter.cpp - 登入驗證（使用 & 參考，可以直接存取私有成員）：**
```cpp
for (OAAdmin& admin : admins) {
    if (admin._username == username && admin._password == password) {
        return admin;
    }
}
```

**OAUser.cpp - 查看我預約的房間（使用 this 指標比較！）：**
```cpp
void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();

    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {  // r.user() 是 OAUser* 指標
                                 // this 也是 OAUser* 指標（指向「我自己」）
                                 // 如果兩個指標相同，代表這間是我預約的
            cout << "Meeting room No." << r.mid()
                 << ", meeting room capacity: " << r.capacity() << ", "
                 << this->_department << "'s " << this->_username
                 << " already booked!" << endl;
        }
    }
}
```

**OAAdmin.cpp - 顯示所有會議室（使用 -> 存取指標）：**
```cpp
void OAAdmin::showAllMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& m : rooms) {
        cout << "MeetingRoom No.: " << m.mid()
             << ", capacity: " << m.capacity();
        if (m.user() == &(OAUser::ERROR_USER)) {
            cout << ", reservable! " << endl;
        } else {
            cout << ", this meeting room is reserved by "
                 << m.user()->department() << "'s " << m.user()->username() << "!"
                 << endl;
        }
    }
}
```

### 等效的傳統 for 迴圈

```cpp
// 範圍式 for（簡潔）：
for (OAAdmin& admin : admins) {
    if (admin._username == username) { return admin; }
}

// 等效的傳統 for：
for (int i = 0; i < admins.size(); i++) {
    OAAdmin& admin = admins[i];
    if (admin._username == username) { return admin; }
}
```

### 變化形式

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

// 傳值（複製每個元素，修改不影響原本）
for (int n : numbers) { }

// 傳參考（不複製，可修改原本元素）
for (int& n : numbers) { n *= 2; }

// 傳 const 參考（不複製，但只能讀不能改）
for (const int& n : numbers) { }

// 使用 auto（編譯器推導型別）
for (auto& n : numbers) { }
```

**本專案中：** 所有範圍式 for 都用 `&`（參考版本），原因：
1. 避免複製大型物件
2. 可以直接修改元素（如 `clearMeetingRoomStatus` 修改 `_user`）

---

## 19. 錯誤處理模式

### 模式一：標記值物件（用於登入驗證）

本程式碼用**靜態標記物件**表示「操作失敗」。

**管理員登入失敗：**
```cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;              // 成功：回傳實際管理員的參考
        }
    }
    return OAAdmin::ERROR_ADMIN;       // 失敗：回傳特殊標記物件的參考
}

// 呼叫端：透過比較地址來判斷
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &(OAAdmin::ERROR_ADMIN)) {  // 比較地址（不是比較值！）
    cout << "Login failed";
}
```

**用戶登入失敗：**
```cpp
OAUser& OADataCenter::loginUser(string username, string password) {
    for (OAUser& user : users) {
        if (user._username == username && user._password == password) {
            return user;              // 成功：回傳實際用戶
        }
    }
    return OAUser::ERROR_USER;        // 失敗：回傳標記物件
}

OAUser& user = dataCenter.loginUser(username, password);
if (&user == &(OAUser::ERROR_USER)) {
    cout << "Login failed, return to previous level" << endl;
} else {
    showUserMainPage(user);  // 成功才進入選單
}
```

### 模式二：整數回傳代碼（用於預約操作）

`#define` 定義了一組整數代碼，函式回傳這些代碼表示操作結果。

**OADataCenter.cpp - bookMeetingRoom 實作：**
```cpp
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    // 第一步：找到這間會議室
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }

    // 第二步：檢查是否存在
    if (it == rooms.end()) {
        return MEETING_ROOM_NOT_FOUND;  // -1：找不到這間房
    }

    // 第三步：檢查是否已被預約
    if (it->_user != &OAUser::ERROR_USER) {
        return MEETING_ROOM_BOOKED;     // 1：已有人預約
    }

    // 第四步：執行預約（把指標指向這個使用者）
    it->_user = &user;  // & 取得 user 的地址，存進指標
    return MEETING_ROOM_BOOK_SUCCESS;   // 0：預約成功
}
```

**OADataCenter.cpp - cancelBookMeetingRoom 實作：**
```cpp
int OADataCenter::cancelBookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }

    if (it == rooms.end()) {
        return CANCEL_MEETING_ROOM_NOT_FOUND;  // -1：找不到這間房

    }

    if (it->_user != &user) {          // 如果預約者不是你
        return CANCEL_MEETING_ROOM_FAIL;   // 1：不是你訂的，不能取消
    }

    it->_user = &OAUser::ERROR_USER;   // 把指標重設回「空房間」
    return CANCEL_MEETING_ROOM_SUCCESS; // 0：取消成功
}
```

**呼叫端（OASystemUI.cpp）根據代碼顯示不同訊息：**
```cpp
// 預約：
int res = user.bookMeetingRoom(mid);
if (res == MEETING_ROOM_BOOK_SUCCESS)  { cout << "Booked successfully!"; }
else if (res == MEETING_ROOM_BOOKED)   { cout << "Already booked!"; }
else if (res == MEETING_ROOM_NOT_FOUND){ cout << "Room not found!"; }

// 取消：
int res = user.cancelBookMeetingRoom(mid);
if (res == CANCEL_MEETING_ROOM_SUCCESS)  { cout << "Cancelled!"; }
else if (res == CANCEL_MEETING_ROOM_FAIL){ cout << "Not your booking!"; }
else if (res == CANCEL_MEETING_ROOM_NOT_FOUND) { cout << "Room not found!"; }
```

### 模式三：指標指向標記物件（用於會議室狀態）

```cpp
// 建立時：沒人預約
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _user = &OAUser::ERROR_USER;  // 指向 ERROR_USER = 空房間
}

// 預約後：指向真實使用者
it->_user = &user;  // 指向實際用戶物件

// 取消後：回到空房間
it->_user = &OAUser::ERROR_USER;

// 清除所有預約：
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 全部重設
    }
}
```

### 替代方法（現代 C++）

**例外處理：**
```cpp
OAAdmin& loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username)
            return admin;
    }
    throw runtime_error("Admin not found");
}

try {
    OAAdmin& admin = loginAdmin("name", "pass");
} catch (const runtime_error& e) {
    cout << "Error: " << e.what();
}
```

**std::optional（C++17）：**
```cpp
#include <optional>

std::optional<OAAdmin> loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username)
            return admin;
    }
    return std::nullopt;
}

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
│  displayMainPage()   │         │ inputNumber()│
│  showLogin()         │         └─────────────┘
│  showAdminMainPage() │
│  showAdminAddUser    │
│  showAdminDelUser    │
│  showAdminAddRoom    │
│  showAdminDelRoom    │
│  showUserMainPage()  │  ← 使用者選單（完整實作）
│  showUserBookRoom    │
│  showUserCancelRoom  │
└──────────┬───────────┘
           │ 透過 OAAdmin / OAUser 操作
           ▼
┌──────────────────────┐  ┌─────────────────────────────┐
│       OAAdmin        │  │           OAUser             │
│  Facade（門面）      │  │   Facade（門面）             │
│  username()          │  │   username(), department()   │
│  addUser()           │  │   bookMeetingRoom() ──────▶  │
│  deleteUser()        │  │   cancelBookMeetingRoom() ──▶│
│  showAllUsers()      │  │   showAllBookedRooms()        │
│  addMeetingRoom()    │  │   + ERROR_USER               │
│  deleteMeetingRoom() │  └─────────────────────────────┘
│  clearRoomStatus()   │
│  showAllRooms()      │
│  + ERROR_ADMIN       │
└──────────┬───────────┘
           │ 全部委託給
           ▼
┌──────────────────────────────────────────────────────┐
│                    OADataCenter (Singleton)           │
│  - admins: vector<OAAdmin>                           │
│  - users: vector<OAUser>                             │
│  - rooms: vector<OAMeetingRoom>                      │
│  + getInstance()                                     │
│  - loginAdmin() / loginUser()                        │
│  - addUser() / deleteUser()                          │
│  - addMeetingRoom() / deleteMeetingRoom()            │
│  - clearMeetingRoomStatus()                          │
│  - bookMeetingRoom() / cancelBookMeetingRoom()  ← 新 │
│  - getUsers() / getRooms()                           │
└──────┬───────────────────────────────────────────────┘
       │ 儲存（vector）
       ▼
┌────────────┐   ┌──────────────────────────┐   ┌──────────────────┐
│  OAAdmin   │   │         OAUser           │   │  OAMeetingRoom   │
│ _username  │   │ _username                │   │ _mid             │
│ _password  │   │ _password                │   │ _capacity        │
│ ERROR_ADMIN│   │ _department              │   │ _user: OAUser*──▶│ ──▶ OAUser 物件
└────────────┘   │ ERROR_USER（兩種用途）   │   │   或 ERROR_USER  │
                 └──────────────────────────┘   └──────────────────┘
```

### 執行流程（完整版）

```
1. 程式開始 → main()
2. main() 呼叫 OASystemUI::displayMainPage()
3. displayMainPage() 進入無限迴圈，顯示主選單：
   - 1. Admin Login
   - 2. User Login
   - 3. Quit
4. 使用者輸入選擇 → OAUtils::inputNumber()（帶輸入驗證）

─── 選 1（管理員登入）───
5a. showLogin(FLAG_ADMIN)：
    - 讀取帳號/密碼
    - OADataCenter::getInstance().loginAdmin() 驗證
    - 失敗（&admin == ERROR_ADMIN）：印出錯誤，返回主選單
    - 成功：進入 showAdminMainPage(admin)

6a. showAdminMainPage() 進入管理員選單（無限迴圈，8 個選項）：
    - 1. Add user    → showAdminAddUserPage()
                        → admin.addUser() → OADataCenter::addUser()
    - 2. Delete user → showAdminDeleteUserPage()
                        → admin.deleteUser() → OADataCenter::deleteUser()
    - 3. List users  → admin.showAllUsers() → OADataCenter::getUsers() 再印出
    - 4. Add room    → showAdminAddMeetingRoomPage()
                        → admin.addMeetingRoom() → OADataCenter::addMeetingRoom()
    - 5. Delete room → showAdminDeleteMeetingRoomPage()
                        → admin.deleteMeetingRoom() → OADataCenter::deleteMeetingRoom()
    - 6. List rooms  → admin.showAllMeetingRooms() → OADataCenter::getRooms() 再印出
    - 7. Reset rooms → admin.clearMeetingRoomStatus() → OADataCenter::clearMeetingRoomStatus()
    - 8. Exit        → goto end → 登出，返回主選單

─── 選 2（用戶登入）───
5b. showLogin(FLAG_USER)：
    - 讀取帳號/密碼
    - OADataCenter::getInstance().loginUser() 驗證
    - 失敗（&user == ERROR_USER）：印出錯誤
    - 成功：進入 showUserMainPage(user)

6b. showUserMainPage() 進入用戶選單（無限迴圈，4 個選項）：
    - 1. Reserve room  → showUserBookMeetingRoomPage()
                          → user.bookMeetingRoom(mid)
                          → OADataCenter::bookMeetingRoom(user, mid)
                          → 用迭代器找房間，設定 _user = &user
    - 2. Cancel room   → showUserCancelBookMeetingRoom()
                          → user.cancelBookMeetingRoom(mid)
                          → OADataCenter::cancelBookMeetingRoom(user, mid)
                          → 驗證 _user == &user，設定 _user = &ERROR_USER
    - 3. List my rooms → user.showAllBookedMeetingRooms()
                          → OADataCenter::getRooms() 拿副本
                          → 遍歷，找 r.user() == this 的房間
    - 4. Exit          → goto end → 登出，返回主選單

─── 選 3（退出）───
5c. goto end → 印出告別訊息，程式結束
```

### 逐檔案說明

#### main.cpp
```cpp
int main() {
    OASystemUI::displayMainPage();  // 啟動應用程式
    return EXIT_SUCCESS;            // 向作業系統回傳成功
};
```
**用途**：程式進入點，只是啟動 UI。

#### OASystemUI.h / OASystemUI.cpp
**用途**：處理所有使用者介面（View 層）。

| 函式 | 用途 |
|------|------|
| `displayMainPage()` | 主選單無限迴圈，轉發到 showLogin |
| `showLogin(int flag)` | 讀帳號密碼，呼叫 OADataCenter 驗證 |
| `showAdminMainPage(OAAdmin&)` | 管理員選單，8 個操作 |
| `showAdminAddUserPage(OAAdmin&)` | 讀資料，呼叫 admin.addUser() |
| `showAdminDeleteUserPage(OAAdmin&)` | 讀用戶名，呼叫 admin.deleteUser() |
| `showAdminListUsers(OAAdmin&)` | 呼叫 admin.showAllUsers() |
| `showAdminAddMeetingRoomPage(OAAdmin&)` | 讀 ID 和容量，呼叫 admin.addMeetingRoom() |
| `showAdminDeleteMeetingRoomPage(OAAdmin&)` | 讀 ID，呼叫 admin.deleteMeetingRoom() |
| `showUserMainPage(OAUser&)` | 使用者選單，4 個操作 |
| `showUserBookMeetingRoomPage(OAUser&)` | 讀 ID，呼叫 user.bookMeetingRoom() |
| `showUserCancelBookMeetingRoom(OAUser&)` | 讀 ID，呼叫 user.cancelBookMeetingRoom() |

#### OADataCenter.h / OADataCenter.cpp
**用途**：中央資料管理（單例模式）；所有真實的資料操作都在這裡。

| 方法 | 用途 | 回傳值 |
|------|------|--------|
| `getInstance()` | 取得單例 | `OADataCenter&` |
| `loginAdmin(user, pw)` | 驗證管理員 | `OAAdmin&` |
| `loginUser(user, pw)` | 驗證用戶 | `OAUser&` |
| `addUser(user, pw, dept)` | 新增用戶 | `bool` |
| `deleteUser(user)` | 刪除用戶 | `bool` |
| `addMeetingRoom(id, cap)` | 新增會議室 | `bool` |
| `deleteMeetingRoom(id)` | 刪除會議室 | `bool` |
| `clearMeetingRoomStatus()` | 清除所有預約 | `void` |
| `bookMeetingRoom(user, mid)` | 執行預約 | `int`（狀態代碼） |
| `cancelBookMeetingRoom(user, mid)` | 取消預約 | `int`（狀態代碼） |
| `getUsers()` | 取得用戶副本 | `vector<OAUser>` |
| `getRooms()` | 取得會議室副本 | `vector<OAMeetingRoom>` |

#### OAAdmin.h / OAAdmin.cpp
**用途**：管理員類別，同時也是 Facade（門面），包裝 OADataCenter 的操作。

包含：
- `_username`、`_password`（私有，friend OADataCenter 可直接讀）
- `static OAAdmin ERROR_ADMIN` — 登入失敗標記物件
- 所有操作都委託給 `OADataCenter`

#### OAUser.h / OAUser.cpp
**用途**：使用者類別，同時也是 Facade。現在有完整的預約功能！

包含：
- `_username`、`_password`、`_department`（私有）
- `static OAUser ERROR_USER` — 兩種用途：登入失敗標記、空房間標記
- `bookMeetingRoom()` — 呼叫 `OADataCenter::bookMeetingRoom(*this, mid)`
- `cancelBookMeetingRoom()` — 呼叫 `OADataCenter::cancelBookMeetingRoom(*this, mid)`
- `showAllBookedMeetingRooms()` — 遍歷所有房間，找 `r.user() == this` 的

#### OAMeetingRoom.h / OAMeetingRoom.cpp
**用途**：代表一間會議室。

包含：
- `_mid`（整數）— 會議室編號
- `_capacity`（整數）— 容納人數
- `_user`（`OAUser*` 指標）— 指向預約者；`= &OAUser::ERROR_USER` 代表空房
- `mid()`、`capacity()`、`user()` — getter 方法

#### OAUtils.h / OAUtils.cpp
**用途**：工具函式 + 狀態代碼定義。

包含：
- 6 個 `#define` 狀態代碼（預約/取消的結果）
- `inputNumber()` — 帶驗證的安全整數輸入

---

## 21. 指標（Pointer）

### 什麼是指標？

想像你家的**地址**。地址本身不是你的家，它只是告訴你家在哪裡。**指標**就是記憶體地址——它不是物件本身，而是告訴你物件住在記憶體的哪個位置。

```
記憶體（想像成一排房子）：

地址 1000 → [OAUser "zhangsan"]
地址 1004 → [OAUser "lisi"]
地址 1008 → [OAUser "ERROR_USER"]

OAUser* _user = 1008;  // _user 裡存著「地址 1008」，指向 ERROR_USER
```

### 宣告指標

```cpp
int x = 42;
int* p;   // p 是一個「指向 int 的指標」（目前沒有指向任何地方，危險！）

// 本專案的範例：
OAUser* _user;  // 指向 OAUser 物件的指標（在 OAMeetingRoom 裡）
```

### 取得地址（& 運算子 = 「取地址」）

`&` 放在變數前面，意思是「給我這個變數的記憶體地址」：

```cpp
int x = 42;
int* p = &x;  // p = x 的地址（比如 1000）

OAUser* _user = &OAUser::ERROR_USER;  // 指向 ERROR_USER 物件的地址
```

### 解參考（* 運算子 = 「去那裡取值」）

`*` 放在指標前面，意思是「去那個地址，把那裡的東西拿給我」：

```cpp
int x = 42;
int* p = &x;   // p 存著 x 的地址
int y = *p;    // *p = 「去 p 的地址，取得那裡的值」= 42
*p = 100;      // 「去 p 的地址，把那裡的值改成 100」→ x 變成 100
```

### 指標在本專案中的所有用途

**1. OAMeetingRoom 的 _user 指標（表示預約狀態）：**
```cpp
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _user = &OAUser::ERROR_USER;  // 空房間：指向 ERROR_USER
}
```

**2. bookMeetingRoom 中儲存使用者地址：**
```cpp
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    // user 是參考（別名）
    // &user 取得原始物件的地址
    it->_user = &user;  // 把地址存進指標！
    // 現在 _user 指向真實的使用者物件
}
```

**3. cancelBookMeetingRoom 中比較指標：**
```cpp
int OADataCenter::cancelBookMeetingRoom(OAUser& user, int mid) {
    if (it->_user != &user) {  // 比較：_user 存的地址 是否等於 user 的地址
        return CANCEL_MEETING_ROOM_FAIL;  // 不是你訂的！
    }
    it->_user = &OAUser::ERROR_USER;  // 取消：重設回空房間
}
```

**4. 清除所有預約：**
```cpp
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;  // 把指標重設回 ERROR_USER 地址
    }
}
```

**5. Room 4 用 new 建立物件並存入指標：**
```cpp
OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
// new 在堆積上建立 OAUser 物件，並回傳它的地址
// user 這個指標存著那個地址
OAMeetingRoom r(4, 12);
r._user = user;  // 把那個地址存進 r._user
```

### 指標比較（判斷是否是同一個物件）

```cpp
// 判斷房間是否空著：
if (m.user() == &(OAUser::ERROR_USER)) {
    cout << "reservable!";
}
// m.user() 回傳的是 OAUser* 指標
// &OAUser::ERROR_USER 是 ERROR_USER 物件的地址
// 如果兩個地址相同 → 這間房間指向 ERROR_USER → 空房間

// 判斷登入是否失敗：
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &OAAdmin::ERROR_ADMIN) {
    cout << "Login failed";
}
// loginAdmin 回傳 OAAdmin 的參考
// &admin 取得這個參考所代表物件的地址
// 如果地址等於 ERROR_ADMIN → 登入失敗
```

### 指標 vs 參考 vs 值

| 特性 | 值 | 參考（&） | 指標（*） |
|------|----|-----------|---------|
| 可以為空（null） | 不行 | 不行 | 可以（nullptr） |
| 可以重新指向 | — | 不行（一旦綁定就固定） | 可以 |
| 語法宣告 | `OAUser u` | `OAUser& r = u` | `OAUser* p = &u` |
| 存取成員 | `u.method()` | `r.method()` | `p->method()` |
| 何時用 | 需要副本 | 需要別名，不能為空 | 可能為空，需要重新指向 |

---

## 22. 箭頭運算子（->）

### 什麼是 -> ？

當你有一個**指標**，而你想呼叫它所指向物件的方法或存取成員，你用 `->`：

```cpp
OAUser* p = &someUser;

// 方法一：先解參考再用點運算子（麻煩）
(*p).username();
(*p)._username;

// 方法二：用箭頭運算子（簡潔，推薦）
p->username();
p->_username;
```

`p->method()` 就是 `(*p).method()` 的簡寫。

### 本專案中的所有 -> 用法

**1. OAAdmin.cpp - showAllMeetingRooms（透過 user() 指標存取預約者資訊）：**
```cpp
void OAAdmin::showAllMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& m : rooms) {
        if (m.user() == &(OAUser::ERROR_USER)) {
            cout << ", reservable! " << endl;
        } else {
            // m.user() 回傳 OAUser*（指標）
            // 用 -> 呼叫指標所指物件的方法
            cout << ", this meeting room is reserved by "
                 << m.user()->department()  // OAUser* → department()
                 << "'s " << m.user()->username()  // OAUser* → username()
                 << "!" << endl;
        }
    }
}
```

**2. OADataCenter.cpp - 使用迭代器存取成員（迭代器也用 ->）：**
```cpp
for (; it != rooms.end(); it++) {
    if (it->_mid == mid) {  // it 是迭代器，用 -> 存取它指向元素的 _mid
        break;
    }
}
if (it->_user != &OAUser::ERROR_USER) {  // 存取 _user 私有成員
    return MEETING_ROOM_BOOKED;
}
it->_user = &user;  // 修改 _user 成員
```

**3. OAUser.cpp - 使用 this 指標存取私有成員：**
```cpp
void OAUser::showAllBookedMeetingRooms() {
    // this 是指向「自己」的指標
    // 用 this-> 存取自己的私有成員（通常可以直接寫 _department，效果相同）
    cout << this->_department << "'s " << this->_username << " already booked!";
    //    ^^^^^                          ^^^^^
    //    等同於直接寫 _department、_username，但 this-> 更明確
}
```

### 記憶口訣

- **點運算子 `.`** → 物件本身（值）或參考（`&`）→ 用點
- **箭頭運算子 `->`** → 指標（`*`）→ 用箭頭

```cpp
OAUser  u;      // 物件（值）
OAUser& r = u;  // 參考（別名）
OAUser* p = &u; // 指標（地址）

u.username();   // 值 → 用點
r.username();   // 參考 → 用點（和值一樣）
p->username();  // 指標 → 用箭頭
```

---

## 23. 迭代器（Iterator）

### 什麼是迭代器？

迭代器是一個「位置標記」，告訴你現在在容器的哪個位置。就像書籤——它記住你讀到哪一頁，而且可以向前翻頁。

```
vector<OAUser>: [zhangsan] [lisi] [wangwu]
                    ↑
                  迭代器（目前指向 zhangsan）
```

### 宣告迭代器

```cpp
vector<OAUser> users;

// 方法一：明確寫出型別（很長）
vector<OAUser>::iterator it = users.begin();

// 方法二：用 auto（編譯器自動推導型別，更簡潔）
auto it = users.begin();
```

### begin() 和 end()

```cpp
vector<int> v = {10, 20, 30};
//                ↑           ↑
//            begin()       end()（指向最後一個元素之後的「超出末尾」位置！）
```

**重要**：`end()` 不指向最後一個元素，而是指向「超出末尾」的位置。這是 C++ 的慣例，用來表示「已經到達終點」。

### 本專案中的迭代器用法

**1. deleteUser() - 找到並刪除使用者：**
```cpp
bool OADataCenter::deleteUser(string username) {
    auto it = users.begin();           // 從第一個元素開始
    for (; it != users.end(); it++) {  // 一個一個往後走
        if (it->username() == username) {  // 找到了！（用 -> 存取）
            break;                     // 停下來，it 指向要刪除的元素
        }
    }

    if (it == users.end()) {           // 走完了都沒找到
        return false;
    }

    users.erase(it);                   // 刪除 it 指向的元素
    return true;
}
```

**2. deleteMeetingRoom() - 明確型別版本：**
```cpp
bool OADataCenter::deleteMeetingRoom(int mid) {
    vector<OAMeetingRoom>::iterator it = rooms.begin();  // 明確寫出迭代器型別
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {         // friend 關係：直接存取私有 _mid
            break;
        }
    }
    if (it == rooms.end()) return false;
    rooms.erase(it);
    return true;
}
```

**3. bookMeetingRoom() - 找到房間後修改 _user：**
```cpp
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) { break; }
    }
    if (it == rooms.end()) { return MEETING_ROOM_NOT_FOUND; }
    if (it->_user != &OAUser::ERROR_USER) { return MEETING_ROOM_BOOKED; }
    it->_user = &user;  // 透過迭代器修改元素的私有成員（friend）
    return MEETING_ROOM_BOOK_SUCCESS;
}
```

**4. cancelBookMeetingRoom() - 驗證後取消預約：**
```cpp
int OADataCenter::cancelBookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) { break; }
    }
    if (it == rooms.end()) { return CANCEL_MEETING_ROOM_NOT_FOUND; }
    if (it->_user != &user) { return CANCEL_MEETING_ROOM_FAIL; }
    it->_user = &OAUser::ERROR_USER;
    return CANCEL_MEETING_ROOM_SUCCESS;
}
```

### erase() 方法

`erase()` 接受一個迭代器，刪除它所指向的元素，並自動把後面的元素往前移：

```cpp
vector<int> v = {1, 2, 3, 4, 5};
auto it = v.begin() + 2;  // 指向 3（跳過兩個）
v.erase(it);              // 刪除 3
// v 現在是 {1, 2, 4, 5}（後面元素自動前移）
```

### 範圍式 for vs 迭代器 for

```cpp
// 範圍式 for（簡單遍歷，讀或改元素，但不能刪除）
for (OAUser& u : users) {
    cout << u.username();
}

// 迭代器 for（需要刪除元素時使用）
for (auto it = users.begin(); it != users.end(); it++) {
    if (it->username() == "delete_me") {
        users.erase(it);  // 刪除後 it 失效，所以必須 break
        break;
    }
}
```

**為什麼範圍式 for 不能刪除？** 因為刪除元素後，迭代器會失效，範圍式 for 內部也是用迭代器，繼續使用失效的迭代器會導致程式崩潰。

---

## 24. auto 關鍵字

### 什麼是 auto？

`auto` 告訴編譯器：「你自己去推斷這個變數是什麼型別，我懶得寫。」

```cpp
auto x = 42;           // 編譯器知道 x 是 int
auto name = "Alice";   // 編譯器知道 name 是 const char*
auto it = v.begin();   // 編譯器知道 it 是 vector<...>::iterator
```

### 為什麼需要 auto？

有些型別名字很長，容易打錯：

```cpp
// 不用 auto：很長很難寫
vector<OAMeetingRoom>::iterator it = rooms.begin();

// 用 auto：簡潔，而且不容易打錯
auto it = rooms.begin();
```

兩者完全等效。

### 本專案中的 auto

**四個 bookMeetingRoom / cancelBookMeetingRoom 函式都用 auto：**
```cpp
// OADataCenter.cpp
auto it = rooms.begin();     // 代替 vector<OAMeetingRoom>::iterator
// 或：
auto it = users.begin();     // 代替 vector<OAUser>::iterator
```

**為什麼這個比另一個好看：**
```cpp
// 沒有 auto（deleteUser）：
vector<OAUser>::iterator it = users.begin();

// 有 auto（bookMeetingRoom）：
auto it = rooms.begin();
```

兩種都在本專案中使用，兩種都正確，`auto` 只是打字更方便。

### auto 的限制

- `auto` 必須在宣告時就初始化（因為需要右邊的值才能推斷型別）
- 不能用 `auto` 宣告函式的參數（C++20 之前）

```cpp
auto x;        // 錯誤！沒有初始值
auto x = 5;    // 正確！推斷為 int
```

---

## 25. 前向宣告（Forward Declaration）

### 什麼是前向宣告？

有時候兩個類別互相需要對方，會造成「先有雞還是先有蛋」的問題。前向宣告就是說：「我保證這個類別存在，詳細定義在別的地方。」

### 問題：循環依賴

```
OADataCenter.h 引入了 OAAdmin.h（因為用到 OAAdmin）
OAAdmin.h 引入了 OADataCenter.h（因為用到 OADataCenter）
→ 循環！誰先誰後？
```

### 解決方案：前向宣告

**OADataCenter.h 中的寫法：**
```cpp
#pragma once
#include <vector>
#include "OAAdmin.h"
#include "OAUser.h"
#include "OAMeetingRoom.h"

// 前向宣告：「我保證這些類別存在，完整定義在對應的 .h 檔案裡」
class OAAdmin;
class OAUser;
class OAMeetingRoom;

class OADataCenter {
   private:
    vector<OAAdmin> admins;   // 因為有前向宣告，編譯器知道 OAAdmin 存在
    vector<OAUser> users;
    vector<OAMeetingRoom> rooms;
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
    friend class OADataCenter;
   private:
    OAUser* _user;  // 可以宣告指向 OAUser 的指標（因為指標大小固定）
   // ...
};
```

### 為什麼指標可以用前向宣告，值不行？

```cpp
class Foo;  // 前向宣告（只知道名字，不知道大小）

Foo f;      // 錯誤！編譯器需要知道 Foo 有多大才能分配空間
Foo* p;     // 正確！指標大小固定（通常 8 bytes），不需要知道 Foo 的大小
```

這就是為什麼 `OAMeetingRoom` 用 `OAUser*`（指標）而不是 `OAUser`（值）的原因之一。

---

## 26. Facade 設計模式

### 什麼是 Facade（門面）模式？

Facade 模式就像飯店的**前台接待員**。客人不需要知道「廚房在哪裡、清潔阿姨幾點上班、保安怎麼換班」，只要跟接待員說「我要一杯咖啡」，接待員會幫你協調好一切。

### 本專案中有兩個 Facade

**OAAdmin 是管理員的 Facade：**

```
OASystemUI（客人）
    ↓ 呼叫 admin.addUser()
OAAdmin（接待員）
    ↓ 委託給 OADataCenter::getInstance().addUser()
OADataCenter（後台）
```

```cpp
// OAAdmin.cpp：幾乎所有方法都是「委託」
bool OAAdmin::addUser(string username, string password, string department) {
    return OADataCenter::getInstance().addUser(username, password, department);
    // OAAdmin 自己沒有做任何實際工作，只是轉發請求
}

bool OAAdmin::deleteMeetingRoom(int mid) {
    return OADataCenter::getInstance().deleteMeetingRoom(mid);
}

void OAAdmin::clearMeetingRoomStatus() {
    OADataCenter::getInstance().clearMeetingRoomStatus();
}
```

**OAUser 也是使用者的 Facade：**

```
OASystemUI（客人）
    ↓ 呼叫 user.bookMeetingRoom(mid)
OAUser（接待員）
    ↓ 委託給 OADataCenter::getInstance().bookMeetingRoom(*this, mid)
OADataCenter（後台）
```

```cpp
// OAUser.cpp：預約和取消也都是委託
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
    // 把「自己」(*this) 和房間 ID (mid) 一起傳給 OADataCenter
}

int OAUser::cancelBookMeetingRoom(int mid) {
    return OADataCenter::getInstance().cancelBookMeetingRoom(*this, mid);
}
```

**注意 `*this` 的作用：** `bookMeetingRoom` 接受 `OAUser& user` 參數。`this` 是指向「我自己」的指標，`*this` 解參考後就是「我自己」這個物件（值）。這樣 OADataCenter 才能拿到「是誰在預約」這個資訊，並存儲 `&user` 地址。

### Facade 的好處

1. **簡化介面**：`OASystemUI` 只需要呼叫 `admin.addUser()`，不需要知道資料如何儲存
2. **降低耦合**：如果 `OADataCenter` 的內部改了，只需要更新 `OAAdmin`，不用改 UI
3. **更好的組織**：每個類別只負責一件事，關注點分離

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
cin >> choice;    // 讀取數字後，Enter 鍵的 '\n' 還留在緩衝區
// 緩衝區現在是：'\n'
cin.get();        // 第一次呼叫只是消耗掉那個 '\n'
// 緩衝區現在是空的
// 使用者什麼都沒有做，程式就繼續了！
```

這就是為什麼本專案在 `showAdminAddUserPage` 裡還有：
```cpp
if (admin.addUser(username, password, department)) { ... }
cout << "Press Enter to continue...";
cin.get();  // 這裡因為前面用了 cin >> username/password/department
            // 最後一個 cin >> 讀完後，緩衝區裡只剩 '\n'
            // 所以這個 cin.get() 剛好消耗掉那個 '\n'，效果正確
```

### system("clear") — 清除螢幕

```cpp
system("clear");  // 在 macOS/Linux 上清除終端機螢幕
```

**system() 是什麼？**

`system()` 讓你的 C++ 程式執行一個**作業系統指令**，就像在終端機輸入指令一樣：

```cpp
system("clear");  // 相當於在 Mac/Linux 終端機輸入 clear
system("cls");    // 相當於在 Windows 命令提示字元輸入 cls
system("ls");     // 相當於在終端機輸入 ls（列出檔案，但不建議在程式裡用）
```

**本專案中哪些地方用了 system("clear")：**
- `showAdminAddUserPage()` — 進入新增用戶頁面前清屏
- `showAdminDeleteUserPage()` — 進入刪除用戶頁面前清屏
- `showAdminAddMeetingRoomPage()` — 進入新增會議室頁面前清屏
- `showAdminDeleteMeetingRoomPage()` — 進入刪除會議室頁面前清屏
- `showAdminMainPage()` — 每次顯示管理員選單前清屏
- `showUserBookMeetingRoomPage()` — 進入預約頁面前清屏
- `showUserCancelBookMeetingRoom()` — 進入取消頁面前清屏
- `showUserMainPage()` — 每次顯示用戶選單前清屏

**缺點和注意事項：**
- `system("clear")` 只在 macOS/Linux 有效；Windows 要用 `system("cls")`
- 這是「平台依賴」的程式碼
- 有安全疑慮（不要用 `system()` 執行來自使用者輸入的指令！）

### \033[2J\033[H — ANSI 跳脫碼清除螢幕

```cpp
cout << "\033[2J\033[H";
// \033  = ESC 字元（ASCII 27），終端機控制命令的開頭
// [2J   = 清除整個螢幕
// [H    = 把游標移到左上角（第 1 行第 1 列）
```

本專案在 `showLogin()` 和 `displayMainPage()` 的結尾用這個方式清屏，而其他函式用 `system("clear")`。兩種方式效果相同，但 ANSI 跳脫碼更「純 C++」。

---

## 28. OAMeetingRoom 完整解析

### 類別結構

**OAMeetingRoom.h：**
```cpp
class OAMeetingRoom {
    friend class OADataCenter;  // 允許 OADataCenter 存取所有私有成員

   private:
    int _mid;       // 會議室編號（1, 2, 3, 4...）
    int _capacity;  // 容納人數（10, 6, 4, 12...）
    OAUser* _user;  // 指向預約者的指標
                    // = &ERROR_USER → 沒人預約（空房）
                    // = &zhangsan  → zhangsan 預約了

   public:
    OAMeetingRoom();                      // 預設建構函式
    OAMeetingRoom(int mid, int capacity); // 帶參數建構函式

    int mid();       // 回傳 _mid（getter）
    int capacity();  // 回傳 _capacity（getter）
    OAUser* user();  // 回傳 _user（getter，注意：回傳指標！）
};
```

### 為什麼用指標（OAUser*）而不是值（OAUser）？

1. **表達「沒有人」的狀態**：指標可以指向特殊的 `ERROR_USER` 來表示「空房間」
2. **避免「過時副本」問題**：如果用值，OAMeetingRoom 裡的 OAUser 是副本，無法和 OADataCenter 的原始使用者比較
3. **前向宣告需求**：OAMeetingRoom.h 需要在知道 OAUser 完整定義前就使用它（指標大小固定）
4. **效能**：指標只有 8 bytes，複製便宜；OAUser 物件大得多

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

### 預約狀態的完整生命週期

```
建立時：
  OAMeetingRoom(1, 10)
  _user = &ERROR_USER  → 空房間

預約後（bookMeetingRoom）：
  it->_user = &user    → 指向 zhangsan
  _user = &zhangsan    → 有人了！

取消後（cancelBookMeetingRoom）：
  it->_user = &ERROR_USER  → 空房間

清除所有（clearMeetingRoomStatus）：
  room._user = &ERROR_USER  → 全部清空
```

### 初始資料（OADataCenter.cpp）

系統啟動時自動建立 4 間會議室：

```cpp
OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    // 前 3 間：空的
    rooms.push_back(OAMeetingRoom(1, 10));  // 1 號室，10 人，空
    rooms.push_back(OAMeetingRoom(2, 6));   // 2 號室，6 人，空
    rooms.push_back(OAMeetingRoom(3, 4));   // 3 號室，4 人，空

    // 第 4 間：一開始就被 xiaoxiao 預約了
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    //             ^^^
    //             new 在堆積（heap）上建立 OAUser 物件，回傳地址
    OAMeetingRoom r(4, 12);   // 4 號室，12 人
    r._user = user;            // 設為已預約（friend 關係才能直接改 _user）
    rooms.push_back(r);
}
```

**啟動後的狀態：**
```
rooms[0]: MeetingRoom #1, 10人, _user → ERROR_USER（空）
rooms[1]: MeetingRoom #2, 6人,  _user → ERROR_USER（空）
rooms[2]: MeetingRoom #3, 4人,  _user → ERROR_USER（空）
rooms[3]: MeetingRoom #4, 12人, _user → xiaoxiao（已被預約）
```

### getUsers() 和 getRooms() — 回傳副本

```cpp
vector<OAUser> OADataCenter::getUsers() {
    vector<OAUser> users_back(users);  // 複製整個 vector（每個 OAUser 都被複製）
    return users_back;
}

vector<OAMeetingRoom> OADataCenter::getRooms() {
    return vector<OAMeetingRoom>(rooms);  // 同樣回傳副本
}
```

**回傳副本而不是參考的原因：** 安全！外部程式碼拿到的是複印件，不是原件。修改副本不影響 OADataCenter 的內部資料。

**但是指標值仍然相同！** 這是關鍵：

```cpp
// 原始 rooms 裡的 Room #4：
rooms[3]._user = 某個地址（指向 xiaoxiao）

// 副本裡的 Room #4：
copy[3]._user = 同樣的地址（指向同一個 xiaoxiao）

// 所以比較指標仍然有效：
if (copy[3].user() == this) { ... }  // 地址比較，正確！
```

---

## 29. this 指標

### 什麼是 this？

在類別的方法內部，`this` 是一個特殊的**指標**，它自動指向「呼叫這個方法的那個物件」。

就像你自我介紹時說「我」——「我的名字是...」，「我的部門是...」，`this` 就是 C++ 裡的「我」。

```cpp
class OAUser {
   private:
    string _username;

   public:
    void printInfo() {
        // 在方法內部，以下兩種寫法完全等效：
        cout << _username;        // 隱式使用 this
        cout << this->_username;  // 明確使用 this
        //     ^^^^
        //     this 是指向「我自己（呼叫這個方法的 OAUser 物件）」的指標
    }
};
```

### 本專案中的 this 用法

**1. 在 showAllBookedMeetingRooms 中存取私有成員：**
```cpp
void OAUser::showAllBookedMeetingRooms() {
    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {
            // 這裡用 this-> 明確表示「這是我自己的成員」
            cout << this->_department << "'s " << this->_username << " already booked!";
            //     ^^^^^^^^^^^^^^^^^ 私有成員，但在類別方法裡可以存取
        }
    }
}
```

**2. 最重要：比較指標 r.user() == this**
```cpp
void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {
        //  ^^^^^^^^   ^^^^
        //  r.user() 是 OAUser* 指標（指向預約這間房間的用戶）
        //  this 也是 OAUser* 指標（指向「我自己」）
        //  如果兩個指標指向同一個地址 → 這間房間是「我」預約的！
        }
    }
}

// 例子：
// zhangsan 呼叫 showAllBookedMeetingRooms()
// → this = &zhangsan 的地址（比如 1000）
// → 遍歷所有房間：
//     Room 1: r.user() = &ERROR_USER（不是 1000）→ 跳過
//     Room 2: r.user() = 1000（是 zhangsan！）→ 顯示
//     Room 3: r.user() = &lisi（不是 1000）→ 跳過
```

**3. 最重要：*this 解參考（傳給另一個函式）**
```cpp
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
    //                                                  ^^^^^
    //  this 是指標（OAUser*），*this 是「把指標解參考，得到物件本身」
    //  OADataCenter::bookMeetingRoom 接受 OAUser& user（參考）
    //  *this 就是「我自己這個 OAUser 物件」，可以作為參考傳過去
}
```

**為什麼要 `*this` 而不是直接 `this`？**

```cpp
// OADataCenter 的函式簽名：
int bookMeetingRoom(OAUser& user, int mid);
//                  ^^^^^^^^
//                  接受 OAUser 的「參考」

// 呼叫方式：
OADataCenter::getInstance().bookMeetingRoom(*this, mid);
//                                           ^^^^^
// *this 是 OAUser 物件（值），可以作為 OAUser& 參考傳入

// 如果直接傳 this：
OADataCenter::getInstance().bookMeetingRoom(this, mid);  // 錯誤！
// this 是 OAUser* 指標，不是 OAUser&，型別不匹配
```

### this 的型別

```cpp
class OAUser {
    void someMethod() {
        // this 的型別是 OAUser*（指向 OAUser 的指標）
        OAUser* p = this;       // 可以把 this 存起來
        OAUser& r = *this;      // 解參考後可以作為參考
        OAUser copy = *this;    // 解參考後可以複製
    }
};
```

### this 指標的視覺化

```
// 假設 zhangsan 物件在地址 1000：
OAUser zhangsan("zhangsan", "123123", "Admin Resource");
// 記憶體: 地址 1000 → [_username="zhangsan", _password="123123", _department="Admin Resource"]

zhangsan.bookMeetingRoom(2);
// → 進入 OAUser::bookMeetingRoom 方法
// → this = 1000（指向 zhangsan 的地址）
// → *this = zhangsan 物件本身
// → 呼叫 bookMeetingRoom(*this, 2) = bookMeetingRoom(zhangsan, 2)
// → OADataCenter 儲存 &user = 1000 在 rooms[1]._user 裡
```

### 何時不需要寫 this->

在類別方法內，直接寫成員名字和寫 `this->成員名字` 效果完全一樣。只有兩種情況需要明確寫 `this->`：

```cpp
// 1. 當參數名稱和成員名稱相同時（避免歧義）：
void setUsername(string username) {
    this->_username = username;  // this->_username 是成員，username 是參數
    // _username = username; 也可以，因為 _username 前有底線（不同名）
}

// 2. 想回傳自己（支援方法鏈）：
OAUser& setUsername(string name) {
    _username = name;
    return *this;  // 回傳「自己」，讓呼叫者可以繼續 .setPassword(...) 等
}
```

---

## 30. new 關鍵字與動態記憶體

### 堆疊（Stack）vs 堆積（Heap）

C++ 有兩個主要的記憶體區域：

```
┌─────────────────────────────────────────────────────┐
│                        Stack（堆疊）                 │
│                                                      │
│   OAAdmin admin("admin", "admin");  ← 存在這裡      │
│   int mid = 5;                      ← 存在這裡      │
│                                                      │
│   特點：                                             │
│   - 函式結束時自動釋放                              │
│   - 速度快                                           │
│   - 大小有限                                         │
├─────────────────────────────────────────────────────┤
│                        Heap（堆積）                  │
│                                                      │
│   new OAUser("xiaoxiao", ...)  ← 存在這裡           │
│                                                      │
│   特點：                                             │
│   - 必須手動 delete 才能釋放                        │
│   - 速度稍慢                                         │
│   - 大小大得多（幾 GB）                              │
└─────────────────────────────────────────────────────┘
```

### new 關鍵字

`new` 在堆積上建立一個物件，並回傳指向它的指標：

```cpp
// 在堆疊上建立（函式結束自動消失）：
OAUser user1("zhangsan", "123123", "Admin");  // 存在 stack

// 在堆積上建立（必須手動 delete）：
OAUser* user2 = new OAUser("lisi", "123456", "HR");  // 存在 heap
//       ^                  ^^^ 回傳地址，存入指標
```

### 本專案中的 new

**OADataCenter.cpp 建構函式中的 Room 4：**
```cpp
OADataCenter::OADataCenter() {
    // ...前面的程式碼...

    // 這裡用 new 建立 OAUser 物件在堆積上：
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    //      ^^^^   ^^^
    //      指標    在堆積上建立，回傳地址存入 user 指標

    OAMeetingRoom r(4, 12);
    r._user = user;     // 把堆積上物件的地址存進 r._user
    rooms.push_back(r); // 把 r 複製進 vector（_user 指標的值也被複製）
}
```

**為什麼用 new 而不是直接：**
```cpp
// 如果不用 new（堆疊）：
{
    OAUser tempUser("xiaoxiao", "123123", "abc");  // 建立在 stack
    OAMeetingRoom r(4, 12);
    r._user = &tempUser;  // 指向 stack 上的物件
    rooms.push_back(r);   // r 被複製進 vector，_user 指向 stack 物件
}  // ← 離開大括號後，tempUser 被銷毀！
   //   r._user 現在是懸空指標（dangling pointer），指向已被銷毀的記憶體！
   //   這是一個嚴重的 bug！

// 用 new（堆積）：
OAUser* user = new OAUser("xiaoxiao", "123123", "abc");  // 建立在 heap
OAMeetingRoom r(4, 12);
r._user = user;   // 指向 heap 上的物件
rooms.push_back(r);
// heap 上的物件不會自動消失，所以指標一直有效
```

### new 的語法

```cpp
// 基本語法：
Type* ptr = new Type;              // 呼叫預設建構函式
Type* ptr = new Type(arg1, arg2);  // 呼叫帶參數建構函式

// 本專案的用法：
OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
//              ^^^ 關鍵字
//                   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 建構函式參數
```

### delete 關鍵字（本專案沒有用到）

`new` 建立的物件**必須**用 `delete` 釋放，否則會造成「記憶體洩漏」（Memory Leak）：

```cpp
OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
// ... 使用 user ...
delete user;   // 釋放記憶體（本專案沒有這行！→ 記憶體洩漏）
user = nullptr; // 好習慣：釋放後把指標設為 nullptr
```

**本專案的問題：** `OADataCenter` 建構函式用了 `new` 建立 `xiaoxiao`，但程式結束時沒有 `delete`。這是一個輕微的記憶體洩漏，在這個小程式中無關緊要，但在大型程式裡要避免。

### 現代 C++ 的智慧指標（解決 delete 的問題）

```cpp
#include <memory>

// unique_ptr：唯一擁有者，自動在超出作用域時 delete
unique_ptr<OAUser> user = make_unique<OAUser>("xiaoxiao", "123123", "abc");
// 不需要手動 delete！

// shared_ptr：多個擁有者，引用計數為 0 時自動 delete
shared_ptr<OAUser> user = make_shared<OAUser>("xiaoxiao", "123123", "abc");
```

現代 C++ 幾乎不再需要裸指標 + 手動 `delete`，建議使用智慧指標。

---

## C++ 概念總結

| 概念 | 使用位置 | 重點 |
|------|----------|------|
| 標頭/原始碼分離 | 所有檔案 | .h 宣告，.cpp 實作 |
| `#pragma once` | 所有標頭檔 | 防止重複引入 |
| `#include` | 所有檔案 | 複製貼上標頭內容 |
| `#define` 巨集 | OASystemUI.cpp, OAUtils.h | 文字替換，有意義的常數名 |
| `using namespace` | 多個檔案 | 省略 std:: 前綴 |
| 類別 | 所有 OA 類別 | 藍圖 → 建立物件 |
| 存取修飾符 | 所有類別 | public/private 保護資料 |
| 建構函式 | 所有類別 | 物件誕生時初始化 |
| 建構函式多載 | OAAdmin, OAMeetingRoom | 不同參數的多個版本 |
| 靜態成員 | OAUtils, OADataCenter, OAAdmin, OAUser | 屬於類別，非物件 |
| 友元類別 | OADataCenter 是三個類別的友元 | 存取私有成員 |
| 參考（`&`） | loginAdmin 回傳型別、for 迴圈、bookMeetingRoom 參數 | 別名，避免複製 |
| 單例模式 | OADataCenter | 整個程式只有一個實例 |
| STL vector | OADataCenter 的三個 vector | 動態陣列 |
| cin/cout | 多個檔案 | 輸入/輸出串流 |
| 流程控制 | OASystemUI.cpp | while/switch/if/goto |
| 範圍式 for | OADataCenter.cpp, OAUser.cpp, OAAdmin.cpp | 簡潔遍歷容器 |
| 範圍解析（`::`） | 所有 .cpp 檔案 | 指定成員屬於哪個類別/命名空間 |
| 標記值錯誤處理 | ERROR_ADMIN、ERROR_USER | 登入失敗的標記物件 |
| 整數回傳代碼 | bookMeetingRoom、cancelBookMeetingRoom | #define 代碼 + int 回傳值 |
| 指標（`*`） | OAMeetingRoom._user（`OAUser*`）, new 的結果 | 記憶體地址 |
| 取地址（`&`） | `&OAUser::ERROR_USER`、`&user`、`&admin` | 取得物件的地址 |
| 箭頭運算子（`->`） | `m.user()->username()`、`it->_mid`、`this->_department` | 指標存取成員 |
| 迭代器 | deleteUser、deleteMeetingRoom、bookMeetingRoom、cancelBookMeetingRoom | 容器的位置標記 |
| `auto` 關鍵字 | `auto it = users.begin()` | 自動推導型別 |
| 前向宣告 | OADataCenter.h、OAMeetingRoom.h | 解決循環依賴 |
| Facade 設計模式 | OAAdmin 和 OAUser 委託給 OADataCenter | 簡化介面 |
| Singleton 設計模式 | OADataCenter | 唯一實例 |
| `cin.get()` | 多個 UI 函式 | 等待按 Enter |
| `system("clear")` | 多個 UI 函式 | 清除終端機螢幕 |
| `this` 指標 | OAUser::showAllBookedMeetingRooms, bookMeetingRoom | 指向「我自己」 |
| `*this` | OAUser::bookMeetingRoom, cancelBookMeetingRoom | 解參考 this，得到物件本身 |
| `new` 關鍵字 | OADataCenter 建構函式（Room 4） | 在堆積上建立物件 |

---

## 後續學習建議

1. **修復記憶體洩漏**：替換 Room 4 的 `new OAUser(...)` 為 `make_unique<OAUser>(...)` 或在 OADataCenter 的解構函式裡 `delete`

2. **指標進階**：學習 `delete`、`nullptr`、智慧指標（`unique_ptr`、`shared_ptr`），這是現代 C++ 記憶體管理的核心

3. **繼承**：`OAAdmin` 和 `OAUser` 其實可以繼承同一個基底類別 `OAPerson`（都有 `_username` 和 `_password`），學習類別階層和虛擬函式

4. **模板**：`vector<T>` 本身就是模板的應用，學習泛型程式設計，可以建立可用於任何型別的函式和類別

5. **現代 C++**：探索 C++11/14/17/20 特性：`std::optional`、`std::variant`、結構化綁定、範圍演算法

6. **STL 進階**：學習更多容器（`map` 用來建立 ID 到物件的映射、`set`、`unordered_map`）和演算法（`std::find_if`、`std::remove_if`）

7. **例外處理**：`try`/`catch`/`throw`——比本專案的整數代碼和標記值更現代的錯誤處理方式

8. **資料持久化**：使用檔案 I/O（`fstream`）儲存資料，讓重啟後資料不會消失

祝學習愉快！
