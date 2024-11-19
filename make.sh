./clean.sh
mkdir -p bin
mkdir -p obj
g++ -I./include -Wall -Wextra -std=c++17 -c src/main.cpp -o obj/main.o
g++ -I./include -Wall -Wextra -std=c++17 -c src/utils.cpp -o obj/utils.o
g++ -I./include -Wall -Wextra -std=c++17 -c src/web_crawler.cpp -o obj/web_crawler.o
g++ obj/main.o obj/utils.o obj/web_crawler.o -o bin/web_crawler -lcurl -lgumbo -pthread