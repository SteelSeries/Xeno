# Xeno Firmware Repository

This repository contains three types from the Xeno project: 96, 65, and TKL.

## 📂 Repository Structure

```
Xeno/
│── common/
│   ├── Artery/
│   ├── STM32/
|── data/
|── drivers_sdk/
│── projects/
│   ├── Xeno_65/
│   ├── Xeno_96/
│   ├── Xeno_TKL/
│── tools/
│── README.md
```

- `drivers_sdk/, data/` – Documentation, API references, and user guides.  
- `tools/` – Utility scripts, test applications, and firmware update tools.  

## 🏗️ Development Guidelines  
- Follow the **coding style guide**:   [Coding Style Guidelines](https://steelseries.atlassian.net/wiki/spaces/SFW/pages/2014314498/Coding+Style+Guidelines)  
- Use **feature branches** for new developments.
- Use **ticket branches** for new developments.  
- Submit changes via **pull requests (PRs)**.
- All PRs merged in **stage** branch must be tested.  
- Recommended submission/PR Format:  
  `ticket-d: <description>` for new features or bug fixes
  `docs: <description>` for documentation updates.  
  `style: <description>` for formatting changes.  
  `refactor: <description>` for code refactoring.  
  `test: <description>` for adding or updating tests.  

## 🔄 Git Workflow and Branching Strategy
Main Branches:  
  - ` main`  
  - `stage-<project-name>`  
  
Supporting Branches:
 - feature branch: `feature-<project/feature-name>`  
 - ticket branch: `<ticket-id>-<short-description>`  
 
For details on the Git workflow and Branching Strategy for this firmware project, refer to:  
[Git Workflow for Firmware Project](https://steelseries.atlassian.net/wiki/spaces/SFW/pages/2011594753/Git+Workflow+for+Firmware+Project)
