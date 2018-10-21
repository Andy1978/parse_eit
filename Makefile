.PHONY=all
.PHONY=clean

all: parse_eit

parse_eit: parse_eit.h parse_eit.c
	gcc -Wall -Wextra -ggdb parse_eit.c -o parse_eit

en_300468v011201o.pdf:
	wget https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.12.01_40/en_300468v011201o.pdf

clean:
	rm -f parse_eit
