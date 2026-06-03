monitor: monitor.cpp
	g++ -O3 monitor.cpp -o monitor -lncurses
	strip -s monitor

clean:
	rm -f monitor

.PHONY: clean