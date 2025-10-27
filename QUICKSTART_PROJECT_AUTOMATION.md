# プロジェクト自動化 - クイックスタート

## 現在の問題
? Issue がプロジェクトに追加される
? **Role, Team, Priority などのフィールドが空のまま**

## 修正内容
? Issue テンプレートから情報を自動抽出
? プロジェクトのカスタムフィールドに自動設定
? 手動入力不要

---

## ?? すぐに実行する手順

### 1?? フィールドIDを取得（1回のみ）

```powershell
# PowerShell で実行
.\.github\scripts\get_project_fields.ps1 -Token "YOUR_TOKEN_HERE"
```

**トークンの取得方法:**
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. "Generate new token (classic)" をクリック
3. 権限を選択: `repo`, `project`
4. トークンをコピー

### 2?? ワークフローを更新

出力された情報を `.github/workflows/add_to_project.yml` にコピー＆ペースト：

```javascript
// この部分を更新（61行目あたり）
const projectId = 'PVT_kwHOAKhcas4AvQ1B'; // ← 実際のProject IDに置き換え

const fieldIds = {
  role: 'PVTSSF_lAHO...', // ← 実際のField IDに置き換え
  team: 'PVTSSF_lAHO...',
  // ...
};

const fieldValueIds = {
  role: {
    'Planner': 'xxxxx',  // ← 実際のOption IDに置き換え
    'Programmer': 'yyyy',
    // ...
  },
  // ...
};
```

### 3?? テスト

1. 新しい Task Issue を作成
2. Role, Team, Priority を選択
3. Submit
4. プロジェクトボードで確認 → **自動的にフィールドが設定される！**

---

## ?? 対応フィールド

| Issue テンプレート | プロジェクトフィールド |
|------------------|-------------------|
| Role (dropdown) | Role |
| Team (dropdown) | Team |
| Priority (dropdown) | Priority |
| Start date (input) | Start date |
| End date (input) | End date |
| Component (input) | Component (任意) |

---

## ?? トラブルシューティング

### エラー: "Field not updated"
→ `.github/workflows/add_to_project.yml` の ID が間違っている
→ 再度 `get_project_fields.ps1` を実行して最新のIDを取得

### フィールドが空のまま
→ Actions タブでワークフローログを確認
→ トークンの権限を確認（`project` 権限が必要）

### 日付が反映されない
→ YYYY-MM-DD 形式で入力されているか確認（例: 2025-11-15）

---

## ?? その他のドキュメント

詳細な説明: [PROJECT_AUTOMATION_SETUP.md](.github/docs/PROJECT_AUTOMATION_SETUP.md)
