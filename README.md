[![C CI Build](https://github.com/R-Gld/FiSH/actions/workflows/c.yml/badge.svg)](https://github.com/R-Gld/FiSH/actions/workflows/c.yml)

# FiSH

## Description

This project was completed as the end-of-semester assignment for Semester 4 (L2) in Systems and Systems Programming. <br>
The instruction PDF can be found [here](https://github.com/R-Gld/FiSH/blob/master/projet-fish.pdf).

The aim of the project is to create a simple shell that allows executing commands with arguments, piped commands, background commands, and to enable redirection (with < or >) to files, ...

## Installation

```bash
sudo apt-get update # Update the package list
sudo apt-get install -y git gcc make doxygen # Install the dependencies (doxygen is optional and used only if you want to generate the documentation)
git clone https://github.com/R-Gld/FiSH.git # Clone the repository
cd FiSH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/execs # Make temporary library available.
# If you wan't to install it permanently, see the next section.
make clean install # Compile the project
```

If you want to generate the documentation, you can run `make full-docs` after installing the dependencies.

## Usage

```bash
./execs/fish
```

If you want to be able to use it from anywhere, run this from the root of the projects to add the executable to your path:
```bash
make permanent-install
```

After, you can just execute `fish` from anywhere.

## Authors

 - **[Romain GALLAND](https://github.com/R-Gld)**: Me, the one who did the project.
 - **Eric MERLET**: My teacher, the one who wrote the start of the project.