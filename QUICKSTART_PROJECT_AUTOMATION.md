# プロジェクト自動化 - クイックスタート

## 現在の問題
? Issue がプロジェクトに追加される
? **Role, Team, Priority などのフィールドが空のまま**

## 修正内容
? Issue テンプレートから情報を自動抽出
? プロジェクトのカスタムフィールドに自動設定
? **フィールドIDを自動取得**（手動設定不要）
? 手動入力不要

---

## ?? すぐに実行する手順

### ? ステップ1: ワークフローファイルの更新確認

最新の `.github/workflows/add_to_project.yml` は以下の機能を含んでいます：

- ? Issue からフィールドを自動抽出
- ? プロジェクトのフィールドIDを動的に取得
- ? デバッグログ出力
- ? エラーハンドリング

**追加の設定は不要です！** すぐにテストできます。

### ? ステップ2: テスト

1. 新しい Task Issue を作成
2. **必ず以下のフィールドを入力**:
   - Role: `Programmer` / `Designer` / `Planner` / `QA` から選択
   - ProgramTeam: `A` / `B` / `C` から選択
   - Priority: `High` / `Medium` / `Low` から選択
   - Start date: `2025-01-15` 形式で入力
   - End date: `2025-01-20` 形式で入力
3. Submit
4. **Actions タブで確認**:
   - "Auto Add Issue to Project" ワークフローの実行ログを確認
   - `? Set Role: Programmer` などのログが表示されるはず
5. **プロジェクトボードで確認**: フィールドが自動的に設定される！

---

## ?? 対応フィールド（自動マッピング）

| Issue テンプレート | プロジェクトフィールド | タイプ |
|------------------|-------------------|--------|
| Role (dropdown) | Role | Single Select |
| Team (dropdown) | ProgramTeam | Single Select |
| Priority (dropdown) | Priority | Single Select |
| Component (dropdown) | Component | Single Select |
| Size (dropdown) | Size | Single Select |
| Start date (input) | Start date | Date |
| End date (input) | End date | Date |
| Estimate (input) | Estimate | Number |

**注意**: プロジェクトのフィールド名と完全に一致する必要があります。

---

## ?? トラブルシューティング

### 1. フィールドが空のまま

**確認項目:**

1. **Actions タブでログを確認**
   ```
   Repository → Actions → "Auto Add Issue to Project" → 最新の実行
   ```

2. **"Debug - Show extracted fields" ステップを確認**
   ```
   Role: Programmer
   Team: A
   Priority: High
   ```
   
   値が抽出されていない場合:
   - Issue テンプレートで「Choose an option」のまま Submit していないか確認
   - 必須フィールド（Role, Priority）を必ず入力

3. **"Set Project Fields" ステップを確認**
   ```
   ? Set Role: Programmer
   ? Set ProgramTeam: A
   ? Set Priority: High
   ```
   
   エラーが出ている場合:
   - プロジェクトのフィールド名が一致しているか確認
   - トークンの権限を確認（下記参照）

### 2. "Warning: Field not found in project"

プロジェクトのフィールド名と Issue テンプレートが一致していません。

**修正方法:**
1. プロジェクトのフィールド名を確認
   - GitHub Projects → Settings → Fields
2. `.github/workflows/add_to_project.yml` の `setSingleSelectField()` 呼び出しを修正
   ```javascript
   // 例: "ProgramTeam" を "Team" に変更
   await setSingleSelectField('Team', process.env.TEAM);
   ```

### 3. "Warning: Option not found for field"

Issue で選択した値がプロジェクトのオプションに存在しません。

**修正方法:**
1. ログで利用可能なオプションを確認
   ```
   Available options: ["Programmer", "Designer", "Planner", "QA"]
   ```
2. Issue テンプレートのオプションを一致させる
   ```yaml
   # .github/ISSUE_TEMPLATE/01_task.yml
   - type: dropdown
     id: role
     attributes:
       label: Role
       options: [Programmer, Designer, Planner, QA]  # 完全一致させる
   ```

### 4. トークン権限エラー

**必要な権限:**
- ? `project` (read/write)
- ? `repo` (read)
- ? `org:read` (組織プロジェクトの場合)

**トークンの再生成:**
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. "Generate new token (classic)"
3. 上記の権限を選択
4. Repository Settings → Secrets → `ORG_PROJECTS_TOKEN` を更新

### 5. 日付が反映されない

- 形式が `YYYY-MM-DD` になっているか確認（例: `2025-01-15`）
- `2025/01/15` や `15-01-2025` は不可
- "No date" や空欄の場合は設定されません

---

## ?? デバッグモード

詳細なログを確認する方法:

1. Actions タブ → 該当ワークフロー → "Set Project Fields" ステップ
2. 以下のようなログが表示されます:
   ```
   Setting fields for item: PVTI_lAHO...
   ? Set Role: Programmer
   ? Set ProgramTeam: A
   ? Set Priority: High
   ? Set Start date: 2025-01-15
   Skipping End date: no value
   ? All fields processed
   ```

**エラー例:**
```
? Failed to set Role: Field value validation failed
```
→ 選択した値がプロジェクトのオプションに存在しない

---

## ?? 動作確認チェックリスト

- [ ] Issue を作成
- [ ] Actions でワークフローが成功（緑チェック）
- [ ] ログで "? Set Role:" などが表示される
- [ ] プロジェクトボードでフィールドが設定されている
- [ ] Start date / End date が反映されている

すべて ? なら成功です！

---

## ?? よくある質問

### Q: 既存の Issue には反映されますか？
**A:** いいえ。ワークフローは `on: issues: types: [opened]` でトリガーされるため、新しく作成された Issue のみが対象です。

既存の Issue に適用する場合:
1. Issue を閉じてから再度開く（`reopened` トリガーを追加する必要があります）
2. または手動でプロジェクトのフィールドを設定

### Q: フィールド名を変更できますか？
**A:** はい。`.github/workflows/add_to_project.yml` の該当箇所を編集してください。

例: "ProgramTeam" → "開発チーム"
```javascript
await setSingleSelectField('開発チーム', process.env.TEAM);
```

### Q: 新しいフィールドを追加できますか？
**A:** はい。以下の手順で追加できます：

1. Issue テンプレートにフィールドを追加
2. ワークフローの `Extract Issue Fields` ステップで抽出ロジックを追加
3. `Set Project Fields` ステップで設定ロジックを追加

詳細は `.github/docs/PROJECT_AUTOMATION_SETUP.md` を参照。

---

## ?? その他のドキュメント

詳細な説明: [PROJECT_AUTOMATION_SETUP.md](.github/docs/PROJECT_AUTOMATION_SETUP.md)

**問題が解決しない場合は、Actions のログ全体をコピーして Issue を作成してください！**
