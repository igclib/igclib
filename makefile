# All commentend cmake instructions are available for cmake >= 3.14, not default on Ubuntu LTS

profile: debug
	perf record -o - -g -- build/src/igclib xc --flight tmp/flights/fai_30602.igc | perf script | c++filt | gprof2dot -f perf | dot -Tsvg -o tmp/profile.svg

debug:build-dir
	#cmake -S . -B build
	#cmake --build build -j $(nproc)
	cd build && cmake .. && make -j $(nproc)

	
release:build-dir
	#cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	#cmake --build build -j $(nproc)
	cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)

install:release
	#cmake --install build
	cd build && make install


build-dir:
	mkdir -p build