# FiSH

## Description

This project was completed as the end-of-semester assignment for Semester 4 (L2) in Systems and Systems Programming. <br>
The instruction PDF can be found [here](https://github.com/R-Gld/FiSH/blob/master/projet-fish.pdf).

The aim of the project is to create a simple shell that allows executing commands with arguments, piped commands, background commands, and to enable redirection (with < or >) to files, ...

## Installation

```bash
sudo apt-get update
sudo apt-get install -y git gcc make doxygen 
git clone https://github.com/R-Gld/FiSH.git
cd FiSH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/execs/
make clean
make install
```

If you want to generate the documentation, you can run `make full-docs` after installing the dependencies.

## Usage

```bash
./execs/fish
```

If you want to be able to use it from anywhere, run this from the root of the projects to add the executable to your path:
```bash
echo "export PATH=$PATH:$PWD/execs" >> ~/.bashrc
source ~/.bashrc
```

After, you can just execute `fish` from anywhere.

## Authors

 - **[Romain GALLAND](https://github.com/R-Gld)**: Me, the one who did the project.
 - **Eric MERLET**: My teacher, the one who wrote the start of the project.