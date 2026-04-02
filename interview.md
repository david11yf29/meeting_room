# C++ 面試準備指南 — 半導體設備公司
## ASML / KLA / LAM Research / Applied Materials（AMAT）

本文件從我們的 OA 會議室預約專案中，萃取出與半導體設備公司技術面試**直接相關**的 C++ 概念。這些公司建造的是任務關鍵系統（微影、晶圓檢測、蝕刻、沉積），軟體的可靠性與正確性是毫無妥協空間的底線。

---

## 為什麼半導體設備業大量使用 C++

| 公司 | 主要產品 | 為什麼用 C++ |
|------|---------|------------|
| ASML | EUV/DUV 微影機台 | 即時運動控制、硬體抽象層 |
| KLA | 晶圓檢測與量測 | 高吞吐量影像處理、資料管線 |
| LAM Research | 電漿蝕刻／沉積腔體 | 設備控制軟體、製程配方管理 |
| AMAT（應用材料） | CVD/PVD/CMP 工具機台 | 硬體驅動程式、製程控制、診斷系統 |

這四家公司都高度依賴 C++ 的原因：
- **確定性效能**：沒有垃圾回收（GC）的暫停延遲
- **接近硬體的程式設計**：直接記憶體存取、裝置驅動程式
- **長壽命程式碼庫**：產品生命週期長達 10～20 年，需要可維護的 OOP 設計
- **安全關鍵的可靠性**：一個 bug 可能毀掉價值上億的晶圓批次

---

## 目錄

1. [記憶體管理 — 指標、參考、new/delete](#1-記憶體管理--指標參考newdelete)
2. [OOP 基礎 — 封裝、類別、存取控制](#2-oop-基礎--封裝類別存取控制)
3. [建構函式與物件生命週期](#3-建構函式與物件生命週期)
4. [靜態成員](#4-靜態成員)
5. [設計模式 — 單例與門面](#5-設計模式--單例與門面)
6. [錯誤處理 — 回傳代碼 vs 例外](#6-錯誤處理--回傳代碼-vs-例外)
7. [STL 容器與迭代器](#7-stl-容器與迭代器)
8. [友元類別](#8-友元類別)
9. [this 指標](#9-this-指標)
10. [前向宣告與標頭檔組織](#10-前向宣告與標頭檔組織)
11. [常見面試問題與標準回答](#11-常見面試問題與標準回答)
12. [OOP 進階 — 繼承、多態、虛函數](#12-oop-進階--繼承多態虛函數)
13. [類型推導與函數式工具](#13-類型推導與函數式工具)
14. [進階記憶體管理](#14-進階記憶體管理)
15. [智慧指標與現代-c11](#15-智慧指標與現代-c11)
16. [動態庫與靜態庫](#16-動態庫與靜態庫)
17. [並發與執行緒](#17-並發與執行緒)
18. [網路與系統](#18-網路與系統)
19. [C++ 學習路線](#19-c-學習路線)

---

## 1. 記憶體管理 — 指標、參考、new/delete

> **半導體設備公司為什麼在乎這個**：設備控制軟體負責管理硬體資源（相機緩衝區、感測器資料、DMA 區域）。記憶體洩漏或懸空指標會在製程執行中途讓設備當機，造成昂貴的晶圓報廢損失。

### 指標（`*`）

指標儲存的是**記憶體地址**——它告訴你一個物件「住在哪裡」，而不是物件本身。

```cpp
// 宣告
int x = 42;
int* p = &x;   // p 存著 x 的地址（例如地址 1000）

// 解參考 — 去那個地址取值
int y = *p;    // y = 42
*p = 100;      // 透過指標把 x 改為 100
```

**在我們的專案中** — `OAMeetingRoom` 用指標記錄是誰預約了這間會議室：

```cpp
class OAMeetingRoom {
   private:
    OAUser* _user;  // 指向預約者
                    // 如果沒人預約，則指向 ERROR_USER
};

// 預約：把使用者的地址存入指標
it->_user = &user;   // & 取得 user 的地址

// 取消：重設為「沒人預約」
it->_user = &OAUser::ERROR_USER;

// 檢查：比較地址
if (m.user() == &OAUser::ERROR_USER) {
    // 這間會議室沒人預約
}
```

**面試小提示**：說明「指向 `ERROR_USER` 的哨兵指標」和 `nullptr` 的差異。兩者都代表「沒有有效物件」——哨兵模式可以避免解參考 null 造成的崩潰；`nullptr` 則是現代標準做法。

---

### 參考（`&`）

參考是一個**別名**——同一個物件的另一個名字。它不能為 null，也不能重新綁定到另一個物件。

```cpp
int x = 10;
int& ref = x;   // ref 就是 x — 同一塊記憶體，不同的名字
ref = 20;       // x 現在是 20
```

**與指標的關鍵差異**：

| | 指標（`*`） | 參考（`&`） |
|--|------------|------------|
| 可以為 null | 可以（`nullptr`） | 不行 |
| 可以重新綁定 | 可以 | 不行 |
| 存取成員的語法 | `p->method()` | `r.method()` |
| 能代表「沒有物件」 | 可以（`nullptr`） | 不行 |

**在我們的專案中** — `loginAdmin` 回傳的是參考，而不是副本：

```cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;               // 成功 — 回傳真實物件的參考
        }
    }
    return OAAdmin::ERROR_ADMIN;        // 失敗 — 回傳哨兵物件的參考
}

// 呼叫端透過比較地址來判斷結果：
OAAdmin& admin = dataCenter.loginAdmin(username, password);
if (&admin == &OAAdmin::ERROR_ADMIN) {  // 比較地址
    // 登入失敗
}
```

如果 `loginAdmin` 回傳的是**值（副本）**，每次回傳都會建立一個有新地址的副本——哨兵檢查 `&admin == &ERROR_ADMIN` 就永遠是 false，整個模式就失效了。

---

### `new` / `delete` 與堆疊（Stack）vs 堆積（Heap）

```
堆疊 Stack（自動儲存）              堆積 Heap（動態儲存）
────────────────────────            ────────────────────────
OAAdmin admin("a","b");             OAUser* u = new OAUser("a","b","c");
  ↑ 離開作用域時自動銷毀              ↑ 存活到你呼叫 delete 為止
  快速，大小有限                      稍慢，容量大得多
  不會有洩漏風險                      如果忘記 delete 就會洩漏
```

**在我們的專案中** — Room 4 一開始就已被預約，所以預約者物件的存活時間必須超過建構函式的作用域：

```cpp
OADataCenter::OADataCenter() {
    // 錯誤做法 — 堆疊物件在建構函式結束時被銷毀：
    // OAUser tempUser("xiaoxiao", "123123", "abc");
    // r._user = &tempUser;   // 建構函式結束後 tempUser 消失 → 懸空指標！

    // 正確做法 — 堆積物件在整個程式存活期間都存在：
    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    OAMeetingRoom r(4, 12);
    r._user = user;         // 安全：堆積物件不會被自動銷毀
    rooms.push_back(r);
}
```

**記憶體洩漏**：呼叫了 `new` 但永遠沒有呼叫 `delete`——`xiaoxiao` 的記憶體直到程式結束前都不會被釋放。在生產等級的設備軟體中，這應該用智慧指標來修正：

```cpp
// 現代 C++ 的修正方式 — 不需要手動 delete：
#include <memory>
auto user = std::make_unique<OAUser>("xiaoxiao", "123123", "abc");
// 當 OADataCenter 被銷毀時，user 也會自動被 delete
```

**預期會被問到的面試問題**：
- 「什麼是懸空指標？請舉例。」→ 指向一個已超出作用域或已被釋放的記憶體的指標
- 「什麼是記憶體洩漏？如何偵測？」→ 呼叫 `new` 後沒有 `delete`；工具：Valgrind、AddressSanitizer
- 「什麼時候用 `new`，什麼時候用堆疊物件？」→ 當物件的存活時間必須超過當前作用域時用 `new`
- 「什麼是智慧指標？`unique_ptr` 和 `shared_ptr` 分別在什麼時候用？」→ `unique_ptr`：單一擁有者；`shared_ptr`：多個擁有者，透過引用計數管理生命週期

---

## 2. OOP 基礎 — 封裝、類別、存取控制

> **半導體設備公司為什麼在乎這個**：設備軟體的壽命超過 10 年。封裝可以防止外部程式碼破壞硬體關鍵的內部狀態。

### 類別與物件

**類別**是藍圖。**物件**是依據藍圖建造出來的具體實例。

```cpp
// 藍圖（類別定義）：
class OAMeetingRoom {
   private:
    int _mid;
    int _capacity;
    OAUser* _user;   // 指標 — 記錄誰預約了這間房
   public:
    OAMeetingRoom(int mid, int capacity);
    int mid();
    int capacity();
    OAUser* user();
};

// 具體實例（物件）：
OAMeetingRoom room1(1, 10);   // 1 號會議室，10 人座
OAMeetingRoom room2(2, 6);    // 2 號會議室，6 人座
```

### 存取修飾符

```cpp
class Example {
   public:    // 任何地方都可以存取 — 對外公開的 API
    void doSomething();

   private:   // 只有這個類別（以及 friend）可以存取 — 受保護的內部狀態
    int _data;

   protected: // 這個類別以及衍生類別可以存取 — 用於繼承架構
    int _sharedData;
};
```

**封裝的實際效果** — 沒有 `private` 的話，任何人都可以破壞狀態：

```cpp
// 沒有 private — 危險：
room._user = someRandomPointer;  // 可能直接破壞預約狀態！

// 有 private — 必須走過有驗證的方法：
dataCenter.bookMeetingRoom(user, mid);  // 先驗證，再安全地設定 _user
```

**在我們的專案中**，只有 `OADataCenter`（透過 `friend`）才能直接碰 `_user`。所有其他程式碼必須透過 `bookMeetingRoom()` / `cancelBookMeetingRoom()`，這些方法會先驗證輸入是否合法。

**預期會被問到的面試問題**：
- 「什麼是封裝？為什麼它重要？」→ 隱藏內部狀態，只對外暴露安全的操作介面
- 「`public`、`private`、`protected` 三者有什麼差別？」
- 「為什麼要把成員變數設為 `private` 並提供 getter，而不是直接設為 `public`？」→ 可以加入驗證邏輯、為未來修改留空間、強制唯讀

---

## 3. 建構函式與物件生命週期

> **半導體設備公司為什麼在乎這個**：硬體資源（檔案 handle、裝置連線、DMA 緩衝區）必須被正確地初始化和清理。建構函式與解構函式負責強制執行這件事——這就是 RAII（資源獲取即初始化）慣用法的核心。

### 建構函式類型

**預設建構函式** — 無參數，初始化為安全的預設值：
```cpp
OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
    _user = &OAUser::ERROR_USER;  // 安全的預設值：沒有人預約
}
```

**帶參數的建構函式** — 用具體資料初始化：
```cpp
OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;  // 新建的房間永遠從「沒人預約」開始
}
```

**建構函式多載** — 同名、不同參數簽名：
```cpp
OAAdmin admin1;                        // 呼叫 OAAdmin()
OAAdmin admin2("admin", "password");   // 呼叫 OAAdmin(string, string)
```

### 初始化列表（推薦語法）

```cpp
// 在建構函式本體中賦值（兩個步驟：先預設建構，再賦值）：
OAAdmin::OAAdmin(string username, string password) {
    _username = username;
    _password = password;
}

// 初始化列表（一個步驟 — 直接初始化，效率更好）：
OAAdmin::OAAdmin(string username, string password)
    : _username(username), _password(password) { }
```

對於 `const` 成員和參考成員，初始化列表是**強制的**——它們在建構完成後就無法再賦值了。

### RAII 模式（設備軟體的核心準則）

RAII 的意思是：在建構函式裡獲取資源，在解構函式裡釋放資源。這樣可以保證即使在例外拋出時，資源也一定會被清理。

```cpp
// RAII 範例（與設備軟體高度相關）：
class CameraSession {
   private:
    int _handle;
   public:
    CameraSession(int id) { _handle = openCamera(id); }   // 獲取資源
    ~CameraSession()       { closeCamera(_handle); }       // 釋放資源
};
// 當 CameraSession 離開作用域時，相機一定會被關閉——
// 即使中途拋出了例外也一樣。
```

**預期會被問到的面試問題**：
- 「什麼是 RAII？為什麼它重要？」→ 確保資源安全、例外安全
- 「初始化列表和在建構函式本體中賦值有什麼差別？」
- 「解構函式什麼時候被呼叫？」→ 離開作用域時（堆疊），呼叫 `delete` 時（堆積）
- 「什麼是三法則 / 五法則 / 零法則（Rule of Three / Five / Zero）？」→ 如果你定義了解構函式、複製建構函式、複製賦值運算子中的任何一個，就應該全部定義（或者用五法則加上移動語意）

---

## 4. 靜態成員

> **半導體設備公司為什麼在乎這個**：硬體管理器、設定儲存庫和日誌系統天生就是單例。`static` 成員是實現它們的基礎建構塊。

### 靜態變數 — 所有實例共享的單一副本

```cpp
class OAAdmin {
   public:
    static OAAdmin ERROR_ADMIN;  // 只有「一份」，被所有 OAAdmin 物件共享
};

// 必須在類別外部的某一個 .cpp 檔案中定義：
OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();
```

在我們的專案中，`ERROR_USER` 身兼兩種用途：
1. 登入失敗的哨兵（`loginUser` 找不到使用者時回傳它）
2. 空房間的哨兵（`OAMeetingRoom::_user` 指向它代表「沒人預約」）

### 靜態方法 — 不需要物件就能呼叫

```cpp
class OAUtils {
   public:
    static int inputNumber();   // 不需要建立物件
};

int choice = OAUtils::inputNumber();  // 用類別名稱呼叫，而非物件
```

**為什麼工具類別要用 static**：硬體工具類別（例如 `CRCCalculator`、`UnitConverter`）沒有狀態——它們根本不需要一個物件實例。

### 靜態 vs 非靜態

| | 靜態 | 非靜態 |
|--|------|--------|
| 屬於 | 類別 | 物件實例 |
| 只有一份 | 是 | 每個物件各自一份 |
| 能存取 `this` | 不能 | 能 |
| 呼叫方式 | `Class::method()` | `object.method()` |

**預期會被問到的面試問題**：
- 「`static` 用在類別成員變數上是什麼意思？」→ 所有實例共享同一份資料
- 「靜態方法可以呼叫非靜態成員嗎？」→ 不行，因為沒有 `this` 指標
- 「為什麼靜態成員變數必須在類別外部定義？」

---

## 5. 設計模式 — 單例與門面

> **半導體設備公司為什麼在乎這個**：這兩個模式在設備軟體中無處不在——「ChuckController（晶圓座控制器）」單例、「RecipeManager（製程配方管理器）」單例、把幾十個伺服驅動器包裝起來的「MotionSubsystem（運動子系統）」門面。

### 單例模式（Singleton）

**意圖**：確保一個類別只有一個實例，並提供全域的存取點。

**在我們的專案中的實作**（`OADataCenter`）：

```cpp
// 標頭檔 — 私有建構函式 + 靜態實例 + 公開存取器：
class OADataCenter {
   private:
    static OADataCenter _instance;   // 唯一的那個實例
    OADataCenter();                   // 私有 — 外部無法建立新的

   public:
    static OADataCenter& getInstance();  // 唯一的取得方式
};

// .cpp — 初始化並提供存取：
OADataCenter OADataCenter::_instance = OADataCenter();

OADataCenter& OADataCenter::getInstance() {
    return _instance;
}

// 使用 — 每個呼叫者拿到的都是「同一個」物件：
OADataCenter& dc = OADataCenter::getInstance();
```

**單例的三大支柱**：
1. **私有建構函式**：防止外部呼叫 `new OADataCenter()`
2. **靜態實例**：與程式共生死，整個執行期間只存在一個
3. **公開的 `getInstance()`**：唯一的存取入口

**真實設備的類比**：「WaferHandler（晶圓搬運器）」單例確保沒有兩個軟體元件能同時命令機械臂——只有一個 handler，所以命令自然是序列化的。

**單例常見的陷阱**（面試官最愛問這個）：
```cpp
// 問題 1：不是執行緒安全的（傳統實作）
// 兩個執行緒同時呼叫 getInstance() 可能建出兩個實例。
// 修正方式：用 mutex，或使用 Meyers' Singleton：
static OADataCenter& getInstance() {
    static OADataCenter instance;  // C++11 保證靜態區域變數的初始化是執行緒安全的
    return instance;
}

// 問題 2：難以進行單元測試
// 單例引入全域狀態——應改為依賴注入（Dependency Injection）以提升可測試性。
```

---

### 門面模式（Facade）

**意圖**：為複雜的子系統提供一個簡化的介面。客戶端不需要了解子系統的內部細節。

**在我們的專案中** — `OAAdmin` 和 `OAUser` 都是 `OADataCenter` 的門面：

```
OASystemUI（客戶端）
    ↓  呼叫 admin.addUser("alice", "pw", "IT")
OAAdmin（門面 — 簡化的介面）
    ↓  委託給 OADataCenter::getInstance().addUser("alice", "pw", "IT")
OADataCenter（子系統 — 複雜的內部實作）
    ↓  搜尋 vector、檢查重複、push 新的 OAUser
```

```cpp
// OAAdmin.cpp — 幾乎每個方法都是純粹的委託：
bool OAAdmin::addUser(string username, string password, string department) {
    return OADataCenter::getInstance().addUser(username, password, department);
}

bool OAAdmin::deleteMeetingRoom(int mid) {
    return OADataCenter::getInstance().deleteMeetingRoom(mid);
}
```

**OAUser 也是一個門面**：
```cpp
// OAUser.cpp — 把預約操作委託給 OADataCenter，透過 *this 把自己傳過去：
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
}

int OAUser::cancelBookMeetingRoom(int mid) {
    return OADataCenter::getInstance().cancelBookMeetingRoom(*this, mid);
}
```

**真實設備的類比**：「ProcessRecipeManager（製程配方管理器）」門面隱藏了配方是存在資料庫、檔案還是記憶體中。UI 只需要呼叫 `loadRecipe("etch_step_1")`，不需要知道背後的細節。

**門面模式的好處**：
1. **降低耦合**：UI 只依賴 `OAAdmin`，不依賴 `OADataCenter` 的內部結構
2. **更容易修改**：把 `OADataCenter` 換成資料庫後端 → 只需要更新 `OAAdmin`，UI 完全不用動
3. **可測試性**：只需要 mock 門面，不需要 mock 整個資料中心

**預期會被問到的面試問題**：
- 「什麼是單例模式？它解決了什麼問題？又帶來了什麼問題？」
- 「什麼是門面模式？請舉一個真實世界的例子。」
- 「如何讓單例變成執行緒安全的？」
- 「門面模式（Facade）和轉接器模式（Adapter）有什麼差別？」

---

## 6. 錯誤處理 — 回傳代碼 vs 例外

> **半導體設備公司為什麼在乎這個**：即時嵌入式系統通常無法承受例外展開（exception unwinding）帶來的不確定時序。許多設備程式碼庫使用回傳代碼。你必須能夠討論兩種方式及其取捨。

### 模式一：整數回傳代碼（本專案用於預約操作）

為每種結果定義有意義的具名常數：

```cpp
// OAUtils.h — 所有可能的結果：
#define MEETING_ROOM_BOOK_SUCCESS   0
#define MEETING_ROOM_BOOKED         1    // 已被人預約
#define MEETING_ROOM_NOT_FOUND     -1

#define CANCEL_MEETING_ROOM_SUCCESS  0
#define CANCEL_MEETING_ROOM_FAIL     1   // 不是你預約的
#define CANCEL_MEETING_ROOM_NOT_FOUND -1
```

函式回傳代碼，呼叫端處理每一種情況：

```cpp
// 生產端（OADataCenter）：
int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) break;
    }
    if (it == rooms.end())                   return MEETING_ROOM_NOT_FOUND;
    if (it->_user != &OAUser::ERROR_USER)    return MEETING_ROOM_BOOKED;
    it->_user = &user;
    return MEETING_ROOM_BOOK_SUCCESS;
}

// 消費端（OASystemUI）：
int res = user.bookMeetingRoom(mid);
if      (res == MEETING_ROOM_BOOK_SUCCESS) { cout << "預約成功！"; }
else if (res == MEETING_ROOM_BOOKED)       { cout << "已被人預約！"; }
else if (res == MEETING_ROOM_NOT_FOUND)    { cout << "找不到這間會議室！"; }
```

### 模式二：哨兵物件（本專案用於登入驗證）

回傳參考，呼叫端檢查回傳的參考是否為哨兵：

```cpp
OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password)
            return admin;              // 成功 — 真實物件
    }
    return OAAdmin::ERROR_ADMIN;       // 失敗 — 哨兵物件
}

// 呼叫端：
OAAdmin& admin = dataCenter.loginAdmin(user, pw);
if (&admin == &OAAdmin::ERROR_ADMIN) {
    // 登入失敗 — 比較地址，不是比較值
}
```

### 模式三：例外（現代替代方案）

```cpp
OAAdmin& loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password)
            return admin;
    }
    throw std::runtime_error("Admin not found");  // 失敗時拋出例外
}

try {
    OAAdmin& admin = loginAdmin(user, pw);
    showAdminMainPage(admin);
} catch (const std::runtime_error& e) {
    cout << "登入失敗：" << e.what();
}
```

### 三種方式的取捨比較

| | 回傳代碼 | 哨兵物件 | 例外 |
|--|---------|---------|------|
| 呼叫端可以忽略？ | 可以（容易忘記） | 較難（必須處理參考） | 不行（未處理會崩潰） |
| 即時系統安全？ | 安全 | 安全 | 不安全（時序不確定） |
| 可讀性 | 較囉嗦 | 普通 | 簡潔 |
| 效能開銷 | 無 | 無 | 堆疊展開的成本 |
| 現代 C++ 風格？ | 舊式 | 特定場景 | 推薦 |

**對於即時設備軟體（LAM、AMAT）**：回傳代碼或 `std::expected`（C++23）通常比例外更受歡迎，因為例外處理的時序是不確定的。

**預期會被問到的面試問題**：
- 「什麼時候用例外，什麼時候用回傳代碼？」
- 「例外處理的成本是什麼？」→ 堆疊展開（stack unwinding）、RTTI 開銷
- 「什麼是 `std::optional` / `std::expected`？」→ 回傳代碼的現代替代方案
- 「什麼叫做一個函式是『例外安全』的？」→ 基本保證、強保證、不拋出保證

---

## 7. STL 容器與迭代器

> **半導體設備公司為什麼在乎這個**：處理晶圓檢測資料、製程步驟列表和感測器日誌，都需要高效的容器操作。STL 知識幾乎在每次面試中都會被考到。

### `vector` — 動態陣列

```cpp
#include <vector>

vector<OAAdmin>       admins;   // 管理員的動態陣列
vector<OAUser>        users;    // 使用者的動態陣列
vector<OAMeetingRoom> rooms;    // 會議室的動態陣列

// 核心操作：
v.push_back(obj);    // 加到尾端 — O(1) 均攤
v.size();            // 元素數量
v[i];                // 用索引存取 — O(1)，不檢查邊界
v.at(i);             // 用索引存取 — O(1)，超出範圍時拋出例外
v.front();           // 第一個元素
v.back();            // 最後一個元素
v.erase(it);         // 刪除迭代器指向的元素 — O(n)（需要搬移後面的元素）
v.clear();           // 刪除所有元素
v.empty();           // 如果 size == 0 則回傳 true
```

**為什麼 `getUsers()` / `getRooms()` 要回傳副本而非參考**：

```cpp
vector<OAUser> OADataCenter::getUsers() {
    return vector<OAUser>(users);  // 回傳副本，不是參考
}
```

如果回傳內部 `users` 的參考，呼叫端就可以繞過所有存取控制，直接修改資料。回傳副本是一種安全契約。（`OAMeetingRoom` 副本裡的指標值仍然指向同一個堆積物件——`showAllBookedMeetingRooms` 裡的指標比較仍然有效。）

### 迭代器（Iterator）

迭代器是容器裡的一個**位置標記**，就像書籤一樣，可以向前移動。

```cpp
vector<OAUser>::iterator it = users.begin();  // 明確型別
auto it = users.begin();                       // auto — 效果相同，更簡潔
```

- `begin()` → 指向第一個元素
- `end()` → 指向**最後一個元素之後**的位置（哨兵位置，永遠不要解參考它）
- `it++` → 前進一個位置
- `*it` → 解參考 — 取得目前位置的元素
- `it->member` → 解參考並存取成員（`(*it).member` 的簡寫）

**為什麼刪除元素要用迭代器而不是索引迴圈**：

```cpp
// 正確 — 用迭代器刪除：
bool OADataCenter::deleteUser(string username) {
    auto it = users.begin();
    for (; it != users.end(); it++) {
        if (it->username() == username) break;
    }
    if (it == users.end()) return false;
    users.erase(it);   // erase 會搬移元素，使 it 失效 — 所以必須馬上 break
    return true;
}

// 為什麼範圍式 for 不能用來刪除？
for (OAUser& u : users) {        // 範圍式 for 內部也是用迭代器
    if (u.username() == name) {
        users.erase(???);        // 沒有迭代器可以傳給 erase！
    }
}
```

### 範圍式 `for` 迴圈

```cpp
// 用於唯讀遍歷或就地修改（不做刪除）：
for (OAMeetingRoom& room : rooms) {      // & = 參考，避免複製
    room._user = &OAUser::ERROR_USER;    // 就地修改
}

// 沒有 & — 每次複製一份，修改不影響容器裡的原始元素：
for (OAMeetingRoom room : rooms) { ... }  // 通常是錯誤的做法
```

**預期會被問到的面試問題**：
- 「`push_back` 和 `erase` 的時間複雜度是什麼？」→ O(1) 均攤；O(n)
- 「呼叫 `erase` 之後迭代器會怎樣？」→ 它會失效（invalidated）
- 「什麼時候用 `vector`，什麼時候用 `list`，什麼時候用 `map`？」→ 需要隨機存取用 vector；頻繁在中間插入/刪除用 list；需要鍵值查找用 map
- 「`at()` 和 `operator[]` 的差別是什麼？」→ `at()` 會做邊界檢查

---

## 8. 友元類別

> **半導體設備公司為什麼在乎這個**：硬體驅動程式有時需要對子系統內部有特權存取，但不能對所有人都開放。`friend` 提供了受控的特權存取機制。

### `friend` 做什麼

在類別 `Y` 裡寫 `friend class X`，代表授予 `X` 完整存取 `Y` 所有私有和保護成員的權限。

```cpp
class OAMeetingRoom {
    friend class OADataCenter;   // OADataCenter 擁有「萬能鑰匙」

   private:
    OAUser* _user;               // 正常情況下外部無法存取
};

// OADataCenter 現在可以直接碰 _user：
void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;   // 直接存取私有成員 — 只有 friend 才能這樣做
    }
}
```

本專案中三個資料類別都授予 `OADataCenter` 友元權限：
- `OAAdmin` → `OADataCenter` 可以讀取 `_username` / `_password` 進行登入驗證
- `OAUser` → 同上，另外還有 `_department`
- `OAMeetingRoom` → `OADataCenter` 可以讀寫 `_mid`、`_capacity`、`_user`

### `friend` 的關鍵特性

- **不對稱**：`A 是 B 的 friend` 並**不代表** `B 是 A 的 friend`
- **不傳遞**：`A 是 B 的 friend`，`B 是 C 的 friend`，**不代表** `A 是 C 的 friend`
- **不繼承**：`friend` 的衍生類別不會自動獲得友元權限
- **謹慎使用**：過度使用 `friend` 會破壞封裝——每一個 `friend` 都是一個耦合點

**預期會被問到的面試問題**：
- 「什麼是 `friend` 類別？什麼時候會用到它？」
- 「`friend` 是否破壞了封裝？有沒有合理使用它的場景？」
- 「`friend` 關係是對稱的嗎？是可傳遞的嗎？」

---

## 9. this 指標

> **半導體設備公司為什麼在乎這個**：`this` 在回呼（callback）、事件處理器（event handler）和觀察者模式（observer pattern）中大量出現——這些在事件驅動的設備架構中極為常見。

### 什麼是 `this`

在任何非靜態的成員函式內部，`this` 是一個隱含的指標，指向「呼叫這個方法的那個物件」。

```cpp
class OAUser {
    void printInfo() {
        cout << _username;         // 隱式使用 this — 編譯器自動加上 this->
        cout << this->_username;   // 明式使用 this — 效果完全相同
    }
};
```

### 用 `this` 進行指標比較

本專案中最重要的應用：透過比較指標來找出「我的」預約。

```cpp
void OAUser::showAllBookedMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& r : rooms) {
        if (r.user() == this) {
        //  ^^^^^^^^^^   ^^^^
        //  OAUser*      OAUser*（指向「呼叫這個方法的物件」的指標）
        //  兩者都是地址 — 如果相等，代表這間房間是「我」預約的
            cout << this->_department << "'s " << this->_username
                 << " already booked room " << r.mid() << endl;
        }
    }
}
```

情境說明：`zhangsan.showAllBookedMeetingRooms()`
- `this` = `zhangsan` 的地址（例如 0x1000）
- Room 2 的 `_user = 0x1000` → `r.user() == this` → true → 顯示這間
- Room 3 的 `_user = &lisi`（0x2000）→ false → 跳過

### `*this` — 把自己傳給另一個函式

`OADataCenter::bookMeetingRoom` 需要的是 `OAUser&`（使用者的參考）。用 `*this` 解參考指標，得到物件本身：

```cpp
int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
    //                                                  ^^^^^
    //  this   = OAUser*（指標）
    //  *this  = OAUser （物件本身 — 可以綁定到 OAUser& 參數）
}

// OADataCenter::bookMeetingRoom 的函式簽名：
int bookMeetingRoom(OAUser& user, int mid);
//                  ^^^^^^^^ 需要的是參考，不是指標
```

在 `bookMeetingRoom` 內部，`OADataCenter` 接著儲存 `&user`——它的地址和原始的 `this` 相同：

```cpp
it->_user = &user;   // 預約者的地址，之後和 this 比較時可以對應上
```

**預期會被問到的面試問題**：
- 「`this` 的型別是什麼？」→ `ClassName* const`（指向物件的指標，指標本身是 const）
- 「`this` 可以是 null 嗎？」→ 不行——只要你在成員函式內部，`this` 永遠有效
- 「`*this` 是什麼意思？什麼時候會用到它？」→ 解參考自身指標，在需要傳入物件參考或值的地方使用
- 「`return *this` 用在哪裡？」→ 方法鏈（fluent interface / method chaining）

---

## 10. 前向宣告與標頭檔組織

> **半導體設備公司為什麼在乎這個**：大型設備程式碼庫有數百個類別。循環依賴和過多的 `#include` 會導致編譯時間暴增和高度耦合。知道何時用前向宣告、何時用完整引入，是真實的程式碼品質技能。

### 問題：循環依賴

```
OADataCenter.h 引入了 OAAdmin.h（因為需要 OAAdmin 型別）
OAAdmin.h 又引入了 OADataCenter.h（因為需要 OADataCenter 型別）
→ 無限的引入循環！
```

### 解決方案：前向宣告

告訴編譯器「這個型別存在——我之後再給你完整的定義」：

```cpp
// OADataCenter.h
#pragma once
#include "OAAdmin.h"    // 完整引入 — OADataCenter 儲存 vector<OAAdmin>
#include "OAUser.h"
#include "OAMeetingRoom.h"

class OAAdmin;          // 前向宣告 — 在這裡是多餘的，但有文件說明的效果
class OAUser;
class OAMeetingRoom;

class OADataCenter { ... };
```

```cpp
// OAMeetingRoom.h
#pragma once
#include "OAUser.h"     // 需要完整引入，因為 OAUser* 需要知道 OAUser 存在

class OAUser;           // 為指標成員做前向宣告

class OAMeetingRoom {
   private:
    OAUser* _user;      // 指標 — 大小固定（8 bytes），不需要完整定義
};
```

### 規則：什麼時候可以用前向宣告，什麼時候需要完整引入？

| 使用方式 | 需要完整引入？ |
|---------|-------------|
| 指向該型別的指標（`T*`） | 不需要 — 前向宣告即可 |
| 對該型別的參考（`T&`） | 不需要 — 前向宣告即可 |
| 以值儲存（`T obj`） | 需要 — 編譯器要知道大小 |
| 呼叫該型別的方法 | 需要 — 編譯器要知道定義 |
| 繼承自該型別 | 需要 |

這也是 `OAMeetingRoom` 儲存 `OAUser*`（指標）而非 `OAUser`（值）的原因之一——指標的大小永遠是 8 bytes，不管它指向什麼型別，所以前向宣告就足夠了。

**預期會被問到的面試問題**：
- 「什麼是前向宣告？什麼時候需要用它？」
- 「為什麼儲存指標可以用前向宣告，但儲存值不行？」
- 「`#include "file.h"` 和 `#include <file>` 有什麼差別？」

---

## 11. 常見面試問題與標準回答

以下問題全部來自本專案所展示的概念，可以直接以專案程式碼作為佐證。

### 記憶體與指標

**1. 「指標和參考有什麼差別？」**
   - 指標可以為 null，可以重新指向其他物件；參考不行
   - 指標用 `->` 存取成員；參考用 `.`
   - 專案範例：`OAMeetingRoom::_user` 是指標（可以指向 ERROR_USER 或真實使用者，而且可以改變）；`loginAdmin` 回傳參考（是現有管理員物件的別名）

**2. 「什麼是懸空指標（dangling pointer）？如何避免？」**
   - 懸空指標指向已被釋放或已超出作用域的記憶體
   - 專案範例：如果 Room 4 的設定用了堆疊區域變數而非 `new`，建構函式結束後 `_user` 就會成為懸空指標
   - 預防方式：智慧指標（`unique_ptr`、`shared_ptr`）、謹慎管理物件生命週期、RAII

**3. 「`new` 回傳什麼？如果從不呼叫 `delete` 會怎樣？」**
   - `new` 回傳一個指向新分配之堆積記憶體的指標
   - 忘記 `delete` 會造成記憶體洩漏——那塊記憶體直到行程結束前都不會回到作業系統
   - 對於長時間執行的設備行程，洩漏會持續累積，最終導致記憶體不足崩潰

### OOP

**4. 「請用一個具體的例子說明什麼是封裝。」**
   - 用 `OAMeetingRoom::_user` 指標：它是 `private` 的，所以只有 `OADataCenter`（透過 `friend`）才能修改它。外部程式碼必須透過 `bookMeetingRoom()`，這個方法會先驗證房間是否存在、是否已被預約，才會設定 `_user`。如果沒有 `private`，任何程式碼都能直接破壞預約狀態。

**5. 「`public`、`private`、`protected` 有什麼差別？」**
   - `public`：任何地方都可以存取
   - `private`：只有類別本身（以及 `friend` 類別）可以存取
   - `protected`：類別本身以及衍生類別可以存取（用於繼承架構）

**6. 「什麼是複製建構函式（copy constructor）？它什麼時候被呼叫？」**
   - 複製建構函式用另一個現有物件作為來源，建立一個新物件
   - 呼叫時機：傳值、回傳值、用另一個物件初始化
   - 專案範例：`vector<OAUser> users_back(users)` 呼叫了 vector 的複製建構函式

### 設計模式

**7. 「描述單例模式。它有哪些缺點？」**
   - 確保只存在一個實例，透過 `getInstance()` 提供全域存取
   - 缺點：引入全域狀態（難以測試）、預設不是執行緒安全、隱藏依賴關係
   - 執行緒安全修正：Meyers' Singleton（靜態區域變數 — C++11 保證初始化只發生一次）

**8. 「什麼是門面模式？它和代理模式（Proxy）有什麼差別？」**
   - 門面：為複雜的子系統提供一個統一的簡化介面；客戶端不需要知道子系統的存在
   - 代理：控制對單一物件的存取（介面相同）；門面則包裝了多個物件
   - 專案範例：`OAAdmin` 是門面——它把 `OADataCenter` 複雜的 vector 操作隱藏在 `addUser()`、`deleteUser()` 等方法後面

### 錯誤處理

**9. 「什麼時候應該用回傳代碼而不是例外？」**
   - 即時系統（異常處理的時序不確定）
   - 需要與 C 相容的介面
   - 可預期的、可恢復的失敗路徑（例如「找不到使用者」是預期情況，不是例外情況）
   - 真正意外的失敗（硬體故障、記憶體不足）才用例外

### STL

**10. 「為什麼 `vector::erase()` 會使迭代器失效？這在實務上意味著什麼？」**
    - `erase` 刪除一個元素後，會把後面所有的元素向前移動一格——舊的迭代器位置不再指向相同的元素
    - 實務上：在迴圈中 `erase` 後必須立即 `break`，或者使用 `erase` 的回傳值（它會回傳指向下一個有效元素的迭代器）

**11. 「`vector::at()` 和 `vector::operator[]` 有什麼差別？」**
    - `at()` 會做邊界檢查，若索引超出範圍會拋出 `std::out_of_range`
    - `operator[]` 不做邊界檢查——超出範圍是未定義行為（undefined behavior），但速度較快

### 靜態成員

**12. 「靜態成員函式可以呼叫非靜態成員函式嗎？」**
    - 不行——靜態函式沒有 `this` 指標，因此無法存取實例成員
    - 它們只能存取其他靜態成員，或是建立區域物件

---

## 12. OOP 進階 — 繼承、多態、虛函數

> **半導體設備公司為什麼在乎這個**：設備軟體通常有深層的繼承體系，例如「感測器基底類別 → 光學感測器 → 特定型號感測器」。正確理解虛函數和多態，是確保正確的行為在執行期被呼叫的基礎。

---

### 面對物件的三大特徵

**封裝（Encapsulation）**：隱藏內部資料，只對外暴露安全的介面。（詳見第 2 節）

**繼承（Inheritance）**：子類別繼承父類別的屬性與行為，並可以加以擴充。

```cpp
class Sensor {              // 父類別（基底類別）
public:
    string _name;
    virtual float read() = 0;  // 純虛函數
};

class TemperatureSensor : public Sensor {  // 子類別（衍生類別）
public:
    float read() override { return getTemp(); }
};
```

**多態（Polymorphism）**：同一個介面，在執行期根據物件的實際型別呼叫不同的實作。

```cpp
Sensor* s = new TemperatureSensor();
s->read();  // 執行期決定呼叫 TemperatureSensor::read()，而不是 Sensor::read()
```

---

### 多態的實現原理 — 虛函數表（vtable）與虛函數表指針（vptr）

這是面試最常被問到的底層機制。

**虛函數表（vtable）**：每個含有虛函數的類別，編譯器會自動產生一張「函數指針陣列」，記錄所有虛函數的實際地址。

**虛函數表指針（vptr）**：每個物件內部有一個隱含的指針（`vptr`），指向它所屬類別的 vtable。

```
記憶體佈局：

TemperatureSensor 物件：
┌────────────────┐
│ vptr ──────────┼──→ TemperatureSensor::vtable
│ _name          │     [0] TemperatureSensor::read()
│ ...            │
└────────────────┘

Sensor* s = new TemperatureSensor();
s->read();
// 1. 透過 s 找到物件
// 2. 透過 vptr 找到 TemperatureSensor 的 vtable
// 3. 在 vtable[0] 找到 TemperatureSensor::read 的地址
// 4. 呼叫它 → 多態！
```

**vptr 和 vtable 的建立時機**：
- **vtable**：在**編譯期**由編譯器靜態建立，每個類別只有一份。
- **vptr**：在物件**建構時**（建構函式執行的第一步）由編譯器自動設定，每個物件各有一個。

---

### 虛析構函數（Virtual Destructor）

**為什麼需要？** 透過基底類別指針刪除衍生類別物件時，如果析構函式不是虛函數，只會呼叫基底類別的析構函式，衍生類別的資源不會被釋放，造成洩漏。

```cpp
class Sensor {
public:
    ~Sensor() { cout << "Sensor 析構\n"; }  // 非虛函數 — 危險！
};

class Camera : public Sensor {
    char* _buffer;
public:
    Camera() { _buffer = new char[1024]; }
    ~Camera() { delete[] _buffer; }        // 不會被呼叫！
};

Sensor* s = new Camera();
delete s;  // 只呼叫 Sensor::~Sensor()，_buffer 洩漏！

// 修正方式：把基底類別的析構函式設為 virtual
class Sensor {
public:
    virtual ~Sensor() { cout << "Sensor 析構\n"; }  // 安全！
};
// 現在 delete s 會先呼叫 Camera::~Camera()，再呼叫 Sensor::~Sensor()
```

**規則**：只要類別有虛函數（打算被繼承），析構函式就應該是 `virtual`。

---

### `override` 與 `final` 關鍵字

**`override`**：告訴編譯器「這個函式是要覆寫父類別的虛函數」。如果父類別沒有對應的虛函數，編譯器會報錯——幫你抓住拼錯名字或簽名不對的錯誤。

```cpp
class Sensor {
    virtual float read(int channel);
};

class Camera : public Sensor {
    float read() override;  // 編譯錯誤！父類別是 read(int)，簽名不符
    float read(int channel) override;  // 正確
};
```

**`final`**：
- 用在**類別**上：禁止其他類別繼承它。
- 用在**虛函數**上：禁止衍生類別再次覆寫。

```cpp
class HardwareController final { ... };  // 禁止繼承

class Sensor {
    virtual float read() final;  // 衍生類別不能再 override 這個函式
};
```

---

### 繼承下的建構函式與析構函式呼叫順序

**建構順序**：從最頂層的父類別開始，依序往下呼叫到子類別。
**析構順序**：與建構順序**相反**，從子類別開始，依序往上呼叫到父類別。

```cpp
class A {
public:
    A()  { cout << "A 建構\n"; }
    ~A() { cout << "A 析構\n"; }
};

class B : public A {
public:
    B()  { cout << "B 建構\n"; }
    ~B() { cout << "B 析構\n"; }
};

class C : public B {
public:
    C()  { cout << "C 建構\n"; }
    ~C() { cout << "C 析構\n"; }
};

C obj;
// 輸出：
// A 建構   ← 最上層的父類別先建構
// B 建構
// C 建構   ← 子類別最後建構
// C 析構   ← 子類別先析構
// B 析構
// A 析構   ← 最上層的父類別最後析構
```

**記憶口訣**：建構是「由上而下」，析構是「由下而上」。

---

### 菱形繼承與虛繼承（Virtual Inheritance）

**問題**：當兩個類別繼承自同一個父類別，而第三個類別又同時繼承這兩個類別時，會產生「菱形繼承」問題——最頂層的祖先類別會被包含兩份。

```
      Animal
     /      \
   Dog      Cat
     \      /
     DogCat（存在兩份 Animal 的資料！）
```

```cpp
class Animal {
public:
    int _age;
};

class Dog : public Animal { };
class Cat : public Animal { };

class DogCat : public Dog, public Cat { };

DogCat dc;
dc._age = 5;        // 編譯錯誤！不知道是 Dog 裡的 _age 還是 Cat 裡的 _age
dc.Dog::_age = 5;   // 必須這樣指定，很麻煩
```

**解決方案：虛繼承（`virtual`）**

```cpp
class Dog : virtual public Animal { };  // 加上 virtual
class Cat : virtual public Animal { };

class DogCat : public Dog, public Cat { };

DogCat dc;
dc._age = 5;  // 現在只有一份 Animal，沒有歧義
```

**虛繼承的機制**：使用虛基底類別指針（Virtual Base Pointer）確保共同的祖先類別只有一份實例，由 `DogCat` 直接負責初始化它。

---

### 預設拷貝建構函式的生成時機

C++ 在以下情況會**自動生成**預設拷貝建構函式：
- 沒有使用者自定義的拷貝建構函式
- 所有成員變數都是可拷貝的（非 `= delete`）
- 沒有定義移動建構函式或移動賦值運算子

自動生成的版本執行的是**淺拷貝（shallow copy）**——逐個成員複製，包括直接複製指針的值（地址），而不是複製指針指向的內容。

```cpp
class MyClass {
    int* _data;
public:
    MyClass(int val) { _data = new int(val); }
    // 沒有定義拷貝建構函式 → 自動生成淺拷貝版本
};

MyClass a(10);
MyClass b = a;  // 淺拷貝：b._data 和 a._data 指向同一塊記憶體！
// 當 a 和 b 都被析構時，同一塊記憶體被 delete 兩次 → 崩潰！
```

**解決方式**：當類別擁有指針成員時，必須自行實作拷貝建構函式（深拷貝），或使用智慧指針。

**預期會被問到的面試問題**：
- 「怎麼解決 C++ 菱形繼承？」→ 使用虛繼承（`virtual public`）
- 「請簡述多態實現的原理」→ vtable + vptr，執行期動態派發
- 「`override`、`final` 的作用是什麼？」→ override：防止拼錯；final：禁止覆寫或繼承
- 「繼承下的建構/析構呼叫順序是什麼？」→ 建構由上而下，析構由下而上
- 「虛函數表和 vptr 是何時建立的？」→ vtable 在編譯期建立；vptr 在物件建構時設定
- 「為什麼需要虛析構函式？」→ 透過基底指針 delete 時確保衍生類別的資源被正確釋放
- 「OOP 三大特徵是什麼？」→ 封裝、繼承、多態

---

## 13. 類型推導與函數式工具

> **半導體設備公司為什麼在乎這個**：現代 C++ 程式碼大量使用 `auto`、lambda 和 `std::function` 來寫簡潔的回呼函式、事件處理器和演算法管線。這些是 C++11 後最常用的語言特性之一。

---

### 類型推導 — `auto` 與 `decltype`

**`auto`**：讓編譯器自動推導變數的型別，根據初始值決定。

```cpp
auto x = 42;              // int
auto d = 3.14;            // double
auto s = string("hello"); // string
auto it = v.begin();      // vector<int>::iterator（省去寫長型別名稱的麻煩）

// 搭配範圍式 for：
for (auto& room : rooms) { ... }  // 自動推導為 OAMeetingRoom&
```

**`decltype`**：推導一個「表達式的型別」，但不求值。常用於模板和泛型程式設計中。

```cpp
int x = 10;
decltype(x) y = 20;   // y 的型別是 int（和 x 相同）

// 實際用途 — 在不知道回傳型別時：
auto add(int a, int b) -> decltype(a + b) {
    return a + b;
}
```

**面試重點**：`auto` 推導時會**丟棄** `const` 和參考性質。

```cpp
const int& ref = 10;
auto a = ref;    // a 的型別是 int，不是 const int&
auto& b = ref;  // b 的型別是 const int&（加上 & 才保留）
```

---

### `lambda`、`std::function`、`std::bind`

**Lambda（匿名函式）**：在需要函式的地方直接定義，不需要另外命名。

```cpp
// 語法：[捕獲列表](參數) -> 回傳型別 { 函式本體 }
auto greet = [](string name) { cout << "Hello, " << name; };
greet("Alice");  // 輸出：Hello, Alice

// 捕獲外部變數：
int threshold = 10;
auto isLarge = [threshold](int val) { return val > threshold; };  // 捕獲 threshold（值）
auto isLarge2 = [&threshold](int val) { return val > threshold; }; // 捕獲（參考）

// 常見應用 — 搭配 STL 演算法：
vector<int> v = {3, 1, 4, 1, 5};
sort(v.begin(), v.end(), [](int a, int b) { return a > b; });  // 降序排列
```

**`std::function`**：一個可以儲存任何「可呼叫物件」的通用容器，包括函式指針、lambda、函式物件。

```cpp
#include <functional>

// 儲存普通函式：
std::function<int(int, int)> add = [](int a, int b) { return a + b; };

// 當成參數傳遞（依賴注入、策略模式）：
void processData(vector<int>& data, std::function<bool(int)> filter) {
    data.erase(remove_if(data.begin(), data.end(), filter), data.end());
}

processData(v, [](int x) { return x < 5; });  // 移除所有小於 5 的元素
```

**`std::bind`**：「綁定」函式的部分參數，產生一個新的可呼叫物件。（在 C++11 後大多可以用 lambda 取代）

```cpp
#include <functional>

int multiply(int a, int b) { return a * b; }

// 把 multiply 的第一個參數固定為 3，建立一個「乘以 3」的函式：
auto tripleIt = std::bind(multiply, 3, std::placeholders::_1);
tripleIt(5);  // 回傳 15

// 等效的 lambda（更現代的寫法）：
auto tripleIt2 = [](int x) { return multiply(3, x); };
```

**三者的關係總結**：

| | 用途 | 備註 |
|--|------|------|
| `lambda` | 就地定義匿名函式 | C++11 最推薦的方式 |
| `std::function` | 儲存並傳遞任意可呼叫物件 | 有一定的執行期開銷 |
| `std::bind` | 預先綁定函式參數 | C++11 後多數場景被 lambda 取代 |

**預期會被問到的面試問題**：
- 「`auto` 的型別推導規則是什麼？有什麼陷阱？」→ 丟棄 const/reference；加上 `&` 才保留
- 「lambda 的捕獲列表有哪些形式？」→ `[=]` 值捕獲所有；`[&]` 引用捕獲所有；`[x]` 值捕獲 x；`[&x]` 引用捕獲 x
- 「`std::function` 和函式指針有什麼差別？」→ `std::function` 可以儲存 lambda 和函式物件；有額外的型別抹除開銷
- 「什麼時候用 `std::bind`？」→ 現代 C++ 幾乎都用 lambda 取代

---

## 14. 進階記憶體管理

> **半導體設備公司為什麼在乎這個**：設備軟體長時間無中斷執行（機台 24/7 運作），任何記憶體相關問題都可能在幾天後才爆發。深刻理解記憶體分配機制是系統軟體工程師的基本素養。

---

### `malloc` 如何分配記憶體？`free` 怎麼知道要釋放多少？

**`malloc` 的原理**：
1. 呼叫作業系統（`brk` / `mmap` 系統呼叫）從核心取得一大塊記憶體。
2. 在自己管理的**記憶體池（heap pool）**中尋找一塊夠大的空閒區塊。
3. 把那塊區塊標記為「已使用」，回傳起始地址給呼叫端。

**`free` 怎麼知道要釋放多少？**：
`malloc` 在分配時，會在回傳的指針**之前**悄悄多放一個「區塊頭（block header）」，記錄這塊記憶體的大小和狀態。`free` 接到指針後，往前偏移幾個位元組就能找到這個 header，得知要釋放的大小。

```
malloc(100) 回傳的指針：
  ↓
[block header: size=100, used=1][← 100 bytes 的使用者資料 →]
                                 ↑
                         malloc 回傳這個地址

free(ptr) 時：
ptr - sizeof(header) → 找到 block header → 知道 size=100 → 釋放 104 bytes
```

---

### `malloc`/`free` vs `new`/`delete`

| | `malloc` / `free` | `new` / `delete` |
|--|------------------|--------------------|
| 語言 | C 語言函式 | C++ 運算子 |
| 呼叫建構/析構函式 | **不呼叫** | **自動呼叫** |
| 回傳型別 | `void*`（需要強制轉型） | 正確型別的指針，無需轉型 |
| 失敗處理 | 回傳 `nullptr` | 拋出 `std::bad_alloc` 例外 |
| 大小計算 | 需要手動傳入 `sizeof` | 自動計算 |
| 對應關係 | `malloc`/`free` | `new`/`delete`；`new[]`/`delete[]` |

```cpp
// malloc — 不呼叫建構函式，物件處於未初始化狀態！
MyClass* p = (MyClass*)malloc(sizeof(MyClass));  // 危險：建構函式未被呼叫
free(p);  // 析構函式未被呼叫

// new — 正確方式
MyClass* p = new MyClass(42);   // 自動呼叫建構函式
delete p;                        // 自動呼叫析構函式
```

**重要**：`new` 和 `delete` 必須配對；`new[]` 和 `delete[]` 必須配對。混用是未定義行為。

---

### 寫時複製（Copy-on-Write，CoW）

**原理**：多個物件共用同一份資料副本，只有當其中一個試圖**修改**資料時，才真正做複製，並讓它持有自己獨立的一份。

**目的**：節省不必要的記製開銷——如果根本沒有寫入，就不需要複製。

```
初始狀態：
  string a = "hello";
  string b = a;        // CoW：a 和 b 共用同一塊 "hello" 記憶體

  a ─→ [ref=2] "hello"
  b ─╯

修改 b 時：
  b += " world";       // 觸發複製！b 獲得自己的副本

  a ─→ [ref=1] "hello"
  b ─→ [ref=1] "hello world"
```

**應用場景**：
- C++98 的 `std::string`（C++11 後因多執行緒問題改為深拷貝）
- Linux 的 `fork()` 系統呼叫：子行程和父行程共享頁面，當任一方寫入才複製那一頁
- 現代容器（`Redis` 的 RDB 快照也使用 fork + CoW）

---

### 頁面置換算法

當實體記憶體不足，作業系統需要把部分頁面換出到磁碟時，用什麼策略選擇要換出哪一頁？

**常見算法**：

| 算法 | 策略 | 優缺點 |
|------|------|-------|
| **FIFO**（先進先出） | 換出最早載入的頁面 | 簡單，但可能換出常用頁面 |
| **LRU**（最近最少使用） | 換出最長時間沒被存取的頁面 | 效能好，但實作成本高 |
| **LFU**（最不常使用） | 換出存取次數最少的頁面 | 適合長期執行的工作負載 |
| **OPT**（最佳算法） | 換出未來最長時間不會用到的頁面 | 理論最佳，實際無法實作（無法預知未來） |
| **Clock（時鐘算法）** | LRU 的近似實作，用一個 reference bit 循環檢查 | 效能與 LRU 接近，實作簡單 |

**Belady 異常**：FIFO 算法中，增加記憶體框數反而導致缺頁次數增加的異常現象（LRU 和 OPT 不會發生）。

**預期會被問到的面試問題**：
- 「`malloc` 是如何分配記憶體的？`free` 怎麼知道釋放多少？」→ 區塊頭（block header）記錄大小
- 「`malloc`/`free` 和 `new`/`delete` 的差別？」→ `new`/`delete` 會呼叫建構/析構函式
- 「寫時複製（CoW）的原理和應用是什麼？」→ 共享資料到有人寫入才複製；fork()、舊版 string
- 「常見的頁面置換算法有哪些？」→ FIFO、LRU、LFU、OPT、Clock

---

## 15. 智慧指標與現代 C++11

> **半導體設備公司為什麼在乎這個**：在 24/7 運行的設備中，手動 `delete` 很容易因例外或複雜的控制流而洩漏。C++11 的智慧指針把資源管理的責任從工程師轉移給型別系統，大幅降低設備軟體中記憶體相關的事故率。

---

### 智慧指針種類與使用場景

**`std::unique_ptr`**（唯一擁有者）

```cpp
#include <memory>

// 建立 — 使用 make_unique（推薦）
auto sensor = std::make_unique<TemperatureSensor>(42);

// 轉移擁有權（不能複製，只能移動）：
auto sensor2 = std::move(sensor);  // sensor 現在為空

// 離開作用域時自動 delete，不需要手動釋放
```

**使用時機**：資源只有一個擁有者，如工廠函式回傳、成員變數等。

---

**`std::shared_ptr`**（共享擁有，引用計數）

```cpp
auto data = std::make_shared<SensorData>();   // 引用計數 = 1
{
    auto data2 = data;   // 引用計數 = 2（複製 shared_ptr 會增加計數）
}                        // data2 離開作用域，引用計數 = 1
// data 離開作用域，引用計數 = 0 → 自動 delete
```

**使用時機**：多個物件需要共同持有同一個資源（例如多個感測器共用一個設定物件）。

**開銷**：每個 `shared_ptr` 維護一個控制區塊（control block）做引用計數，有額外的記憶體和執行緒同步開銷。

---

**`std::weak_ptr`**（觀察者，不影響生命週期）

```cpp
std::shared_ptr<SensorData> shared = std::make_shared<SensorData>();
std::weak_ptr<SensorData> weak = shared;  // 不增加引用計數

// 使用時必須先 lock，確認物件還存在：
if (auto locked = weak.lock()) {
    locked->process();  // 安全使用
} else {
    // 物件已被釋放
}
```

**使用時機**：打破 `shared_ptr` 的循環引用（若 A 持有 B，B 持有 A，引用計數永遠不為 0）；觀察者模式中的觀察者。

---

### 三種智慧指針比較

| | `unique_ptr` | `shared_ptr` | `weak_ptr` |
|--|-------------|-------------|------------|
| 擁有者 | 唯一 | 多個 | 無（只觀察） |
| 可複製 | 否 | 是 | 是 |
| 引用計數 | 無 | 有 | 有（不影響） |
| 開銷 | 幾乎零 | 有（控制區塊） | 有（控制區塊） |
| 使用時機 | 預設選擇 | 共享所有權 | 打破循環引用 |

---

### C++11 常用新特性

```cpp
// 1. auto — 型別推導（見第 13 節）

// 2. Range-based for loop — 範圍式 for
for (auto& user : users) { ... }

// 3. Lambda — 匿名函式（見第 13 節）
auto fn = [](int x) { return x * 2; };

// 4. Smart pointers — 智慧指針（見本節上方）
auto p = std::make_unique<MyClass>();

// 5. nullptr — 取代 NULL
int* p = nullptr;  // 型別安全；NULL 只是 0，可能與整數混淆

// 6. Move semantics — 移動語意（避免不必要的深拷貝）
vector<string> getNames() { return names; }  // 回傳時自動 move，不是 copy

// 7. Initializer list — 初始化列表
vector<int> v = {1, 2, 3, 4, 5};

// 8. std::thread — 標準執行緒庫
#include <thread>
std::thread t([]{ doWork(); });
t.join();

// 9. constexpr — 編譯期常數
constexpr int MAX_ROOMS = 100;

// 10. static_assert — 編譯期斷言
static_assert(sizeof(int) == 4, "int 必須是 4 bytes");
```

**預期會被問到的面試問題**：
- 「`unique_ptr` 和 `shared_ptr` 的差別？各在什麼情況下使用？」
- 「`weak_ptr` 解決了什麼問題？」→ 打破 `shared_ptr` 的循環引用
- 「C++11 有哪些重要的新特性？」→ auto、lambda、move、智慧指針、nullptr、range-for
- 「`nullptr` 和 `NULL` 有什麼差別？」→ `nullptr` 是型別安全的，不會被推導為整數

---

## 16. 動態庫與靜態庫

> **半導體設備公司為什麼在乎這個**：設備控制軟體通常由多個元件組成，分別以動態庫或靜態庫的形式提供（例如相機 SDK、馬達控制庫、資料採集 API）。理解兩者的差異影響部署、更新和除錯策略。

---

### 靜態庫（Static Library）

**定義**：在**編譯連結階段**，把函式庫的程式碼直接複製進最終的執行檔中。

- Linux 副檔名：`.a`（archive）
- Windows 副檔名：`.lib`

```bash
# 編譯生成靜態庫：
g++ -c mylib.cpp -o mylib.o
ar rcs libmylib.a mylib.o

# 連結靜態庫：
g++ main.cpp -L. -lmylib -o app
# 結果：app 包含 mylib 的所有程式碼，不需要額外的 .a 檔就能執行
```

---

### 動態庫（Dynamic Library / Shared Library）

**定義**：函式庫的程式碼**不嵌入**執行檔，只留下一個「引用」。在**執行期**才由作業系統載入到記憶體。

- Linux 副檔名：`.so`（shared object）
- Windows 副檔名：`.dll`（dynamic-link library）

```bash
# 編譯生成動態庫：
g++ -shared -fPIC mylib.cpp -o libmylib.so

# 連結動態庫：
g++ main.cpp -L. -lmylib -o app
# 執行時必須確保 .so 檔在 LD_LIBRARY_PATH 中或標準路徑下
```

---

### 靜態庫 vs 動態庫比較

| | 靜態庫 | 動態庫 |
|--|--------|--------|
| 程式碼嵌入執行檔 | 是 | 否 |
| 執行時需要庫檔 | 不需要 | 需要 `.so` / `.dll` |
| 執行檔大小 | 較大 | 較小 |
| 記憶體使用 | 各行程一份 | 多行程共用同一份 |
| 更新庫 | 需要重新編譯應用程式 | 只需替換 `.so` 檔，不需重新編譯 |
| 執行速度 | 較快（無動態連結開銷） | 略慢（動態連結有一次性開銷） |
| 適合情境 | 嵌入式、獨立部署、安全要求高 | 大型系統、需要熱更新（hot patch） |

**半導體設備業的實際應用**：
- **靜態庫**：嵌入式控制器韌體、確定性實時系統（避免執行期的任何不確定性）
- **動態庫**：相機 SDK、視覺處理庫（多個工具可以共用同一份影像處理庫；更新 SDK 不需要重新編譯控制軟體）

**預期會被問到的面試問題**：
- 「靜態庫和動態庫的差別是什麼？」→ 連結時機（編譯期 vs 執行期）、大小、更新方式
- 「什麼情況下選靜態庫？什麼情況下選動態庫？」
- 「動態庫的符號是怎麼被解析的？」→ 執行期動態連結器（`ld.so`）根據 `LD_LIBRARY_PATH` 搜尋

---

## 17. 並發與執行緒

> **半導體設備公司為什麼在乎這個**：現代設備系統同時處理感測器資料採集、UI、製程控制和日誌記錄等任務。多執行緒是標配，理解進程/執行緒的差異和同步機制是系統軟體工程師的核心能力。

---

### 進程（Process）與執行緒（Thread）的差別

**進程（Process）**：作業系統分配資源的基本單位。每個進程有獨立的虛擬地址空間、程式碼、資料、堆積、堆疊和作業系統資源（檔案描述符、訊號等）。

**執行緒（Thread）**：CPU 排程的基本單位。同一個進程內的多個執行緒**共享**進程的地址空間和資源，但各自有獨立的堆疊和暫存器狀態。

```
進程 A                     進程 B
┌────────────────────┐    ┌────────────────────┐
│ 虛擬地址空間       │    │ 虛擬地址空間       │
│ ┌──────────────┐  │    │ ┌──────────────┐  │
│ │ 程式碼       │  │    │ │ 程式碼       │  │
│ │ 全域資料     │  │    │ │ 全域資料     │  │
│ │ Heap（共享） │  │    │ │ Heap         │  │
│ │ Thread 1 堆疊│  │    │ │ Thread 1 堆疊│  │
│ │ Thread 2 堆疊│  │    │ └──────────────┘  │
│ └──────────────┘  │    └────────────────────┘
└────────────────────┘
  兩個執行緒共用相同的 Heap
```

| | 進程 | 執行緒 |
|--|------|--------|
| 地址空間 | 各自獨立 | 共享同一進程的 |
| 建立開銷 | 大（需要複製資源） | 小 |
| 通訊方式 | IPC（管道、訊息佇列、共享記憶體等） | 直接讀寫共享記憶體（但需加鎖） |
| 崩潰影響 | 不影響其他進程 | 一個執行緒崩潰可能導致整個進程崩潰 |
| 上下文切換成本 | 高 | 低 |

---

### 進程與執行緒的上下文切換差異

**上下文切換（Context Switch）**：CPU 從執行一個任務切換到另一個任務，需要保存目前任務的狀態（暫存器、程式計數器等），並載入下一個任務的狀態。

**執行緒切換**（同一進程內）：
1. 保存目前執行緒的暫存器狀態和堆疊指針
2. 載入下一個執行緒的暫存器狀態和堆疊指針
3. **不需要**切換虛擬地址空間（頁表不變）→ **TLB 不需要清空**

**進程切換**（不同進程間）：
1. 保存目前進程的暫存器和核心狀態
2. 切換虛擬地址空間（切換頁表）→ **TLB 必須清空**（因為地址映射完全不同）
3. 載入下一個進程的頁表和狀態
4. TLB 清空導致後續的記憶體存取大量 cache miss

**結論**：進程切換比執行緒切換貴得多，主要成本在於頁表切換和 TLB 失效。

---

### 系統調用能否引起執行緒切換？

**可以**，有幾種情況：

1. **阻塞型系統調用**：執行緒呼叫 `read()`、`write()`、`sleep()` 等阻塞操作時，核心會把它標記為「等待中」，讓 CPU 去執行其他執行緒——這是最常見的情況。

2. **時間片耗盡**：系統調用本身執行時間過長，核心排程器在返回使用者空間前強制切換到另一個執行緒。

3. **優先級搶占**：系統調用完成後，核心發現有更高優先級的執行緒準備好了，會在返回之前先切換到高優先執行緒。

**不一定引起切換的情況**：快速完成的非阻塞系統調用（如 `getpid()`）通常不會引起執行緒切換。

---

### Reactor 與 Proactor 模式

這兩個是 I/O 多路復用的設計模式，在高效能網路設備伺服器和資料採集系統中很常見。

**Reactor（反應器）模式**：
- 主動**等待**事件，I/O 就緒後通知應用程式，由**應用程式**自己去讀寫資料
- 典型實作：`select()`、`poll()`、`epoll()`（Linux）

```
事件循環：
  1. epoll_wait() 等待 I/O 事件
  2. 某個 socket 可讀 → 通知對應的 handler
  3. Handler 自己呼叫 read() 讀資料
  4. 處理資料
  5. 回到步驟 1
```

**Proactor（前攝器）模式**：
- 發起**非同步** I/O 操作，等 I/O **完成後**由作業系統通知應用程式，此時資料已在緩衝區中，應用程式直接處理即可
- 典型實作：Windows IOCP（I/O Completion Port）、Linux `io_uring`

| | Reactor | Proactor |
|--|---------|----------|
| 誰做 I/O 操作 | 應用程式 | 作業系統 |
| 觸發時機 | I/O **就緒**（可讀/可寫）時通知 | I/O **完成**後通知 |
| 資料是否已在緩衝區 | 否（需要自己 read） | 是 |
| 典型實作 | epoll（Linux） | IOCP（Windows）、io_uring |
| 程式複雜度 | 相對簡單 | 相對複雜 |

**半導體設備應用**：高速影像感測器資料串流系統（KLA 晶圓檢測）通常使用 Proactor 模式，讓作業系統負責 DMA 傳輸，完成後通知應用程式處理影像。

**預期會被問到的面試問題**：
- 「進程和執行緒的差別是什麼？」→ 地址空間隔離；執行緒共享；切換成本不同
- 「進程和執行緒的上下文切換有什麼差別？」→ 進程切換需要切換頁表和清空 TLB，成本更高
- 「系統調用會引起執行緒切換嗎？」→ 阻塞型系統調用會；快速非阻塞的不一定
- 「Reactor 和 Proactor 的差別是什麼？」→ Reactor：I/O 就緒通知，應用程式自己讀；Proactor：I/O 完成通知，資料已在緩衝區

---

## 18. 網路與系統

> **半導體設備公司為什麼在乎這個**：現代設備系統通常有網路介面（遠端監控、配方下載、資料上傳到 MES 系統）。理解 TCP/IP 和系統調用是設計可靠通訊元件的基礎。

---

### TCP 與 UDP 的差別

| | TCP | UDP |
|--|-----|-----|
| 連線建立 | 需要（三次握手） | 不需要 |
| 可靠性 | 確保資料順序到達，不遺漏 | 不保證，可能亂序或遺漏 |
| 流量控制 | 有（滑動視窗） | 無 |
| 速度 | 較慢（確認機制開銷） | 較快 |
| 適合場景 | 檔案傳輸、製程配方下載、控制命令 | 串流監控影像、即時感測器廣播 |
| 標頭大小 | 20 bytes（較大） | 8 bytes（較小） |

---

### TCP 如何保證傳輸可靠性？

TCP 透過多種機制共同保證可靠性：

1. **序號與確認（Sequence Number + ACK）**：每個位元組都有序號，接收方回傳 ACK 確認收到。沒收到 ACK 就重傳。

2. **重傳機制**：超時未收到 ACK（RTO，重傳超時）或收到重複 ACK（快速重傳）時，自動重傳。

3. **流量控制（滑動視窗）**：接收方告知傳送方自己的接收緩衝區大小（接收視窗），防止傳送方送太快淹沒接收方。

4. **擁塞控制**：偵測網路擁塞（封包遺失、RTT 增加）後，主動降低傳送速率（慢啟動、擁塞避免、快速恢復）。

5. **校驗和（Checksum）**：每個 TCP segment 有校驗和，接收方驗算，有誤則丟棄並等待重傳。

6. **有序投遞**：TCP 在接收端重新排列收到的 segment，確保上層應用收到的資料順序正確。

---

### Linux 系統如何收發網路資料包？

**收包流程（下行）**：

```
網卡（NIC）
  ↓ DMA 把資料寫入核心 Ring Buffer（sk_buff）
  ↓ 觸發硬體中斷（IRQ）
CPU 中斷處理（上半部）
  ↓ 確認中斷，關閉 NIC 中斷，排程軟中斷（NAPI）
軟中斷處理（下半部，NAPI poll）
  ↓ 從 Ring Buffer 取出 sk_buff
  ↓ L2（以太網）處理 → L3（IP）處理 → L4（TCP/UDP）處理
  ↓ 資料放入 Socket 接收緩衝區
應用程式
  ↓ 呼叫 read() / recv() → 從 Socket 緩衝區複製到使用者空間
```

**發包流程（上行）**：

```
應用程式
  ↓ 呼叫 write() / send() → 資料複製到核心 Socket 傳送緩衝區
L4（TCP）處理 → 分段、加 TCP 標頭、序號
L3（IP）處理 → 加 IP 標頭、路由決策
L2（以太網）處理 → 加 MAC 標頭、ARP 解析
  ↓ 放入網卡 TX Ring Buffer
網卡 DMA 傳出 → 觸發傳輸完成中斷
```

**關鍵概念**：**零複製（Zero-copy）** 技術（如 `sendfile()`）可以讓資料直接從核心頁面快取送到網卡，不需要複製到使用者空間再複製回核心，大幅提升高吞吐量傳輸的效能（AMAT 的資料採集系統會用到這個）。

---

### 寫文件時進程崩潰，數據會丟失嗎？

**取決於寫入是否已完成以及是否使用了緩衝**：

**情境分析**：

```
應用程式 write()
  ↓（通常）先寫入「Page Cache（頁面快取）」→ 回傳成功
  ↓（非同步）核心背景把 Page Cache 的資料刷到磁碟（pdflush）
  ↓（最終）資料安全落盤
```

| 情況 | 資料是否丟失？ |
|------|--------------|
| `write()` 已呼叫且回傳成功，但進程崩潰 | **不一定**——資料可能在 Page Cache，還沒到磁碟。進程崩潰不影響 Page Cache，系統正常時會自動刷盤。但若整個系統也崩潰（斷電），資料丟失。 |
| `write()` 後呼叫了 `fsync()` | **安全**——`fsync()` 強制把 Page Cache 刷到磁碟後才回傳，崩潰後資料已在磁碟。 |
| 使用 `O_DIRECT` 旗標開啟檔案 | **安全**——繞過 Page Cache，直接寫磁碟（但速度較慢）。 |
| `write()` 尚未呼叫 | **丟失** |

**設備軟體的實踐**：製程日誌和量測資料通常在每次寫入後呼叫 `fsync()`，確保設備突然斷電時資料完整性，以便事後追溯晶圓損壞原因。

---

### 系統調用的整個流程

系統調用是使用者程式請求作業系統核心服務的機制（如 `read()`、`write()`、`open()`）。

```
使用者空間（User Mode）
  │
  │ 1. 應用程式呼叫 read()（C 標準庫封裝）
  │
  │ 2. 把系統調用號碼放入暫存器（如 rax = 0 代表 read）
  │    把參數放入其他暫存器（rdi, rsi, rdx...）
  │
  │ 3. 執行 syscall 指令（x86-64）或 swi（ARM）
  │    → CPU 切換到核心模式（Ring 0），跳到核心入口點
  │
─────────────────────────────── 使用者/核心 邊界 ───
  │
核心空間（Kernel Mode）
  │
  │ 4. 核心查系統調用表（sys_call_table），根據號碼找到 sys_read()
  │
  │ 5. 核心驗證參數（指針是否合法、緩衝區大小等）
  │
  │ 6. 執行實際的核心操作（等待資料、複製到使用者空間等）
  │
  │ 7. 把回傳值放入 rax，執行 sysret 指令
  │    → CPU 切換回使用者模式
  │
─────────────────────────────── 使用者/核心 邊界 ───
  │
使用者空間
  │
  │ 8. C 標準庫收到回傳值，若為負值則設定 errno 並回傳 -1
  │
  └─ 應用程式繼續執行
```

**性能代價**：每次系統調用都需要兩次 CPU 模式切換（使用者→核心→使用者），有一定開銷，這也是為什麼高效能系統傾向批次讀寫（減少系統調用次數），或使用 `io_uring`（減少模式切換）。

**預期會被問到的面試問題**：
- 「TCP 和 UDP 的差別是什麼？」→ 可靠性、連線、順序、速度
- 「TCP 如何保證傳輸可靠性？」→ 序號/ACK、重傳、流量控制、擁塞控制、校驗和
- 「Linux 是如何收發網路資料包的？」→ NIC → DMA → sk_buff → 協議棧 → Socket 緩衝區 → 應用程式
- 「寫文件時進程崩潰，資料會丟失嗎？」→ 取決於是否已到 Page Cache 及是否 fsync
- 「請描述系統調用的整個流程？」→ 使用者態設定暫存器 → syscall 指令 → 核心態處理 → sysret 返回

---

## 19. C++ 學習路線

> 這是給初學者或轉換跑道工程師的建議路線，按重要性和學習深度排序。

---

### 基礎階段（3～6 個月）

**目標**：能閱讀和修改真實的 C++ 程式碼，通過初級面試。

1. **C++ 基本語法**：變數、型別、控制流、函式、陣列
2. **指針與參考**：`*`、`&`、解參考、nullptr（最重要的基礎之一）
3. **OOP 基礎**：class、建構/析構函式、封裝、繼承、多態
4. **STL 入門**：`vector`、`map`、`string`、`algorithm`（sort/find）
5. **記憶體管理**：stack vs heap、`new`/`delete`、記憶體洩漏、RAII

**學習資源**：
- 書籍：《C++ Primer》（第 5 版，Lippman）
- 線上：cppreference.com（語法查閱）、learncpp.com（教學）

---

### 進階階段（6～12 個月）

**目標**：能設計和實作中型系統，通過 ASML/KLA/LAM/AMAT 的技術面試。

1. **現代 C++11/14/17**：auto、lambda、move語意、智慧指針、constexpr
2. **模板（Templates）**：函式模板、類別模板、模板特化
3. **例外處理**：throw/catch、例外安全保證、RAII 的重要性
4. **並發程式設計**：`std::thread`、`mutex`、`condition_variable`、`atomic`
5. **設計模式**：單例、工廠、觀察者、策略、RAII——以 C++ 實作
6. **調試工具**：GDB、Valgrind（記憶體洩漏）、AddressSanitizer

**學習資源**：
- 書籍：《Effective Modern C++》（Scott Meyers，C++11/14）
- 書籍：《C++ Concurrency in Action》（Williams，並發）

---

### 深度階段（1～2 年，針對半導體設備業）

**目標**：能主導架構設計，解決效能和可靠性問題。

1. **作業系統原理**：行程管理、記憶體管理、I/O、系統調用
2. **網路程式設計**：socket API、epoll、TCP/IP 協議棧
3. **效能優化**：CPU cache、記憶體佈局、SIMD、Profile 工具（perf、VTune）
4. **嵌入式/即時系統**：RTOS 概念、中斷處理、記憶體受限環境下的 C++
5. **領域知識**：半導體製程、設備通訊協議（如 SECS/GEM）

**學習資源**：
- 書籍：《The Linux Programming Interface》（Kerrisk，OS 和系統調用）
- 書籍：《Computer Systems: A Programmer's Perspective》（CS:APP）

---

### 半導體設備公司面試準備重點

根據 ASML / KLA / LAM / AMAT 的常見考題，優先掌握：

| 優先級 | 主題 |
|--------|------|
| ★★★ | 指針/參考、記憶體管理、RAII、智慧指針 |
| ★★★ | OOP（封裝、繼承、多態、虛函數） |
| ★★★ | STL 容器（vector、map）和迭代器 |
| ★★ | 設計模式（單例、門面、觀察者） |
| ★★ | 並發（mutex、thread、data race） |
| ★★ | 現代 C++11 特性（auto、lambda、move） |
| ★ | 網路（TCP/UDP、socket） |
| ★ | 系統調用、Linux 核心概念 |

---

## 快速對照表：專案特性 → 面試考點

| 專案中的程式特性 | 對應的面試考點 |
|----------------|--------------|
| `OAMeetingRoom::_user`（指標） | 指標、null vs 哨兵、指標算術 |
| `it->_user = &user` | 對參考取址、以指標形式儲存 |
| `OAUser* user = new OAUser(...)` | 堆積分配、記憶體洩漏、智慧指標 |
| `r.user() == this` | `this` 指標、指標比較 |
| `bookMeetingRoom(*this, mid)` | `*this` 解參考、把自己以參考形式傳出 |
| `OADataCenter` 單例 | 單例模式、執行緒安全 |
| `OAAdmin` / `OAUser` 門面 | 門面模式、委託、關注點分離 |
| `friend class OADataCenter` | 友元類別、封裝的取捨 |
| `loginAdmin` 回傳 `OAAdmin&` | 參考、哨兵模式、地址比較 |
| `vector<OAUser> users` | STL vector、動態陣列 |
| `users.erase(it)` | 迭代器失效、vector 元素刪除 |
| `auto it = rooms.begin()` | `auto`、型別推導、迭代器型別 |
| 在完整定義前宣告 `OAUser* _user` | 前向宣告、引入依賴管理 |
| 回傳代碼（`MEETING_ROOM_BOOK_SUCCESS`） | 錯誤處理策略、回傳代碼 vs 例外 |
| `static OAAdmin ERROR_ADMIN` | 靜態成員、哨兵物件 |
| 堆疊物件 vs `new`（Room 4 的設計） | 物件生命週期管理、懸空指標、RAII |

---

## 新增題目快速索引

| 題目 | 所在章節 |
|------|---------|
| 怎麼解決 C++ 菱形繼承？ | [第 12 節 — 虛繼承](#12-oop-進階--繼承多態虛函數) |
| 請簡述多態實現的原理 | [第 12 節 — vtable/vptr](#12-oop-進階--繼承多態虛函數) |
| `override`、`final` 的作用 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| 請簡述 C++ 類型推導用法 | [第 13 節 — auto/decltype](#13-類型推導與函數式工具) |
| `function`、`lambda`、`bind` 的關係 | [第 13 節](#13-類型推導與函數式工具) |
| 繼承下的建構/析構呼叫順序 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| 虛函數表和 vptr 的建立時機 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| 請簡述虛析構函式的作用 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| 面對物件三大特徵 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| 預設拷貝建構函式的生成時機 | [第 12 節](#12-oop-進階--繼承多態虛函數) |
| `malloc` 如何分配記憶體？`free` 怎麼知道釋放多少？ | [第 14 節](#14-進階記憶體管理) |
| `malloc`/`free` vs `new`/`delete` | [第 14 節](#14-進階記憶體管理) |
| 頁面置換算法有哪些？ | [第 14 節](#14-進階記憶體管理) |
| 寫時複製（CoW）的原理與應用 | [第 14 節](#14-進階記憶體管理) |
| 智慧指針種類與使用場景 | [第 15 節](#15-智慧指標與現代-c11) |
| C++11 用過哪些特性？ | [第 15 節](#15-智慧指標與現代-c11) |
| 動態庫與靜態庫的區別 | [第 16 節](#16-動態庫與靜態庫) |
| 進程和執行緒的上下文切換差異 | [第 17 節](#17-並發與執行緒) |
| 系統調用能否引起執行緒切換？ | [第 17 節](#17-並發與執行緒) |
| 進程和執行緒的差別 | [第 17 節](#17-並發與執行緒) |
| Reactor 和 Proactor 的差別 | [第 17 節](#17-並發與執行緒) |
| Linux 如何收發網路資料包？ | [第 18 節](#18-網路與系統) |
| 寫文件時進程崩潰，資料會丟失嗎？ | [第 18 節](#18-網路與系統) |
| TCP 如何保證傳輸可靠性？ | [第 18 節](#18-網路與系統) |
| TCP 與 UDP 的差別 | [第 18 節](#18-網路與系統) |
| 系統調用的整個流程 | [第 18 節](#18-網路與系統) |
| C++ 學習方向及學習路線 | [第 19 節](#19-c-學習路線) |
