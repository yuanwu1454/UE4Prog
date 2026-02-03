:: 关键：获取当前脚本所在目录（将工作目录切换到脚本目录，只需把脚本放在 MyGame.uproject 同目录即可）
:: 前提条件将 UE4Editor-Cmd.exe 设置到系统变量里面
set "PROJECT_FULL_PATH=%~dp0MyGame.uproject"
UE4Editor-Cmd.exe %PROJECT_FULL_PATH%  -game -windowed -ResX=1280 -ResY=720