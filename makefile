.PHONY:xc

# All commentend cmake instructions are available for cmake >= 3.14, not default on Ubuntu LTS
release:build-dir
	#cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	#cmake --build build -j $(nproc)
	cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)

install:release
	#cmake --install build
	cd build && make install


build-dir:
	mkdir -p build

### DEV TARGETS ###

debug:build-dir
	#cmake -S . -B build
	#cmake --build build -j $(nproc)
	cd build && cmake .. && make -j $(nproc)

xc:
	build/src/igclib xc --flight tests/data/flight/xc_col_agnel.igc --airspace tests/data/airspace/france_08_19.txt

profile-perf: debug
	perf record -o - -g -- build/src/igclib xc --flight tests/data/flight/xc_col_agnel.igc --airspace tests/data/airspace/france_08_19.txt | perf script | c++filt | gprof2dot -f perf | dot -Tsvg -o tmp/profile.svg

profile-memory: debug
	valgrind --tool=massif --stacks=yes --massif-out-file=tmp/memory.out build/src/igclib xc --flight tests/data/flight/xc_col_agnel.igc -v --airspace tests/data/airspace/france_08_19.txt
	massif-visualizer tmp/memory.out

memory-check:
	valgrind -s --track-origins=yes build/src/igclib xc --flight tests/data/flight/xc_col_agnel.igc --airspace tests/data/airspace/france_08_19.txt > /dev/null

