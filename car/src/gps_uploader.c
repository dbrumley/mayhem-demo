#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Damn vulnerable C stack exhaustion
void stack_exhaustion() {
  char buff[0x1000];
  while (1) {
    stack_exhaustion();
  }
}

void vulnerable_c(char* line, int latitude, int longitude, double time) {
  int size1, size2, size4;
  unsigned int size3;

  if (time <= 1.0) {
    printf("Skipping record with invalid time.\n");
    return;
  }

  size1 = latitude - longitude; // integer overflow with negative values, difference > INT_MAX

  if (latitude < 0) {
    latitude = -latitude; // integer overflow when latitude == INT_MIN
  }

  size2 = latitude / longitude; // divide by zero

  size3 = (unsigned int)latitude + longitude; // integer overflow when sum > INT_MAX

  if (size3 == 1000) // Stack exhausion
    stack_exhaustion();

  size4 = (long long)size3 + strlen(line);

  char* buff1 = (char*)malloc(size4); // Fails when size4 < 0

  // Out of bounds write (heap overflow)
  // when size4 < strlen(line) or <0
  strcpy(buff1, line);

  volatile char OOBR = buff1[strlen(line) + 1]; // Out of bounds read

  free(buff1);

  if (size2 > 0 && size2 % 7 == 0) {
    free(buff1); // Double Free = Free of Memory Not on Heap
  }
  else {
    if (size2 > 0 && size2 % 11 == 0) {
      buff1[0] = 'a'; // Out of bounds write b.c. use-after-free
    }
  }

}

// Function to parse a NMEA line and extract latitude and longitude
int parseLatLon(char* line, double* time, double* latitude, double* longitude) {
  int i = 0;
  char* token;
  *time = 0.0;
  *latitude = 0.0;
  *longitude = 0.0;

  token = strtok(line, ",");
  if (token == NULL) return;

  // Check message is of assumed message type for our receiver
  if (strncmp(token, "$GPRMC", 6) == 0) {
    // Read pos status field. 
    token = strtok(NULL, ",");
    if (token != NULL)
      *time = atof(token);

    token = strtok(NULL, ","); // pos_status field. 
    if (token != NULL && token[0] != 'A') {
      printf("Skipping record with invalid position.\n");
      return 1;
    }
  }
  else if (strncmp(token, "$GPGGA", 6) == 0) {
    token = strtok(NULL, ",");
    if (token != NULL)
      *time = atof(token);
    // There is no pos status field; continue on. 
  }
  else {
    printf("Unsupported format line: %s\n", line);
    return 1;
  }


  while (token != NULL) {
    switch (i) {
    case 1: // latitude
      *latitude = atof(token) / 100.0;
      break;
    case 2: // latitude direction (N/S)
      if (token[0] == 'S')
        *latitude = -(*latitude);
      break;
    case 3: // longitude
      *longitude = atof(token) / 100.0;
      break;
    case 4: // longitude direction (E/W)
      if (token[0] == 'W')
        *longitude = -(*longitude);
      break;
    }
    token = strtok(NULL, ",");
    i++;
  }

  return 0;
}

// Function to upload position data to API server
void upload_position(const char* URL, double latitude, double longitude)
{
  char command[256];
  if (URL == NULL) {
    printf("No server to upload latitude: %f, longitude: %f\n", latitude, longitude);
    return;
  }

  snprintf(command, sizeof(command),
    "curl -X POST -H \"Content-Type: application/json\"  -u me@me.com:123456 -d '{\"latitude\": %.6f, \"longitude\": %.6f}' %s",
    latitude, longitude, URL);

  system(command);
}


int main(int argc, char* argv[])
{
  char line[256];
  const char* GPS_FILE_PATH = NULL;
  const char* URL = NULL;

  double latitude = 0.0, longitude = 0.0, time = 0.0;

  if (argc != 2 && argc != 3) {
    printf("Usage: %s <nmea_data_file> <URL>\n", argv[0]);
    return 1;
  }

  GPS_FILE_PATH = argv[1];

  if (argc == 3) {
    URL = argv[2];
  }

  FILE* file = fopen(GPS_FILE_PATH, "r");
  if (file == NULL) {
    perror("Error opening GPS file");
    return 1;
  }

  while (fgets(line, sizeof(line), file)) {
    latitude = 0.0;
    longitude = 0.0;
    time = 0.0;

    if (parseLatLon(line, &time, &latitude, &longitude) != 0) {
      printf("Invalid record, continuing\n");
      continue;
    }

    // Example vulnerable C 
    // Ported from https://github.com/hardik05/Damn_Vulnerable_C_Program/blob/master/imgRead.c
    vulnerable_c(line, (int)latitude, (int)longitude, time);

    upload_position(URL, latitude, longitude);
  }

  fclose(file);
  return 0;
}
