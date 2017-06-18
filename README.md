交通大學選課系統API(偽) NCTU course system fake API
==

- 用爬蟲跟一點黑魔法做的偽 API
- 還在開發階段 API可能會持續修改
- 基本上比較像是選課系統的一個前端?

## dependency
- urllib
- beautifulsoup
- flask
    - flask_cache
    - flask_session
    - flask_restful

## how to use
```
# use python 3.6
python server.py
# open browser and goto localhost:9487
```

## API
#### 只有在選課期間能加選/退選課程，使用的都是健康安全的方法，請不要心存僥倖
- POST /api/fake_login
    - 顧名思義會幫你登入系統並保留 session
    - form fields(2)
        - id: 學號
        - password: 選課系統密碼

- GET  /api/course/
    - 獲得所有已選課程
    - return: 所有已選課程json
- POST /api/course/
    -  fields(2)
        - CrsId: 課號
        - CrsType: 神秘數字(?)
        - priority: 志願序
    - return: 選課是否成功，成功會有課程資訊
- PUT  /api/course/
    -  fields(2)
        - CrsId: 課號
        - CrsType: 神秘數字
    - return: 退選是否成功，成功會有課號

## CrsType
```
不同種類的課的選課神秘數字
目前整理出來的規則:
大學選修 0
大學共同必修 5
研究所 X
通識 7
體育 9
...等等，需要整理
```

## TODO
- HTTPS
- session type (filesystem -> redis?)
- safecode cracker improvement
- JWT auth
- CrsType
    

