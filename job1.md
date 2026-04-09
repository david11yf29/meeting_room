# ASML D&E Field – Taiwan Factory Engineering – Software Engineer (Linkou)

## 一、你的背景摘要（依 LinkedIn）

| 期間 | 公司 | 角色 | 重點 |
|---|---|---|---|
| 2025/02–至今 (~1.2y) | TAO Digital Solutions | Software Engineer | Spring Boot、Kafka、PostgreSQL、JUnit、微服務 |
| 2022/09–2024/04 (~1.7y) | KLA | Applications Development Engineer | RDC (Reticle Decision Center) for TSMC HVM、跨部門協作、演算法測試 |
| 2021/06–2022/09 (~1.4y) | KLA | Application Development Engineer | TSMC R&D 新品導入、SQL/Python 分析 |
| 2019/02–2021/06 (~2.4y) | Lam Research | Process Engineer | 3D NAND / DRAM dielectric etching、Metryx metrology、與英國 data scientist 合作 ML model |
| 2018/01–2019/01 (~1.1y) | Lam Research | Process Engineer | Micron CVD/ALD |
| 學歷 | USC MS / Yuan-Ze BS | Chemical Engineering | |
| 語言 | 英文 Professional Working | | |

- **半導體 fab / 客戶端經驗：約 6–7 年**
- **SW / Applications 經驗：約 4 年**（KLA ADE ~3y + TAO ~1.2y）
- **純後端 SW：約 1.2 年**

---

## 二、與 JD 的匹配度分析

| 職缺需求 | 你的狀況 | 匹配度 |
|---|---|---|
| 4+ 年 SW 或 equivalent operational 經驗 | KLA ADE + TAO SWE ≈ 4 年；Lam/KLA operational 6+ 年 | ✅ **符合** |
| C++ / Python / C | Python ✅；C++ 自學中；C 無 | ⚠️ **C++ 是唯一硬傷** |
| DUV 模組知識（Reticle Stage / Wafer Stage / Metro / Illumination / Lens） | **KLA RDC = Reticle 缺陷分類，直接對應 Reticle Stage 領域** | ✅ **強匹配** |
| ASML 診斷系統（metro-analyzer 等） | Lam Metryx metrology tool 啟動、KLA 演算法測試 | ✅ 類似經驗 |
| 製造流程 / cycle time / 品質 | Lam 量產製程 + KLA HVM tool evaluation | ✅ **強匹配** |
| 跨 electrical/mechanical/mechatronics/optical 協作 | Lam/KLA 本來就是跨部門 + 海外 HQ 協作 | ✅ **強匹配** |
| Quick learner、ambiguous env、problem solving | 化工 → Process → Apps → SWE 的轉職軌跡本身就是證明 | ✅ **強匹配** |
| 英文 working level | Professional Working | ✅ |
| Physics/CS/Math 學位（preferred） | Chemical Engineering (USC MS) | ⚠️ preferred 非必要，ChemE 在半導體業普遍被接受 |
| 在地（林口）+ 出差 10% | 你在台灣 | ✅ |

**綜合判斷：這個職缺的匹配度比你想像中高很多。** ASML 這個 JD 明顯是在找「半導體 fab 領域 + 寫得動 code」的混合型人才，不是純粹的 SW engineer。你的 KLA RDC 經驗幾乎是 tailor-made for Reticle 相關模組。

---

## 三、你的關鍵賣點（面試／履歷一定要講）

1. **Reticle 領域直接對接**：KLA RDC 負責 reticle defect classification flow in Linux，直接對應 JD 的 Reticle Stage 模組
2. **TSMC HVM 實戰**：first tool evaluation for TSMC HVM — ASML 在林口廠最大客戶之一就是 TSMC
3. **跨國跨部門協作**：與 Lam UK HQ data scientist、KLA division team 合作 — 對應 JD "multiple sites stakeholder management"
4. **從 Process → Application → Backend SW 的轉型**：完美詮釋 JD 的 "quick learner, generalist mindset"
5. **量產環境 diagnosis 經驗**：Lam onsite 3D NAND/DRAM etching 支援，就是 JD 裡 "resolve SW-related production issues" 的 operational 版本
6. **Backend SW hard skill**：Spring Boot / Kafka / PostgreSQL / JUnit — 證明你能寫 production code、做 testing、懂 async messaging

---

## 四、Gap 與提升方式（條列）

### Gap 1：C++（最大弱點，必須補）
- **短期（1–2 個月）**：
  - 完成 meeting_room 專案，推上 GitHub
  - 讀 *Effective Modern C++*（Scott Meyers）前 20 條
  - 熟悉：RAII、smart pointer、move semantics、STL、lambda
  - 工具：CMake、gdb、valgrind、AddressSanitizer
- **中期（2–4 個月）**：
  - 做 1 個多執行緒 C++ 專案（thread pool / producer-consumer）
  - LeetCode 用 C++ 寫 50 題中等難度
  - 讀一段 open source C++ 程式碼（如 folly、abseil 小模組）
- **面試話術**：「Java 後端的 OOP/並行/記憶體概念已掌握，C++ 是語法與 memory ownership 的遷移；目前正用 meeting_room 專案實作 RAII 與 smart pointer 來內化這些差異。」

### Gap 2：DUV 微影領域知識
- 你熟 etching/CVD/ALD/reticle inspection，但不熟 **scanner/stepper 本體**
- 補充材料：
  - ASML 官方 YouTube：How does a lithography scanner work?
  - 關鍵字搜尋：immersion DUV、overlay、CD uniformity、throughput、reticle stage stability
  - 讀 ASML 年報 / investor deck（了解 NXT、NXE 產品線）

### Gap 3：ASML 內部工具與流程
- 面試時主動問：「metro-analyzer 是否有公開資料可先預習？」展現主動性
- 熟悉 8D / RCA / containment action / CAPA 術語
- 強調你在 Lam 做過 onsite containment support

### Gap 4：學位背景
- ChemE 不是 preferred 但在半導體業普遍接受
- 面試話術：「ChemE 讓我在 process/fab 溝通有天然優勢，同時 USC 期間有 clean room、lithography、ICP etching 實作經驗」——你履歷裡 USC 的 1-D grating fabrication 可以提出來強調**你真的做過 lithography**

---

## 五、履歷調整建議

1. **最上面 Summary 重寫**：強調 "semiconductor fab + backend SW hybrid"，不要只寫 Java Backend Developer
2. **KLA RDC 的描述放大**：把 "reticle defect classification flow in Linux" 擴寫，加入規模數字（TSMC HVM tool 數量、accuracy 改善百分比）
3. **技能區補上**：C++ (learning)、Linux shell、Python、SQL、Git
4. **把 USC 的 lithography / clean room 實作從 Research Assistant 抽出來獨立成一段**，對應 JD 的 "lithography exposure"
5. **TAO 經驗量化**：QPS、微服務數量、訊息吞吐量

---

## 六、面試準備重點

### 行為題
- 「講一個你在量產線上 debug 的經驗」→ Lam etching tool 事件
- 「講一個跨國跨部門協作的經驗」→ Lam × UK data scientist ML model
- 「為什麼從 Process Engineer 轉 SW？」→ 強調在 KLA 看到演算法/SW 的槓桿效應，主動學習而非被動轉職
- 「學習新技術的方法」→ Java → C++ 的自學路徑

### 技術題（準備方向）
- Python：pandas、log parsing、regex、multiprocessing
- C++：memory model、pointer vs reference、RAII、STL container
- Linux：shell、systemd、log 分析、core dump
- 架構題：如何設計一個「線上 tool SW patch 的 rollback 機制」

---

## 七、總評與建議

- **建議：直接投遞，匹配度高**
- 你不是「Java 工程師想跳半導體」，而是「半導體 fab 老手 + 後端 SW」雙棲人才——這正是 ASML 這種職缺最難找的組合
- 最大風險是 **C++ hard skill 驗證**：面試前務必把 C++ 基礎練到能白板寫 RAII / smart pointer / thread
- 若收到面試，強調 **Reticle 經驗 + Lam onsite containment 經驗 + 英文溝通**，C++ 用「正在用 meeting_room 專案學習」誠實回答即可
- **備案**：若此職缺沒上，ASML 林口廠另有 Customer Support Engineer / D&E Software Support 等職缺，你的背景也高度匹配，可一併投遞
