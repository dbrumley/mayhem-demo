FROM debian:stable-slim
RUN apt-get update && apt-get install -y --no-install-recommends build-essential libc6-dbg curl netcat-traditional

# Copy in source code
WORKDIR /app
COPY Makefile Makefile
COPY src src

# Binary build
RUN make

# Some example initial GPS data and tests
COPY testsuite testsuite
COPY gps_data.txt gps_data.txt

# REMOVEME
COPY crashers crashers

CMD ["/app/src/wait-for-it.sh", "api", "--", "make",  "upload"]
