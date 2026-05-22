#  当脚本中的任何命令返回非零退出状态（即失败）时，脚本会立即退出，避免继续执行后续命令。
set -e
 
cmake -B build/
cmake --build build/

cp -r ./src/include ./lib