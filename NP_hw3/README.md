# 介紹

### 新增寄信和收信的功能

`mail-to <username> --subject <subject> --content <content>`

`list-mail`

`retr-mail <mail#>`

`delete-mail <mail#>`

----------------------------------------

### 另外使用AWS的S3來儲存資料

每個已註冊的用戶都會在s3開啟一個bucket

該bucket用來儲存該用戶的post(包括content, comment)，和mail

--------------------------------------------

### 測試方法如下

`./server [port]`

`./test.sh [ip] [port]`

e.g.

`./server 1234`

`./tesh.sh localhost 1234`

---------------------------------------------

####  ~~server是用C寫的，但client因為會用到s3的bucket相關指令，所以用python寫比較方便XD
-----------------------------------------------

### 新增測試資料夾

測試檔案在nctu_nphw3_demo資料夾

裡面也有readme，就不詳細說明了
