# 未來學習路線圖

## 選專案的核心邏輯

目標不是累積 side project 數量，而是練到以下幾個關鍵能力：

- **系統層面的 C++**：記憶體管理、多執行緒、資源生命週期
- **硬體抽象／通訊協議**：設備軟體的核心日常
- **可靠性設計**：類比 Kafka 的思維，用 C++ 實作

---

## 推薦專案清單（由淺到深）

### 第一階段：打基礎，驗證 C++ 語感

#### SerenityOS
- GitHub：`SerenityOS/serenity`
- **為什麼推**：完整的 C++ 作業系統，程式碼品質極高，包含 driver、IPC、memory allocator。可只專注某個子模組，不必全讀
- **能學到**：RAII 在 driver 層的實際應用、如何用 C++ 管理硬體資源
- **重要性**：OS 層的資源管理概念在大廠設備軟體面試中高頻出現

#### cpp-httplib / Crow（HTTP server）
- 複雜度相對較低，可對比熟悉的 Spring Boot
- 建立「C++ 做後端服務」的語感，理解沒有 DI container 時該如何組織程式碼

---

### 第二階段：對應半導體設備情境

#### OpenCV
- GitHub：`opencv/opencv`
- **為什麼推**：KLA 的瑕疵檢測（defect inspection）、AMAT 的製程控制（process control）都大量使用影像處理
- **能學到**：大型開源 C++ 專案結構、效能導向的 C++ 寫法、CMake 構建系統
- **建議方式**：先跟著官方 tutorial 做幾個範例，再閱讀 `core/` 模組的 `Mat` class 實作，重點觀察 buffer 生命週期的管理方式

#### MRPT（Mobile Robot Programming Toolkit）
- GitHub：`MRPT/mrpt`
- **為什麼推**：ASML 的晶圓搬運（wafer handler）、Lam 的機械手臂控制，都屬於類似的運動控制領域
- **能學到**：Real-time 系統設計、感測器資料融合、硬體抽象層設計

#### libmodbus／序列埠相關專案
- **為什麼推**：Modbus 是工業設備的標準通訊協議，序列埠通訊是設備軟體的日常工作
- **建議**：自己動手實作一個「模擬設備通訊的 C++ client」，使用 libmodbus 或 Boost.Asio

---

### 第三階段：貼近 ASML／AMAT 面試題型

#### 自製簡易 OS（Write a Simple OS in C++）
- 推薦資源：`cfenollosa/os-tutorial`（GitHub）或 OSDev wiki 的 C++ bare-metal 教程
- **為什麼**：ASML 機台底層本質上是 real-time embedded system，理解 interrupt handling、memory-mapped IO 是明顯加分項

#### 自建專案：模擬設備數據蒐集 Pipeline（最有效）

結合現有的 Kafka 背景，自行設計一套 pipeline：

```
[模擬感測器] --序列埠--> [C++ 資料蒐集 daemon] --共享記憶體--> [處理模組] --> [Log / Alert]
```

- 可討論此 pipeline 的可靠性設計（對比 Kafka 的思維）
- 面試時這個專案故事具有很強的說服力

---

## 建議學習順序

1. 用 **cpp-httplib** 建一個簡單的 REST server，驗證 C++ 基礎語感
2. 閱讀 **OpenCV 的 `Mat` class 實作**，專注記憶體管理的部分
3. 用 **libmodbus** 或 **Boost.Asio** 完成一個通訊小專案
4. 最後回頭閱讀 **SerenityOS 的 driver 子模組**，此時已有足夠背景讀懂

---

## 下一步確認

目前 C++ 學習進度到哪個階段？

- [ ] 語法已通，pointer／reference 沒問題
- [ ] 還在熟悉 pointer／reference 階段

確認後可以更精確地決定從哪個專案切入。
