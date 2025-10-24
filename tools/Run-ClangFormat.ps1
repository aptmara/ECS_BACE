[CmdletBinding()]
param(
    [switch]$Staged,
    [string[]]$Files,
    [switch]$Check
)

$ErrorActionPreference = 'Stop'

function Find-ClangFormat {
    $cmd = Get-Command clang-format -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Path
    }

    $candidates = @(
        "$env:ProgramFiles\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\clang-format.exe",
        "$env:ProgramFiles\Microsoft Visual Studio\2022\Professional\VC\Tools\Llvm\x64\bin\clang-format.exe",
        "$env:ProgramFiles\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe",
        "$env:ProgramFiles(x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\Llvm\x64\bin\clang-format.exe",
        "$env:ProgramFiles\LLVM\bin\clang-format.exe"
    )

    foreach ($path in $candidates) {
        if ($path -and (Test-Path $path)) {
            return $path
        }
    }

    throw "clang-format.exe が見つかりませんでした。PATH へ追加するか、Visual Studio/LLVM をインストールしてください。"
}

function GetCandidateFiles {
    param(
        [switch]$Staged,
        [string[]]$Files
    )

    if ($Files) {
        return $Files
    }

    $extensions = '\.(c|cc|cpp|cxx|h|hh|hpp|hxx|inl|ipp)$'

    if ($Staged) {
        $staged = git diff --name-only --cached --diff-filter=ACMR
        return $staged | Where-Object { $_ -match $extensions }
    }

    $tracked = git ls-files
    return $tracked | Where-Object { $_ -match $extensions }
}

$clangFormat = Find-ClangFormat

$targetFiles = GetCandidateFiles -Staged:$Staged -Files:$Files | Sort-Object -Unique
$targetFiles = $targetFiles | ForEach-Object {
    if (-not $_) { return }
    $normalized = $_ -replace '/', [IO.Path]::DirectorySeparatorChar
    if (Test-Path $normalized) {
        return $normalized
    }
}

if (-not $targetFiles -or $targetFiles.Count -eq 0) {
    Write-Host "clang-format: 対象ファイルがありません。"
    return
}

foreach ($file in $targetFiles) {
    if ($Check) {
        & $clangFormat --style=file --dry-run --Werror $file | Out-Null
    } else {
        & $clangFormat --style=file -i $file | Out-Null
    }

    if ($LASTEXITCODE -ne 0) {
        throw "clang-format の実行に失敗しました: $file"
    }
}

if ($Check) {
    Write-Host "clang-format: すべて整形済みです。"
} else {
    Write-Host "clang-format: ${($targetFiles.Count)} 件のファイルを整形しました。"
}
