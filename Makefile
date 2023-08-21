.PHONY: all dist check style clean

#CC=arm-linux-gnueabihf-gcc

CFLAGS:= -Wall -Wextra -fsanitize=address -O0 -ggdb
#CFLAGS:= -Wall -Wextra

TARGETS= parse_eit

all: $(TARGETS) en_300468v011601a.pdf

parse_eit: parse_eit.c
	$(CC) $(CFLAGS) $< -o $@

dist: $(TARGETS)
	scp $^ root@dm900:/root

# V1.16.1 2019-05
en_300468v011601a.pdf:
	wget https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.16.01_20/en_300468v011601a.pdf

# V1.12.1 2011-06
#en_300468v011201o.pdf:
#	wget https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.12.01_40/en_300468v011201o.pdf

check:
	cppcheck -q --enable=all --language=c parse_eit.c

style:
	find . \( -name "*.m" -or -name "*.c" -or -name "*.cc" -or -name "*.cc" -or -name "*.h" -or -name "Makefile" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
	find . \( -name "*.c" -or -name "*.cc" -or -name "*.h" \) -exec astyle --style=gnu -s2 -n {} \;

clean:
	rm -f $(TARGETS)
