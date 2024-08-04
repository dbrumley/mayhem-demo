#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

void divide_by_zero(int lat, int lon) {
  volatile int res = 0;
  if(lat == 1)
    res = lat / lon; // Divide by zero when lon = 0
}

void integer_overflow_negative(int lat, int lon){
  // Integer overflow with negative values, difference > INT_MAX
  volatile int res = 0;

  if(lat < 0 && lon == -79927771){
    lat = -lat; 
    printf("Here: %d\n", res);
  }
}

void oob_read(int lat, int lon) {
  volatile char OOBR;
  char line[8];
  strcpy(line, "AAAAAA");
  if (lat == 3 && lon == -79927771) {
    OOBR = line[lat - lon]; // Out of bounds read
  }
}

void oob_write(int lat, int lon) {
  volatile char OOBR;
  char line[8];
  strcpy(line, "AAAAAA");
  if (lat == 4 && lon == -79927771) {
    line[lat - lon] = 'w'; // Out of bounds write
  }
}

void double_free(int lat, int lon) {
  char* buf = malloc(lat);
  free(buf);
  if (lat == 5 && lon == -79927771) // Check to pass
    free(buf); // double free
}

void stack_exhaustion(int lat, int lon) {
  char buff[0x1000];
  if (lat == 6 && lon == -79927771) // Check to get pass
    stack_exhaustion(lat, lon);
}


void vulnerable_c(int bug_num, char* line, int latitude, int longitude) {
  printf("Lat: %d Lon: %d\n", latitude, longitude);
  if(bug_num == 0){
    divide_by_zero(latitude, longitude);
    integer_overflow_negative(latitude, longitude);
    oob_read(latitude, longitude);
    oob_write(latitude, longitude);
    double_free(latitude, longitude);
    stack_exhaustion(latitude, longitude);
  }

  if(bug_num == 1) integer_overflow_negative(latitude, longitude);
  if(bug_num == 2) divide_by_zero(latitude, longitude);
  if(bug_num == 3) oob_read(latitude, longitude);
  if(bug_num == 4) oob_write(latitude, longitude);
  if(bug_num == 5) double_free(latitude, longitude);
  if(bug_num == 6) stack_exhaustion(latitude, longitude);
}


// Function to parse a NMEA line and extract latitude and longitude
int parse_lat_lon(char* line, int* latitude, int* longitude) {
  char* str = NULL;
  char* fields[15];
  int field_count = 0;
  double lat = 1.0, lon = 2.0;

  //str = malloc(strlen(line) + 1);
  //strcpy(str, line);
  str = line; 

  fields[field_count++] = str;
  for (char* ptr = str; *ptr != '\0' && field_count < 15; ptr++) {
    if (*ptr == ',') {
      *ptr = '\0'; // Terminate the current field
      fields[field_count++] = ptr + 1; // Start of the next field
    }
  }
  if (field_count != 15 || atoi(fields[1]) <= 1) {
    //free(str);
    return -1;
  }

  lat = atoi(fields[2]);
  lon = atoi(fields[4]);

  if (fields[3][0] == 'S') lat = -lat;
  if (fields[5][0] == 'W') lon = -lon;


  *latitude = lat;
  *longitude = lon;

  //printf("%d fields parsed\n", field_count);
  //free(str);
  return 0;
} 


int main(int argc, char* argv[])
{
  char line[256];
  int lat = 0, lon = 0;
  double latitude=0.0, longitude=0.0; 
  const char* GPS_FILE_PATH = NULL;
  int fd, in_bytes;

  GPS_FILE_PATH = argv[1];

  if( (fd = open(GPS_FILE_PATH, O_RDONLY)) == -1)
    exit(-1); 

  if( (in_bytes = read(fd, line, sizeof(line)-1)) == -1)
    exit(-1);

  line[in_bytes] = 0;
  parse_lat_lon(line, &lat, &lon);
  vulnerable_c(0, line, lat, lon);

  latitude = (double) lat/ 1000000;
  longitude = (double) lon / 1000000;

  //printf("Here: %lf %lf\n", latitude, longitude);
  return 0;
}



  /* FILE* file = fopen(GPS_FILE_PATH, "r");
  if (file == NULL) {
    perror("Error opening GPS file");
    return 1;
  }

  if(fgets(line, sizeof(line), file) > 0) {
    latitude = 2.0;
    longitude = 1.0;
  }

  fclose(file); */
