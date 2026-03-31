---
name: cpp-explainer
description: >
  Guides Claude through a 4-phase workflow to deeply analyze a C++ project and
  produce a detailed, beginner-friendly explanation document (explanation.md)
  written in Traditional Chinese. Use this skill whenever the user wants to
  understand C++ code, generate documentation from C++ source files, or learn
  C++ concepts from a real project — especially if they mention being new to C++,
  coming from Java, or wanting an explanation of what C++ code does. Trigger even
  if the user just says "explain this C++ project to me" or "I don't understand
  this C++ code". The output is always explanation.md in Traditional Chinese with
  code blocks kept in English.
---

# C++ Project Explainer

You are helping a user understand a C++ codebase. They are a complete C++ beginner (likely with a Java background). Your job is to guide them through a 4-phase analysis that produces a thorough, readable `explanation.md` in Traditional Chinese.

Work through the phases **in order**, confirming quality after each phase before moving on.

---

## Phase 1 — Initial Scan & Draft

**Goal:** Read every source file and produce the first draft of `explanation.md`.

1. Locate the project's source files:
   - Check for `include/`, `source/`, and `src/` directories
   - If a `CMakeLists.txt` or `Makefile` exists, read it too — it reveals the build structure
2. Read every `.h`, `.hpp`, and `.cpp` file you find
3. Write `explanation.md` with **numbered items** (Item #1, Item #2, …), one item per concept or file

Each item must cover:
- What this file/concept does, in plain language
- The key C++ concept it illustrates, with a short friendly explanation
- A code snippet taken directly from the project (with line-by-line annotation)

**Concepts to cover** (at minimum — add more if the project uses them):

| Concept | What to explain |
|---|---|
| Header vs. implementation files | Why `.h` and `.cpp` are separate (unlike Java) |
| Namespaces | How they prevent name collisions |
| Classes, inheritance, polymorphism | Compared to Java's class model |
| Constructors & destructors | Lifecycle management; RAII pattern |
| Pointers (`*`) vs. references (`&`) | Semantic differences, when each is used |
| Smart pointers | `unique_ptr`, `shared_ptr`, `weak_ptr` and why they exist |
| Memory: stack vs. heap | Where variables live and who cleans them up |
| Templates | Generic programming, compared to Java generics |
| Exception handling | `try`/`catch`/`throw` in C++ |
| RAII | The idiom of binding resource lifetime to object lifetime |

**Prompt to run (Phase 1):**
> Please look into the `include/` and the `source/` folder and explain these code to me because I am a totally C++ newbie. Include all the C++ important concepts. Put the result into an `explanation.md` file. The more detailed the better.

After Phase 1, review `explanation.md` for completeness before continuing.

---

## Phase 2 — Deepen & Fill Gaps

**Goal:** Find what Phase 1 missed and make every item richer.

1. Re-survey the full codebase (use a directory tree if helpful)
2. Read the current `explanation.md`
3. For every item, check whether there is more code in the project that relates to it
4. For every C++ concept, add all four of the following if not already present:
   - **Plain-language analogy** (pretend you're explaining to a 5-year-old)
   - **Java comparison** (how would you do this in Java?)
   - **Real code snippet** from the project
   - **When and why** you'd use this in practice

**Prompt to run (Phase 2):**
> Please analyse all this project codebase first, and then read the `explanation.md` file. Try to add more detail to complete the `explanation.md` file and make it more clear. Remember one important thing: explain all important concepts and treat me like a five-year-old C++ newbie.

After Phase 2, check that every item has depth and that no source files were missed.

---

## Phase 3 — Backfill Items #1–#20

**Goal:** Phase 2 likely uncovered new code that is also relevant to the earlier items. Go back and add it.

- Read Items #1 through #20 in `explanation.md`
- For each item, check: did Phase 2 surface any code that belongs here but isn't yet mentioned?
- If yes, add the new code snippets and explanations to that item
- **Do not renumber or restructure** — only add content within existing items

**Prompt to run (Phase 3):**
> Don't forget to also modify item #1 to #20 in `explanation.md` due to there also being new code related to item #1 to #20.

After Phase 3, the document should feel complete and internally consistent.

---

## Phase 4 — Translate to Traditional Chinese

**Goal:** Make the document accessible to a Chinese-speaking reader.

Translate all prose to Traditional Chinese. Keep the following in English — do **not** translate:
- Everything inside code blocks
- C++ standard library names (e.g., `std::vector`, `std::unique_ptr`)
- Function names, variable names, class names
- C++ technical terms may be kept in English with a Chinese explanation in parentheses, e.g.: `RAII（資源取得即初始化）`

Style: conversational and easy to read — the reader has a technical background but has never touched C++.

**Prompt to run (Phase 4):**
> Change the language from English to Traditional Chinese for the `explanation.md` file.

---

## Output Specification

| Field | Value |
|---|---|
| File | `explanation.md` (in the project root) |
| Language | Traditional Chinese (code + terms stay in English) |
| Structure | Numbered items: Item #1, Item #2, … |
| Depth | Suitable for a complete C++ beginner with Java background |
| Code annotation | Line-by-line or block-by-block explanations |

---

## Tips

- After each phase, **pause and read the output** before moving on. Quality matters more than speed.
- If the project has unusual structure (no `include/` folder, monolithic files, etc.), adapt the scan strategy accordingly and tell the user what you found.
- If a concept appears in multiple files, pick the clearest example for the main explanation and mention the others as cross-references.
- Phase 3 is additive only — preserve all existing item numbers and structure.
