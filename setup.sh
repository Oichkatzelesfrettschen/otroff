#!/bin/bash
# setup.sh - Install troff tooling, BCPL, and ACK compilers
# This script should be run while network access is available.
set -euo pipefail

# Ensure the script is executed as root.
if [ "$(id -u)" -ne 0 ]; then
    echo "This script must be run as root." >&2
    exit 1
fi

# Update package lists
apt-get update

# Install apt packages for troff and development
apt-get install -y --no-install-recommends \
    build-essential git clang gdb valgrind afl++ \
    strace ltrace binutils autoconf automake libtool \
    pkg-config groff groff-base mandoc \
    radare2 \
    bcpl bcpl-dev ack ack-dev

# Upgrade pip and install Python tooling for analysis
python3 -m pip install --upgrade pip
python3 -m pip install capstone lief unicorn pwntools pyelftools r2pipe hexdump rich troff-utils

# Install any troff tooling available via npm
if command -v npm >/dev/null 2>&1; then
    npm install -g troff
fi


