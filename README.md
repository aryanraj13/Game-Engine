# 2D Game Engine

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build Status](https://travis-ci.org/yourusername/2d-game-engine.svg?branch=master)](https://travis-ci.org/yourusername/2d-game-engine)
[![Issues](https://img.shields.io/github/issues/yourusername/2d-game-engine.svg)](https://github.com/yourusername/2d-game-engine/issues)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

2D Game Engine is a lightweight and modular C++ game engine built on top of the Simple DirectMedia Layer (SDL). It provides a foundation for developing 2D and simple 3D games with a focus on simplicity, performance, and extensibility.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Overview

2D Game Engine is designed to offer a flexible and efficient platform for game development. With its modular architecture and SDL integration, it aims to provide an accessible yet powerful framework for creating both 2D and simple 3D games.

## Features

- **SDL Integration:** Utilizes the power and flexibility of SDL for graphics, input, and window management.
- **Modularity:** Designed with a modular architecture for easy extension and customization.
- **Cross-Platform:** Works seamlessly across different platforms, ensuring a consistent experience for users on Windows, Linux, and macOS.
- **Efficient Rendering:** Optimized rendering pipeline for smooth graphics performance.
- **Input Handling:** Simple and intuitive API for handling user input.
- **Extensive Documentation:** Well-documented codebase and developer-friendly documentation to get you started quickly.

## Getting Started

Follow the [Getting Started Guide](docs/getting-started.md) in the documentation to set up and start using 2D Game Engine in your projects.

## Usage

Here's a basic example of how to use 2D Game Engine:

```cpp
#include <2d_game_engine/game_engine.h>

int main() {
    // Initialize the game engine
    GameEngine engine;

    // Load assets, set up scenes, etc.

    // Main game loop
    while (engine.isRunning()) {
        // Update game logic
        engine.update();

        // Render graphics
        engine.render();
    }

    // Cleanup and exit
    engine.cleanup();

    return 0;
}
