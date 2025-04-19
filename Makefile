.PHONY: all fetcher parser clean0
all:

fetcher:
	g++ -std=c++17 mhwilds_fetcher.cpp -lcpr -lcurl -lssl -lcrypto -o fetcher && ./fetcher

parser:
	g++ -std=c++17 mhwilds_parser.cpp -o parser && ./parser

clean:
	rm	fetcher parser