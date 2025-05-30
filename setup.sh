#!/bin/bash
# setup.sh - Install troff tooling, BCPL, and ACK compilers
# This script should be run while network access is available.

# Exit on errors, undefined variables, and failed pipelines
set -euo pipefail

# Ensure the script is executed as root for package installation
if [[ "$(id -u)" -ne 0 ]]; then
    echo "This script must be run as root." >&2
    exit 1
fi

# Update package lists
apt-get update

# Determine if the ACK compiler is already installed
ACK_NEEDED=false
if ! command -v ack >/dev/null 2>&1; then
    ACK_NEEDED=true
fi

# Base packages required for building and analysis
PKGS="build-essential git clang gdb valgrind afl++ \
    strace ltrace binutils autoconf automake libtool \
    pkg-config groff groff-base mandoc radare2 \
    bcpl bcpl-dev"

# Install ACK packages only when missing
if [ "$ACK_NEEDED" = true ]; then
    PKGS="$PKGS ack ack-dev"
fi


# Install all required packages in one transaction
apt-get install -y --no-install-recommends $PKGS

# Verify that the ACK compiler is now available
if ! command -v ack >/dev/null 2>&1; then
    echo "ACK compiler is required but could not be installed." >&2
    exit 1
fi

# Upgrade pip and install Python tooling for analysis
python3 -m pip install --upgrade pip
python3 -m pip install capstone lief unicorn pwntools pyelftools r2pipe hexdump rich troff-utils

# Install any troff tooling available via npm
if command -v npm >/dev/null 2>&1; then
    npm install -g troff
fi

# Build the repository using the installed toolchain
make


