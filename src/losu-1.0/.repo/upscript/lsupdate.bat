@echo off
echo "开始下载更新包"
curl -o %LPTROOT%tmp.zip https://losu.tech/archive/lasted/.repo/release/losu_Windows.zip

echo "开始更新"

7z x -y -r "%LPTROOT%tmp.zip" -o"%LPTROOT%"

del "%LPTROOT%tmp.zip"
echo "完成"
del "%LPTROOT%lsupdate.bat"
