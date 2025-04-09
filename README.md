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

The project uses a flattened state machine with conditional execution paths:

```c
#define STATE_START 0
#define STATE_CHECK_ROOT 1
#define STATE_SETUP_ACTIONS 2
// ... more states
```

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

## 🔍 Usage Warning

This demonstration requires root privileges. **Only use on systems you own or have explicit permission to test**.

## ⏳ Project Status

This project is currently **under development** and not finished. Future work includes:

- [ ] Improved error handling
- [ ] Memory optimization
- [ ] Additional evasion techniques
- [ ] Cleanup routines

## 🤝 Contribution

No contributions are being accepted as this is a research demonstration project.

## 📝 License

**Educational and Research Purposes Only**. Not for deployment on unauthorized systems.

---

⚠️ This software demonstrates techniques that could be misused. It is shared solely for educational purposes to understand system security vulnerabilities and detection methods.
