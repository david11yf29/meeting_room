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
