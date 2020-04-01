profile: bin
	perf record -o - -g -- build/src/igclib xc --flight tmp/flights/fai_30602.igc | perf script | c++filt | gprof2dot -f perf | dot -Tsvg -o tmp/profile.svg

bin:
	cd build && make