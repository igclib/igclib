.PHONY:xc

# All commentend cmake instructions are available for cmake >= 3.14, not default on Ubuntu LTS
debug:build-dir
	#cmake -S . -B build
	#cmake --build build -j $(nproc)
	cd build && cmake .. && make -j $(nproc)

xc:
	build/src/igclib xc --flight tests/data/flights/xc_col_agnel.igc --airspace tests/data/airspace/france_08_19.txt

profile: debug
	perf record -o - -g -- build/src/igclib xc --flight tmp/flights/fai_30602.igc | perf script | c++filt | gprof2dot -f perf | dot -Tsvg -o tmp/profile.svg
	
release:build-dir
	#cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	#cmake --build build -j $(nproc)
	cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)

install:release
	#cmake --install build
	cd build && make install


build-dir:
	mkdir -p build

memory-check:
	valgrind -s --track-origins=yes build/src/igclib xc --flight tests/data/flights/xc_col_agnel.igc --airspace tests/data/airspace/france_08_19.txt > /dev/null