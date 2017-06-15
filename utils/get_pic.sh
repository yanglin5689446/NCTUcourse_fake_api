#!/usr/local/bin/fish
for x in (seq 10)
    curl -G https://course.nctu.edu.tw/function/Safecode.asp > $x.bmp
end 
