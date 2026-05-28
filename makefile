monitor: monitor.cpp
	g++ -O3 -static monitor.cpp -o monitor
	strip -s monitor

clean:
	rm -f monitor

.PHONY: clean
