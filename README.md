![igclib banner](https://cdn.jsdelivr.net/gh/igclib/assets@master/img/banner/igclib_banner.svg)

A command-line tool for paragliding competitions and XC.

## Current Features

- Airspace checking of flights against OpenAir files.
- Task validation
- Task optimization

## In Progress

- XC score computation

## Future features

- Race scoring

## Install

Two options :

- Docker `docker run teobouvard/igclib`
- Build from source, see the [install wiki](https://github.com/igclib/igclib/wiki/Install)

## Deploy

```
docker build -f docker/Dockerfile.bare -t teobouvard/igclib:bare .
docker push teobouvard/igclib:bare

docker build -f docker/Dockerfile.node -t teobouvard/igclib:node .
docker push teobouvard/igclib:node
```

## Acknowledgements

This project uses the following libraries.

- [Boost](https://www.boost.org/)
- [GeographicLib](https://geographiclib.sourceforge.io/html/index.html)
- [nlohmann/json](https://github.com/nlohmann/json.git)
