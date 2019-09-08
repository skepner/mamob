# Eugene Skepner <eu.skepner at skepner.eu>

all: mamob

mamob: mamob.cc
	g++ -Weverything -Wno-c++98-compat -O3 -std=c++2a  -o $@ $^ -F/System/Library/PrivateFrameworks -framework MultitouchSupport -framework ApplicationServices 

clean:
	rm mamob
