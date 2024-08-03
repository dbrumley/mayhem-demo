FROM gcc:latest
RUN apt-get update && apt-get install -y --no-install-recommends build-essential curl netcat-traditional libc6-dbg

# Copy in source code
WORKDIR /app
COPY src src

# Binary build
RUN gcc -o gps_uploader -g -fsanitize=undefined -fno-sanitize-recover src/gps_uploader.c
RUN gcc -o gps_uploader_gcov -g -fprofile-arcs -ftest-coverage src/gps_uploader.c

# Some demo example GPS data
COPY testsuite testsuite

CMD ["/app/src/wait-for-it.sh", "api", "--", "/app/gps_uploader", "/testsuite/gps_data.txt", "http://api:8000/location"]
