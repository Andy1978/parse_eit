.PHONY: all clean

CFLAGS:= -Wall -Wextra -fsanitize=address -O0 -ggdb

all: parse_eit en_300468v011601a.pdf

parse_eit: parse_eit.c
	$(CC) $(CFLAGS) $< -o $@

# V1.16.1 2019-05
en_300468v011601a.pdf:
	wget https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.16.01_20/en_300468v011601a.pdf

# V1.12.1 2011-06
#en_300468v011201o.pdf:
#	wget https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.12.01_40/en_300468v011201o.pdf

clean:
	rm -f parse_eit
