#!/usr/local/bin/fish
# a small piece of fish script to get test safecode pic
for x in (seq 10)
    curl -G https://course.nctu.edu.tw/function/Safecode.asp > $x.bmp
end 
