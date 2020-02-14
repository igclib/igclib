![igclib banner](https://raw.githubusercontent.com/igclib/assets/master/img/banner/igclib_banner.svg?sanitize=true)

A library for paragliding competitions and XC

## Features

- Efficient airspace checking against OpenAir files

## Future features

- XC score computation
- Task validation
- Race scoring

## Get started

Before building the project, make sure you have installed all the [requirements](#requirements).

```
https://github.com/igclib/igclib.git
cd igclib
mkdir build

cmake -S . -B build
cmake --build build
```

## Requirements

- [Boost](https://www.boost.org/)
- [GeographicLib](https://geographiclib.sourceforge.io/html/index.html)
