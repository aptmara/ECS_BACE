````markdown
# GitHub Copilot カスタム指示 - HEW_GAME プロジェクト（C++17統一版）

このファイルは、GitHub Copilot が本プロジェクト（**HEW_GAME**）でコードを生成・修正する際に従うべき **技術規約・運用フロー** を定義する。  
**大規模変更前には必ず「プロジェクト読込 → 計画立案 → レビュー承認」を経ること。**

---

## プロジェクト概要

- **プロジェクト名**: HEW_GAME
- **目的**: Entity Component System (ECS) を活用したチームゲーム開発
- **言語**: **C++17（厳守）**
- **プラットフォーム**: Windows / DirectX 11
- **アーキテクチャ**: Entity Component System
- **開発スタイル**: Git/GitHub（Draft PR + 承認ゲート運用）

---

## 重要な制約事項（C++ 標準）

### 使用可能（C++17）
```cpp
// 例示（OK）
std::optional<int> v;
if constexpr (std::is_same_v<T, U>) { /* ... */ }
std::filesystem::path p{"assets/texture.png"};
auto [x, y, z] = GetPosition();
inline constexpr int MAX_ENTITIES = 1000;
````

### 使用禁止（C++20 以降）

```cpp
// 例示（NG）
std::span<int> s;                 // C++20
std::format("{}", 42);            // C++20
concepts / ranges / coroutine 等   // C++20+
```

> 旧来の「C++14 互換代替」節は **廃止**。必要なら C++17 での正攻法を採用する。

---

## **出力制約（Copilot のコメント/テキスト生成を全面制御）**

**目的**: Copilot が C++ ファイル上に「🆕 などの絵文字」「補足・メモ・NOTE」等の**装飾/自然文**を挿入することを**全面禁止**する。

### 絶対禁止

* **絵文字・顔文字・装飾記号**（例: `🆕`, `✨`, `📌`, `🔥`, `✅`, `❗`, `❌`, `💡` などの Unicode 絵文字全般）
* **自然文の補足/感想/助言**（例: `NOTE:`, `Tip:`, `補足:`, `メモ:` など）。レビュー依頼文の自動挿入も不可
* **Markdown 見出し/箇条書き**の**コード外**生成（C++ ファイル内に `#`, `- ` 等で見出し/リストを書かない）
* **関数本体内のコメント**全般（例外を除く。下記「唯一許可」参照）

### 唯一許可（最小限のみ）

* **Doxygen コメント**のみ（`@file`, `@brief`, `@details`, `@note`, `@warning`, `@param`, `@return`, `@tparam`, `@version`, `@date`, `@author`）。

  * 位置: **宣言直前**（クラス/構造体/関数/ファイル先頭）。
  * 内容: **機能説明に限定**。主観・感嘆・雑談・装飾表現は不可。
* **抑止例外**: 静的解析のための **NOLINT** 等、**ツール指示コメント**のみ許可（必要最小限）。

### Copilot への出力指示（強制）

* **コード以外の文章を出力するな**。説明が必要なら **Doxygen の最小限**に収める。
* 上記に反する生成を試みる場合、**出力を空にする**か**Doxygen の最小形式**に自動変換すること。
* **TODO** を出力したい場合は **Jira/Issue 番号必須**（例: `// TODO[PROJ-1234]: …`）。番号なし TODO は禁止。

---

## ECS アーキテクチャの原則

### Entity（識別子のみ）

```cpp
struct Entity {
    uint32_t id;   // エンティティID
    uint32_t gen;  // 世代番号（削除/再利用管理）
};
```

**禁止**: Entity にロジックや状態を持たせない。

### Component（2 種類）

1. **データコンポーネント（IComponent 継承）**: データ保持＋軽いヘルパのみ

```cpp
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;
    void TakeDamage(float dmg) {
        current -= dmg;
        if (current < 0.0f) current = 0.0f;
    }
    bool IsDead() const { return current <= 0.0f; }
};
```

2. **Behaviour コンポーネント（Behaviour 継承）**: 毎フレーム更新されるロジック

```cpp
struct Rotator : Behaviour {
    float speedDegY = 45.0f;
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) t->rotation.y += speedDegY * dt;
    }
};
```

### System 実装パターン

* **Behaviour パターン（推奨）**: 各 Entity に振る舞いを装着
* **ForEach パターン**: データ指向に一括処理

```cpp
void UpdateMovementSystem(World& world, float dt) {
    world.ForEach<Transform, Velocity>([dt](Entity, Transform& t, Velocity& v) {
        t.position.x += v.velocity.x * dt;
        t.position.y += v.velocity.y * dt;
        t.position.z += v.velocity.z * dt;
    });
}
```

### Component 規約

* **`include/samples/ComponentSamples.h` は** *設計の原案集* **のみ**。ゲームコードから **参照・include 禁止**。

  * ビルド対象から除外すること。`src/**` での `#include "samples/ComponentSamples.h"` を検出した場合は CI で失敗させる（後述の CI 参照）。

---

## コーディング規約

| 要素      | 規約                     | 例                                          |
| ------- | ---------------------- | ------------------------------------------ |
| クラス/構造体 | PascalCase             | `Transform`, `MeshRenderer`, `World`       |
| 関数      | PascalCase             | `CreateEntity()`, `TryGet()`, `OnUpdate()` |
| 変数      | camelCase              | `deltaTime`, `entityId`, `speed`           |
| メンバ変数   | camelCase + `_` サフィックス | `world_`, `nextId_`                        |
| 定数      | UPPER_SNAKE_CASE       | `MAX_ENTITIES`, `DEFAULT_SPEED`            |

```cpp
class PlayerController : public Behaviour {
public:
    float speed = 5.0f;
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* transform = w.TryGet<Transform>(self);
        if (transform) transform->position.x += speed * dt;
    }
private:
    InputSystem* input_ = nullptr;
};
```

---

## World クラスの使用

### エンティティ作成（ビルダーパターン推奨）

```cpp
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0,0,0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0,1,0})
    .With<Rotator>(45.0f)
    .With<PlayerTag>()
    .Build();
```

### コンポーネント操作

```cpp
if (auto* t = world.TryGet<Transform>(entity)) { t->position.x += 1.0f; }
world.Add<Health>(entity, Health{100.0f, 100.0f});
if (world.Has<Transform>(entity)) { /* ... */ }
world.Remove<Health>(entity);
world.DestroyEntityWithCause(entity, World::Cause::Collision);
```

### ForEach 利用

```cpp
world.ForEach<Transform>([](Entity, Transform& t) { t.position.y += 0.1f; });
world.ForEach<PlayerTag, Transform>([](Entity, PlayerTag&, Transform& t) { /* プレイヤのみ */ });
```

---

## DirectXMath の使用（保持は `XMFLOAT*`、計算は `XMVECTOR`）

```cpp
void MoveTowards(Transform& t, const DirectX::XMFLOAT3& target, float speed) {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&t.position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR dir = XMVectorSubtract(tgt, pos);
    dir = XMVector3Normalize(dir);
    XMVECTOR move = XMVectorScale(dir, speed);
    XMStoreFloat3(&t.position, XMVectorAdd(pos, move));
}
```

---

## ドキュメンテーション規約（Doxygen のみ許可）

ファイル/宣言直前に限る。**関数本体内へのコメントは原則禁止**（NOLINT 等のツール指示を除く）。

```cpp
/**
 * @file MyComponent.h
 * @brief コンポーネントの説明
 * @author ...
 * @date 2025
 * @version 6.0
 */
```

* `@brief`, `@details`, `@note`, `@warning`, `@param`, `@return`, `@tparam` を必要最小限で付与。
* 絵文字・装飾記号・感嘆・主観は禁止。

---

## 禁止事項（アーキ破壊/非推奨パターン）

* Entity にロジック/状態を追加しない
* グローバルでエンティティ管理しない（World 管理）
* コンポーネントから別コンポーネントへの**直接生ポインタ保持**禁止（World 経由で取得）
* Update 内で同期的に大量スポーン/破棄しない（**Enqueue** を使用）
* 毎フレームの不要な動的確保を避け、メンバ再利用

---

## デバッグとログ（規約）

```cpp
#include "app/DebugLog.h"
DEBUGLOG("Entity created: ID=%u", entity.id);
DEBUGLOG_WARNING("Transform not found on entity %u", entity.id);
DEBUGLOG_ERROR("Failed to load resource: %s", resourceName.c_str());
#ifdef _DEBUG
DEBUGLOG("Delta time = %f", deltaTime);
#endif
```

**セキュリティ/運用**

* PII/シークレット（鍵/トークン/メール）を出力しない
* スロットリング：同一メッセージは 1 秒に 1 回まで（呼び出し側で制御）
* `INFO`, `WARN`, `ERROR` 以外は `_DEBUG` ビルド限定

---

## マクロ活用

### DEFINE_DATA_COMPONENT

```cpp
DEFINE_DATA_COMPONENT(Score,
    int points = 0;
    void AddPoints(int p) { points += p; }
    void Reset() { points = 0; }
);
```

### DEFINE_BEHAVIOUR

```cpp
DEFINE_BEHAVIOUR(CircularMotion,
    float radius = 3.0f;
    float speed = 1.0f;
    float angle = 0.0f;
,
    angle += speed * dt;
    if (auto* t = w.TryGet<Transform>(self)) {
        t->position.x = cosf(angle) * radius;
        t->position.z = sinf(angle) * radius;
    }
);
```

---

## 変更計画と承認フロー（Large Change Gate）

**目的**: 影響の大きい修正に対し、**着手前の「計画→レビュー→承認」を必須化**。

### 大規模変更の判定基準（以下のいずれか）

* 変更行数（加算+削除） > **300**
* **新規ファイル 3 個以上** または **公開 API/コンポーネントのシグネチャ変更**
* **Core 領域**（`include/ecs/*`, `include/components/*`）に触れる
* **スレッドモデル/ライフサイクル**へ影響（スポーン/破棄/更新順序/同期）
* **ビルド設定/依存追加** を含む

### 実行手順

1. **コードベース読込**（関連 `.h/.cpp`、呼び出し関係、依存）
2. **計画作成（PLAN.md 生成）** – 本書テンプレ使用
3. **ドラフト PR 作成（Draft 指定）** – PR 本文に PLAN.md を貼付、レビューア指名
4. **承認ゲート** – レビューアが `APPROVED: <氏名 or チーム>` コメントを付けるまで、**実装はスケルトン/スタブ最小限**
5. **実装開始** – 計画から逸れる場合は PLAN.md 更新 → **再承認** 必須

### 禁止事項

* `APPROVED:` コメント **前** に大規模改変をマージ/Push
* Core 領域の無断編集（Hotfix は `HOTFIX:` チケット + 最小差分に限る）

---

## PLAN.md テンプレート（貼付用）

```md
# PLAN.md – 変更計画

## 概要
- タイトル: <変更名>
- 目的: <ユーザ価値/性能/保守性>
- スコープ: <対象シーン/システム/コンポーネント>

## 影響範囲
- 既存 API/ABI: <有/無 + 詳細>
- 変更予定ファイル:
  - include/...
  - src/...

## 設計方針
- アーキテクチャ: <ECS 原則に沿った分離>
- データフロー/更新順序: <OnStart/OnUpdate/ForEach>
- 代替案比較: <案A/案B/不採用理由>

## 失敗モードと緩和策
- 競合/並列: <EnqueueSpawn/Destroy、イテレータ無効化防止>
- ライフサイクル: <Entity 世代/参照失効>
- 数値安定性: <正規化前の長さチェック/NaN対策>

## マイルストーン
- M1: スケルトン導入（テスト通過）
- M2: 機能A
- M3: 機能B/負荷試験

## 計測/検証
- 性能指標: <フレーム時間、ForEach 走査件数>
- テスト: <単体/統合/シーン回帰>

## ロールバック
- 手順: <Revert/Feature Flag>

## レビュー依頼
- レビューア: <担当者>
- 承認トークン: `APPROVED: <氏名>`
```

---

## 変更凍結領域（要承認）

* `include/ecs/World.h`
* `include/ecs/Entity.h`
* `include/components/Component.h`
* `include/components/Transform.h`
* `include/components/MeshRenderer.h`

**規則**: 上記に触れる PR は **無条件で PLAN.md 必須**。`APPROVED:` 付与前は Draft のまま。

---

## DirectXMath 安全ガード（完全形）

```cpp
static inline void MoveTowardsSafe(Transform& t, const DirectX::XMFLOAT3& target, float speed, float dt) {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&t.position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR delta = XMVectorSubtract(tgt, pos);
    XMVECTOR lenSq = XMVector3LengthSq(delta);
    if (XMVectorGetX(lenSq) < 1e-12f) return;
    XMVECTOR dir = XMVector3Normalize(delta);
    XMVECTOR step = XMVectorScale(dir, speed * dt);
    XMStoreFloat3(&t.position, XMVectorAdd(pos, step));
}
```

---

## チーム開発ルール

### ファイル編集の優先順位

* **コア（触らない/要承認）**: 前節「変更凍結領域」参照
* **自由に編集**: `include/scenes/`, `include/components/Custom*.h`, `src/`
* **要相談**: `include/graphics/`, `include/input/`, `include/app/`

### Git コミットメッセージ

```bash
git commit -m "feat: Add player shooting system"
git commit -m "fix: Resolve collision detection NaN at zero-length"
git commit -m "docs: Update README with component guide"
git commit -m "perf: Optimize render loop"
git commit -m "refactor: Restructure component stores"
```

---

## Copilot 前段タスク（大規模変更時は必須）

* プロジェクトを走査し、関連ファイル/呼び出し関係/依存を列挙
* **PLAN.md** を自動生成（テンプレ厳守）
* Draft PR 本文に PLAN.md を埋め込むテキストを出力
* 承認待ちコメントを自動挿入:
  `READY FOR REVIEW – Reply with "APPROVED: <name>" to proceed.`
* `APPROVED:` が付くまで**実装コード生成はスケルトン/インタフェースのみに限定**

---

## **CI / フックによる強制（実装者向け）**

> Copilot 規約を**機械で担保**する。以下は**プロジェクトに同梱**して運用する。

### 1) GitHub Actions（PR の追加行のみ検査）

`.github/workflows/lint-no-emoji.yml`

```yaml
name: Lint - forbid emojis and decorative comments
on:
  pull_request:
    types: [opened, synchronize, reopened, ready_for_review]
jobs:
  lint-no-emoji:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with: { fetch-depth: 0 }
      - name: Compute base
        id: base
        run: |
          git fetch origin ${{ github.base_ref }} --depth=1
          echo "base=$(git merge-base HEAD origin/${{ github.base_ref }})" >> $GITHUB_OUTPUT
      - name: Scan added lines for forbidden content
        run: |
          python3 - << 'PY'
          import os, re, sys, subprocess
          base = os.environ.get("BASE") or "${{ steps.base.outputs.base }}"
          diff = subprocess.check_output(["bash","-lc",f"git diff -U0 {base}...HEAD"]).decode("utf-8","ignore")
          added = []
          for line in diff.splitlines():
            if line.startswith('+++ ') or line.startswith('--- '): continue
            if line.startswith('+') and not line.startswith('++'):
              added.append(line[1:])
          FORBID_STRINGS = ["🆕","✨","📌","🔥","✅","❗","❌","💡","NOTE:", "Tip:", "補足:", "メモ:", "Ready for review"]
          def has_forbidden_string(s): return any(t in s for t in FORBID_STRINGS)
          RANGES = [(0x1F300,0x1FAFF),(0x1F1E6,0x1F1FF),(0x2600,0x27BF)]
          def has_emoji(s):
            for ch in s:
              c=ord(ch)
              for lo,hi in RANGES:
                if lo<=c<=hi: return True
            return False
          violations=[]
          for i,l in enumerate(added,1):
            if has_forbidden_string(l) or has_emoji(l):
              violations.append((i,l))
          # samples の include 監査
          inc_viol=[(i,l) for i,l in enumerate(added,1) if '#include "samples/ComponentSamples.h"' in l]
          if violations or inc_viol:
            print("❌ Forbidden content detected in added lines:")
            for i,l in violations: print(f"[+{i}] {l}")
            for i,l in inc_viol:  print(f"[+{i}] include forbidden: {l}")
            sys.exit(1)
          print("✅ No forbidden decorative content found.")
          PY
        env:
          BASE: ${{ steps.base.outputs.base }}
```

### 2) ローカル pre-commit（Bash）

`.git/hooks/pre-commit`

```bash
#!/usr/bin/env bash
set -euo pipefail
files=$(git diff --cached --name-only --diff-filter=ACM | tr '\n' ' ')
[ -z "$files" ] && exit 0
diff=$(git diff --cached -U0 -- $files)

forbidden_strings=("🆕" "✨" "📌" "🔥" "✅" "❗" "❌" "💡" "NOTE:" "Tip:" "補足:" "メモ:" "Ready for review" '#include "samples/ComponentSamples.h"')
has_violation=0
while IFS= read -r line; do
  [[ "$line" =~ ^\+\+ ]] && continue
  [[ "$line" =~ ^\+ ]] || continue
  l="${line:1}"
  for s in "${forbidden_strings[@]}"; do
    if [[ "$l" == *"$s"* ]]; then
      echo "pre-commit: NG: $l"
      has_violation=1
      break
    fi
  done
  while IFS= read -r -n1 ch; do
    [[ -z "$ch" ]] && break
    code=$(printf '%d' "'$ch")
    if { [ $code -ge 127744 -a $code -le 129535 ] || [ $code -ge 127462 -a $code -le 127487 ] || [ $code -ge 9728 -a $code -le 10175 ]; }; then
      echo "pre-commit: NG (emoji): $l"
      has_violation=1
      break
    fi
  done <<< "$l"
done <<< "$diff"

if [ $has_violation -ne 0 ]; then
  echo "pre-commit: 絵文字/補足コメントは禁止。削除して再コミット。"
  exit 1
fi
exit 0
```

### 3) ローカル pre-commit（PowerShell / VS2022）

`.git/hooks/pre-commit.ps1`

```powershell
#Requires -Version 5.1
$ErrorActionPreference = "Stop"
$files = git diff --cached --name-only --diff-filter=ACM
if ([string]::IsNullOrWhiteSpace($files)) { exit 0 }
$diff = git diff --cached -U0 -- $files
$forbidden = @("🆕","✨","📌","🔥","✅","❗","❌","💡","NOTE:","Tip:","補足:","メモ:","Ready for review",'#include "samples/ComponentSamples.h"')
$hasViolation = $false
$lines = $diff -split "`n"
foreach ($ln in $lines) {
    if ($ln.StartsWith("++")) { continue }
    if (-not $ln.StartsWith("+")) { continue }
    $l = $ln.Substring(1)
    foreach ($s in $forbidden) {
        if ($l.Contains($s)) { Write-Host "pre-commit: NG: $l"; $hasViolation = $true; break }
    }
    foreach ($ch in $l.ToCharArray()) {
        $code = [int][char]$ch
        if ( ($code -ge 127744 -and $code -le 129535) -or
             ($code -ge 127462 -and $code -le 127487) -or
             ($code -ge 9728   -and $code -le 10175) ) {
            Write-Host "pre-commit: NG (emoji): $l"
            $hasViolation = $true
            break
        }
    }
}
if ($hasViolation) {
    Write-Error "pre-commit: 絵文字/補足コメントは禁止。削除して再コミット。"
    exit 1
}
exit 0
```

### 4) PR テンプレート（ヒューマンチェック）

`.github/pull_request_template.md`

```md
## 禁止事項チェック
- [ ] 差分に絵文字/装飾コメント（`🆕`, `✨`, `📌` 等）は**一切ない**
- [ ] C++ ファイル内の自然文コメントは**Doxygen（宣言直前）限定**
- [ ] `#include "samples/ComponentSamples.h"` を参照していない
```

---

## 参考ファイル

* `include/samples/ComponentSamples.h` – コンポーネント実装例（**参照禁止・ビルド除外**）
* `include/samples/SampleScenes.h` – シーン実装例
* `include/scenes/MiniGame.h` – 小規模ゲーム実装
* `include/ecs/World.h` – 利用ガイド/インタフェース

---

## コード生成チェックリスト（Copilot 用）

* [ ] **C++17 準拠**（C++20+ 機能は不使用）
* [ ] **出力制約**順守（絵文字・補足・NOTE 等の**自然文禁止** / Doxygen 最小限のみ）
* [ ] ECS の分離（Entity / Component / System）
* [ ] コンポーネントは `IComponent` または `Behaviour` を継承
* [ ] エンティティ作成はビルダーパターン（推奨）
* [ ] ポインタ取得は `TryGet` を用いて null チェック
* [ ] 命名規約（PascalCase / camelCase / `_` 接尾辞）遵守
* [ ] Doxygen コメントは**宣言直前のみ**。本体内コメントは禁止（NOLINT 等を除く）
* [ ] DirectXMath の型（XMFLOAT*）と計算手順の分離
* [ ] グローバル管理を避け、World 管理
* [ ] リーク無し（World が所有権）/ ライフサイクル安全
* [ ] **大規模変更時は PLAN.md + Draft PR + `APPROVED:` 承認を確認**

---

## 作成者・メタ情報

* **作成者**: 山内陽
* **最終更新**: 2025
* **バージョン**: v7.0 – C++17統一・コメント出力禁止強化

```

---

### 追加改善提案（任意）
- `clang-tidy` に `readability-*` を有効化し、**関数本体内コメントの多用**を警告化。
- `cmake` 側で `samples/` を**ビルド対象外**に固定（`add_subdirectory(samples EXCLUDE_FROM_ALL)` 等を使わない）。
- `pre-push` フックで **C++20 記号**（`<format>`, `concept` 等）の禁則も検査しておく。
```
