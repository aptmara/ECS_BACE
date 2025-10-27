# ?? 修正完了 - プロジェクト自動化

## 問題

Issue を作成してプロジェクトに追加されても、**Role、Team、Priority などのカスタムフィールドが空のまま**だった。

---

## 解決内容

### ? 修正されたファイル

1. **`.github/workflows/add_to_project.yml`**
   - Issue テンプレートからフィールドを自動抽出
   - GraphQL API でプロジェクトのフィールドIDを動的に取得
   - 各フィールドを自動設定（Role, ProgramTeam, Priority, Component, Size, Estimate, Start/End date）
   - 詳細なデバッグログ出力

2. **`.github/ISSUE_TEMPLATE/01_task.yml`**
   - プロジェクトのフィールド名に合わせて更新
   - `Team` → `ProgramTeam`
   - `Component` をドロップダウンに変更
   - `Size` と `Estimate` フィールドを追加

3. **`.github/ISSUE_TEMPLATE/02_bug.yml`**
   - Bug にもプロジェクトフィールドを追加

4. **ドキュメント**
   - `QUICKSTART_PROJECT_AUTOMATION.md` - すぐに使える簡易ガイド
   - `.github/docs/PROJECT_AUTOMATION_SETUP.md` - 詳細セットアップガイド
   - `.github/scripts/get_project_fields.ps1` - フィールドID取得スクリプト

---

## ?? 次のステップ

### 1. コミット & プッシュ

変更をコミットしてプッシュしてください:

```bash
git add .
git commit -m "fix: プロジェクト自動化のフィールド自動設定を実装"
git push
```

### 2. テスト Issue を作成

1. GitHub で新しい Issue を作成
2. **Task** テンプレートを選択
3. 以下を入力:
   ```
   概要: テスト用タスク
   Role: Programmer
   ProgramTeam: A
   Priority: High
   Component: Core
   Size: M
   Estimate: 2
   Start date: 2025-01-15
   End date: 2025-01-20
   ```
4. **Submit new issue**

### 3. 確認

#### A. Actions タブで確認
1. Repository → **Actions**
2. "Auto Add Issue to Project" ワークフロー
3. ログを確認:
   ```
   === Extracted Fields ===
   Role: Programmer
   Team: A
   Priority: High
   ...
   
   ? Set Role: Programmer
   ? Set ProgramTeam: A
   ? Set Priority: High
   ? Set Component: Core
   ? Set Size: M
   ? Set Estimate: 2
   ? Set Start date: 2025-01-15
   ? Set End date: 2025-01-20
   ? All fields processed
   ```

#### B. プロジェクトボードで確認
1. Projects → HEW project
2. 作成した Issue を確認
3. 以下のフィールドが自動設定されているはず:
   - ? Status: (デフォルト値)
   - ? Role: Programmer
   - ? ProgramTeam: A
   - ? Priority: High
   - ? Component: Core
   - ? Size: M
   - ? Estimate: 2
   - ? Start date: 2025-01-15
   - ? End date: 2025-01-20

---

## ?? 技術的な変更点

### 従来の方法（失敗していた理由）

```yaml
# actions/add-to-project@v0.5.0 の outputs.itemId が取得できていなかった
- name: Set Project Fields
  if: steps.add-project.outputs.itemId  # ← これが空だった
```

### 新しい方法（修正版）

```yaml
# GraphQL API で直接プロジェクトとアイテムIDを取得
- name: Get Project Data
  script: |
    const item = project.items.nodes.find(
      node => node.content && node.content.number === issue.number
    );
    core.setOutput('item_id', item.id);
```

**利点:**
- ? `itemId` を確実に取得
- ? プロジェクトのフィールド情報も同時に取得
- ? フィールドIDやオプションIDを手動設定する必要がない
- ? プロジェクトの構成が変わっても自動的に追従

---

## ?? 対応フィールド一覧

| Issue フィールド | プロジェクトフィールド | タイプ | 必須 |
|----------------|-------------------|--------|-----|
| Role | Role | Single Select | ? |
| ProgramTeam | ProgramTeam | Single Select | - |
| Priority | Priority | Single Select | ? |
| Component | Component | Single Select | - |
| Size | Size | Single Select | - |
| Estimate | Estimate | Number | - |
| Start date | Start date | Date | - |
| End date | End date | Date | - |

---

## ?? 注意事項

### プロジェクトのフィールド名と完全一致が必要

ワークフローは以下のフィールド名を想定しています:

```javascript
await setSingleSelectField('Role', process.env.ROLE);
await setSingleSelectField('ProgramTeam', process.env.TEAM);
await setSingleSelectField('Priority', process.env.PRIORITY);
await setSingleSelectField('Component', process.env.COMPONENT);
await setSingleSelectField('Size', process.env.SIZE);
await setDateField('Start date', process.env.START_DATE);
await setDateField('End date', process.env.DUE_DATE);
await setNumberField('Estimate', process.env.ESTIMATE);
```

**プロジェクトのフィールド名が異なる場合は、ワークフローを編集してください。**

### オプション値も完全一致が必要

Issue テンプレートのドロップダウンの値と、プロジェクトのオプション値が一致している必要があります。

例:
- Issue: `Programmer` ← ?
- Project: `Programmer` ← ? 一致

不一致の場合:
- Issue: `プログラマー` ← ?
- Project: `Programmer` ← ? 設定されない

**ログで確認:**
```
Warning: Option "プログラマー" not found for field "Role"
Available options: ["Programmer", "Designer", "Planner", "QA"]
```

---

## ?? トラブルシューティング

### フィールドが設定されない

**チェックリスト:**
1. [ ] Actions でワークフローが成功している（緑チェック）
2. [ ] "Debug - Show extracted fields" でフィールドが抽出されている
3. [ ] "Set Project Fields" で `? Set Role:` などのログが出ている
4. [ ] プロジェクトのフィールド名が一致している
5. [ ] オプションの値が一致している
6. [ ] トークン `ORG_PROJECTS_TOKEN` に `project` 権限がある

### ワークフローが失敗する

**よくあるエラー:**

1. **"Could not find issue in project"**
   - Issue がまだプロジェクトに追加されていない
   - `actions/add-to-project` が失敗している可能性
   - トークンの権限を確認

2. **"Field not found in project"**
   - プロジェクトに該当するフィールドが存在しない
   - フィールド名のスペルミスを確認

3. **"Option not found for field"**
   - Issue で選択した値がプロジェクトのオプションに存在しない
   - ログの "Available options" を確認して修正

---

## ?? 今後の拡張

### 新しいフィールドを追加する場合

1. **プロジェクトにフィールドを追加**
2. **Issue テンプレートに追加**
   ```yaml
   - type: dropdown
     id: new_field
     attributes:
       label: NewField
       options: [Option1, Option2, Option3]
   ```
3. **ワークフローで抽出**
   ```javascript
   const newFieldMatch = issueBody.match(/### NewField\s*\n+\s*(.+?)(?:\n|$)/i);
   core.setOutput('new_field', newFieldMatch ? newFieldMatch[1].trim() : '');
   ```
4. **ワークフローで設定**
   ```javascript
   await setSingleSelectField('NewField', process.env.NEW_FIELD);
   ```

---

## ? 完了！

すべての変更が完了しました。新しい Issue を作成してテストしてください！

問題があれば `QUICKSTART_PROJECT_AUTOMATION.md` のトラブルシューティングセクションを参照してください。
