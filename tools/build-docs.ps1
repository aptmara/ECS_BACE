# tools/build-docs.ps1
# Doxygen → LaTeX→PDF(xelatex) → 出力集約 を自動実行
# 使い方: pwsh -NoProfile -ExecutionPolicy Bypass -File "$(ProjectDir)tools\build-docs.ps1" -ProjectDir "$(ProjectDir)" -SolutionDir "$(SolutionDir)"

param(
  [string]$ProjectDir = ".",
  [string]$SolutionDir = ".",
  [string]$Doxyfile   = "$ProjectDir\Doxyfile"
)

$ErrorActionPreference = "Stop"

function Resolve-Exe([string]$name, [string[]]$candidates){
  $exe = $null
  try { $exe = (Get-Command $name -ErrorAction Stop).Source } catch {}
  if (-not $exe) { foreach($c in $candidates){ if(Test-Path $c){ $exe = $c; break } } }
  if (-not $exe) { throw "依存コマンドが見つからない: $name" }
  return $exe
}
function Run-External([string]$exe, [string[]]$argv){
  Write-Host ">>> $exe $($argv -join ' ')"
  & $exe @argv
  if ($LASTEXITCODE -ne 0) {
    throw "外部コマンドが失敗: $exe (exit=$LASTEXITCODE)"
  }
}

# 依存コマンド検出
$doxygen  = Resolve-Exe "doxygen" @("C:\Program Files\doxygen\bin\doxygen.exe")
$dot      = Resolve-Exe "dot" @("C:\Program Files\Graphviz\bin\dot.exe")
$xelatex  = Resolve-Exe "xelatex" @(
  "C:\Program Files\MiKTeX\miktex\bin\x64\xelatex.exe",
  "$env:LOCALAPPDATA\Programs\MiKTeX\miktex\bin\x64\xelatex.exe"
)
$makeindex = Resolve-Exe "makeindex" @(
  "C:\Program Files\MiKTeX\miktex\bin\x64\makeindex.exe",
  "$env:LOCALAPPDATA\Programs\MiKTeX\miktex\bin\x64\makeindex.exe"
)

# パス解決
$proj = (Resolve-Path $ProjectDir).Path
$sol  = (Resolve-Path $SolutionDir).Path
$doxy = (Resolve-Path $Doxyfile).Path

Write-Host "ProjectDir = $proj"
Write-Host "SolutionDir = $sol"
Write-Host "Doxyfile   = $doxy"
Write-Host "doxygen    = $doxygen"
Write-Host "dot        = $dot"
Write-Host "xelatex    = $xelatex"
Write-Host "makeindex  = $makeindex"

# 1) doxygen 実行
Push-Location (Split-Path $doxy -Parent)
try {
  Write-Host "=== Running Doxygen ==="
  Run-External $doxygen @("$doxy")
} finally { Pop-Location }

# 2) LaTeX ディレクトリ確認
$latexDir  = Join-Path $proj "docs\latex"
$refmanTex = Join-Path $latexDir "refman.tex"
if (!(Test-Path $refmanTex)) {
  throw "LaTeX ソースが無い: $refmanTex。Doxyfile の GENERATE_LATEX / LATEX_OUTPUT / LATEX_HEADER を確認しろ。"
}

Write-Host "=== LaTeX files ready ==="

# 3) LaTeX → PDF
Push-Location $latexDir
try {
  Write-Host "=== Building PDF with xelatex ==="
  # 1st pass (エラーを無視してPDF生成を試みる)
  & $xelatex @("-interaction=nonstopmode","-file-line-error","refman.tex")
  
  # 索引
  if (Test-Path "refman.idx") { & $makeindex @("refman.idx") 2>$null }
  if (Test-Path "refman.glo") {
    if (Test-Path "refman.ist") { & $makeindex @("-s","refman.ist","-o","refman.gls","refman.glo") 2>$null }
    else { & $makeindex @("refman.glo") 2>$null }
  }
  
  # 2nd / 3rd pass
  & $xelatex @("-interaction=nonstopmode","-file-line-error","refman.tex")
  & $xelatex @("-interaction=nonstopmode","-file-line-error","refman.tex")

  if (!(Test-Path "refman.pdf")) { 
    Write-Host "警告: PDF 生成中にエラーがありましたが、ログを確認してください: refman.log" -ForegroundColor Yellow
    throw "PDF 生成に失敗: refman.pdf が無い" 
  }
  
  Write-Host "PDF生成成功（警告がある場合はrefman.logを確認）" -ForegroundColor Green

  # 中間ファイル掃除（必要ならコメントアウト）
  Get-ChildItem -Include *.aux,*.toc,*.out,*.log,*.lot,*.lof,*.idx,*.ilg,*.ind,*.gl*,*.synctex.gz -File -ErrorAction SilentlyContinue | Remove-Item -Force
}
finally { Pop-Location }

# 4) 成果物集約
$finalDocs = Join-Path $proj "docs\final"
$srcHtml = Join-Path $proj "docs\html"
$dstHtml = Join-Path $finalDocs "html"
$srcPdf  = Join-Path $latexDir "refman.pdf"
$dstPdf  = Join-Path $finalDocs "refman.pdf"

New-Item -ItemType Directory -Force -Path $finalDocs | Out-Null
if (Test-Path $dstHtml) { Remove-Item $dstHtml -Recurse -Force }
if (Test-Path $srcHtml) {
  Write-Host "=== Sync HTML ==="
  Copy-Item $srcHtml $dstHtml -Recurse
}
Copy-Item $srcPdf $dstPdf -Force

Write-Host "=== Done ==="
Write-Host "HTML: $dstHtml\index.html"
Write-Host "PDF : $dstPdf"
