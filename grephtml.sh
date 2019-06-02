grep -Eoi '<a [^>]+>' temp.txt | grep -Eo 'href="[^"]+"' |  grep -Eo '(http|https)://[^"]+' > links.txt
