# IDGeneratorTCP

IDGeneratorTCP is a simple server that sends unique IDs to connected clients every second.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction

IDGeneratorTCP is a C++ server application designed to generate and distribute unique IDs to connected clients over TCP/IP. It provides a simple mechanism for clients to request and receive unique identifiers at regular intervals.

## Features

- Generates and sends unique IDs to connected clients.
- Uses TCP/IP for reliable communication.
- Simple and lightweight.

## Getting Started

Follow the steps below to get the IDGeneratorTCP server up and running on your local machine.

### Prerequisites

Ensure you have the following installed:

- C++ Compiler (e.g., g++)
- CMake

### Installation

Clone the repository and build the project:

```bash
git clone https://github.com/yourusername/IDGeneratorTCP.git
cd IDGeneratorTCP
./compile.sh
```

### Usage

Run the server and the client in the folder 'build/bin'
```bash
./ServerApp # To run the server
./ClientApp # To run the client
```

## Contributing 

Contributions are welcome! Follow the steps below to contribute:

1- Fork the project.

2- Create a new branch.

3- Make your changes and commit them.

4- Push to your fork and submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.

## Acknowledgements

The C++ community for inspiration.

[CMake](https://cmake.org) for the build system.