# 介紹

### 和上次功能差不多，只是client端多了以下指令

`create-board <name>`

`create-post <board-name> --title <title> --content <content>`

`list-board ##<key>`

`list-post <board-name> ##<key>`

`read <post-id>`

`delete-post <post-id>`

`update-post <post-id> --title/content <new>`

`comment <post-id> <comment>`

### 新增store和load資料功能

在client端輸入

`store`

`load`

store可以將資料寫到data.txt裡面，以防資料遺失

load則將該資料取出，放到server裡面

### 測試檔和之前用一樣的方法即可測試

`test.sh [ip] [port]`