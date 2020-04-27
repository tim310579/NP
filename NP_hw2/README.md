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

### 另外有寫一個測試檔，和之前用一樣的方法即可測試

`test.sh [ip] [port]`
