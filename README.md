# SysUpdate 🕵️‍♂️

![License](https://img.shields.io/badge/License-Educational%20Only-red)
![Language](https://img.shields.io/badge/Language-C-blue)
![Platform](https://img.shields.io/badge/Platform-Linux-green)


## 🚨 Educational Purpose Only
This project demonstrates advanced Linux system infiltration techniques. **For security research and educational purposes only.**

## 🧩 Overview

SysUpdate is a sophisticated demonstration of the techniques a trojan on Linux would employ, including:

- Covert binary deployment with in-memory packaging
- Persistence through systemd service integration
- Advanced state machine programming
- Obfuscated control flow techniques
- System monitoring capabilities

## 🔧 Components

### Core Modules
- **SysUpdate**: Remote access utility with shell functionality
- **SysInput**: Keyboard monitoring and input diagnostics

### Technical Features
- Randomized deployment sequence to evade detection
- Flattened state machine architecture (28 distinct states)
- Self-contained binary deployment
- Hidden services with systemd persistence
- Perfect for studying advanced C state machines and system integration

## 📊 Architecture

```
┌─────────────────┐     ┌───────────────────┐
│   State Machine │────▶│  Binary Deployment │
└─────────────────┘     └───────────────────┘
         │                        │
         ▼                        ▼
┌─────────────────┐     ┌───────────────────┐
│ Service Creation│◀────│ Execution Handler │
└─────────────────┘     └───────────────────┘
         │
         ▼
┌─────────────────┐
│  Service Launch │
└─────────────────┘
```

## ⚙️ Implementation

The project uses a flattened state machine with conditional execution paths. The state mappings are as follows:

```c
#define STATE_0xF73A91C5 0
#define STATE_0x8D2E47B0 1
#define STATE_0xC691F4D3 2
#define STATE_0x5AE28D07 3
#define STATE_0xB3C7D059 4
#define STATE_0x4F1E68A9 5
// ... more states
```
A full mapping of all machine states can be found in `/src/state_mapping.md`
Deployment is randomized for improved evasion:
```c
// Shuffle actions
srand(time(NULL));
for (int i = 2; i > 0; i--) {
  int j = rand() % (i + 1);
  int temp = action_indices[i];
  action_indices[i] = action_indices[j];
  action_indices[j] = temp;
}
```
## Flattened Structure
![image](https://github.com/user-attachments/assets/690d557b-82b0-4531-9850-cd3dbdab329a)

## 🔍 Usage Warning

This demonstration requires root privileges. **Only use on systems you own or have explicit permission to test**.

## ⏳ Project Status

This project is currently **under development** and not finished. Future work includes:

- [ ] Include routine if executed without root privileges
- [ ] Additional evasion techniques
- [ ] Cleanup routines

## 🤝 Contribution

No contributions are being accepted as this is a research demonstration project.

## 📝 License

**Educational and Research Purposes Only**. Not for deployment on unauthorized systems.

---

⚠️ This software demonstrates techniques that could be misused. It is shared solely for educational purposes to understand system security vulnerabilities and detection methods.
