# Масове перекодування файлів у UTF-8 з BOM
# Запускати у корені проекту

Get-ChildItem -Recurse -Include *.cpp,*.h,*.txt,*.log,*.cnf,*.ps1 | ForEach-Object {
    Write-Host "Конвертую файл:" $_.FullName
    $content = Get-Content $_.FullName
    $utf8bom = New-Object System.Text.UTF8Encoding($true)  # $true = з BOM
    [System.IO.File]::WriteAllLines($_.FullName, $content, $utf8bom)
}
