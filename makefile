profile: bin
	perf record -o - -g -- build/src/igclib xc --flight tmp/flights/26282.txt | perf script | c++filt | gprof2dot -f perf | dot -Tsvg -o tmp/profile.svg

bin:
	cd build && make