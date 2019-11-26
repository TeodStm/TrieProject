all: minisearch

minisearch:
		gcc -o minisearch stackType.c trie.c -lm main.c

clean:
		rm -rf *o minisearch
