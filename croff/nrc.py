#!/usr/bin/env python3
"""
nrc - NROFF Build Script

This script builds the NROFF text formatter from C source files.
It replaces the original shell script with a modern Python implementation
that provides better error handling, logging, and cross-platform compatibility.

Original functionality:
- Compile C source files with NROFF flag
- Link with object files
- Strip debugging symbols
- Create nroff executable

Features:
- Cross-platform compatibility
- Comprehensive error handling
- Detailed logging and progress reporting
- Dependency checking
- Build validation
- Clean build options
"""

import argparse
import logging
import os
import subprocess
import sys
from pathlib import Path
from typing import List, Optional, Tuple


class NroffBuilder:
    """
    Builder class for NROFF text formatter.

    Handles compilation, linking, and validation of the NROFF executable
    with comprehensive error handling and logging.
    """

    def __init__(self, build_dir: Optional[Path] = None, verbose: bool = False):
        """
        Initialize the NROFF builder.

        Args:
            build_dir: Directory containing source files (default: current directory)
            verbose: Enable verbose logging
        """
        self.build_dir = build_dir or Path.cwd()
        self.verbose = verbose
        self.logger = self._setup_logging()

        # Define source files and object files based on original shell script
        self.source_files = [
            "n1.c",
            "n2.c",
            "n3.c",
            "n4.c",
            "n5.c",
            "n6.c",
            "n7.c",
            "n8.c",
            "n9.c",
            "n10.c",
            "ni.c",
            "nii.c",
            "ntab.c",
        ]
        self.object_files = ["hytab.o", "t.o"]
        self.output_executable = "nroff"

        # Compiler settings (matching original shell script)
        self.compiler = "cc"
        self.compile_flags = ["-DNROFF", "-n", "-O"]
        self.strip_symbols = True

        # Additional build configurations
        self.backup_existing = True
        self.validate_sources = True
    
    def _setup_logging(self) -> logging.Logger:
        """
        Set up logging configuration.

        Returns:
            Configured logger instance
        """
        logger = logging.getLogger("nroff_builder")
        logger.setLevel(logging.DEBUG if self.verbose else logging.INFO)

        if not logger.handlers:
            handler = logging.StreamHandler(sys.stdout)
            formatter = logging.Formatter(
                "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
            )
            handler.setFormatter(formatter)
            logger.addHandler(handler)

        return logger
    
    def check_dependencies(self) -> bool:
        """
        Check if all required source and object files exist.

        Returns:
            True if all dependencies are present, False otherwise
        """
        self.logger.info("Checking build dependencies...")

        missing_files = []

        # Check source files
        for source_file in self.source_files:
            file_path = self.build_dir / source_file
            if not file_path.exists():
                missing_files.append(str(file_path))
            elif self.validate_sources:
                # Basic validation: check if file is not empty and appears to be C code
                try:
                    with open(file_path, "r", encoding="utf-8") as f:
                        content = f.read(100)  # Read first 100 chars
                        if not content.strip():
                            self.logger.warning(
                                f"Source file {source_file} appears to be empty"
                            )
                        elif not (
                            "#include" in content
                            or "int " in content
                            or "char " in content
                        ):
                            self.logger.warning(
                                f"Source file {source_file} may not be valid C code"
                            )
                except (UnicodeDecodeError, IOError) as e:
                    self.logger.warning(
                        f"Could not validate source file {source_file}: {e}"
                    )

        # Check object files
        for object_file in self.object_files:
            file_path = self.build_dir / object_file
            if not file_path.exists():
                missing_files.append(str(file_path))
            else:
                # Basic validation: check if object file has reasonable size
                file_size = file_path.stat().st_size
                if file_size == 0:
                    self.logger.warning(f"Object file {object_file} is empty")
                elif file_size < 50:  # Very small for an object file
                    self.logger.warning(
                        f"Object file {object_file} is suspiciously small "
                        f"({file_size} bytes)"
                    )

        if missing_files:
            self.logger.error("Missing required files:")
            for missing_file in missing_files:
                self.logger.error(f"  - {missing_file}")
            return False

        self.logger.info("All dependencies found.")
        return True
    
    def run_command(self, command: List[str], description: str) -> Tuple[bool, str]:
        """
        Execute a shell command with error handling.
        
        Args:
            command: Command and arguments to execute
            description: Human-readable description of the command
            
        Returns:
            Tuple of (success_flag, output_message)
        """
        self.logger.info(f"{description}...")
        self.logger.debug(f"Executing: {' '.join(command)}")
        
        try:
            result = subprocess.run(
                command,
                cwd=self.build_dir,
                capture_output=True,
                text=True,
                check=True
            )
            
            if result.stdout:
                self.logger.debug(f"stdout: {result.stdout}")
            if result.stderr:
                self.logger.debug(f"stderr: {result.stderr}")
            
            self.logger.info(f"{description} completed successfully.")
            return True, result.stdout
            
        except subprocess.CalledProcessError as e:
            error_msg = f"{description} failed with exit code {e.returncode}"
            if e.stderr:
                error_msg += f"\nError output: {e.stderr}"
            self.logger.error(error_msg)
            return False, error_msg
        
        except FileNotFoundError:
            error_msg = f"Command not found: {command[0]}"
            self.logger.error(error_msg)
            return False, error_msg
    
    def compile_and_link(self) -> bool:
        """
        Compile source files and link with object files.
        
        Returns:
            True if compilation and linking succeed, False otherwise
        """
        # Construct compilation command
        compile_command = [self.compiler] + self.compile_flags
        
        # Add source files
        compile_command.extend(self.source_files)
        
        # Add object files
        compile_command.extend(self.object_files)
        
        # Execute compilation
        success, output = self.run_command(
            compile_command,
            "Compiling and linking NROFF"
        )
        
        return success
    
    def strip_executable(self) -> bool:
        """
        Strip debugging symbols from the executable.
        
        Returns:
            True if stripping succeeds, False otherwise
        """
        if not self.strip_symbols:
            self.logger.info("Skipping symbol stripping (disabled).")
            return True
        
        # Check if a.out exists
        temp_executable = self.build_dir / "a.out"
        if not temp_executable.exists():
            self.logger.error("Temporary executable 'a.out' not found.")
            return False
        
        # Strip symbols
        strip_command = ["strip", "a.out"]
        success, output = self.run_command(
            strip_command,
            "Stripping debugging symbols"
        )
        
        return success
    
    def rename_executable(self) -> bool:
        """
        Rename the temporary executable to the final name.
        
        Returns:
            True if renaming succeeds, False otherwise
        """
        temp_executable = self.build_dir / "a.out"
        final_executable = self.build_dir / self.output_executable
        
        if not temp_executable.exists():
            self.logger.error("Temporary executable 'a.out' not found.")
            return False
        
        try:
            # Remove existing executable if it exists
            if final_executable.exists():
                final_executable.unlink()
                self.logger.debug(f"Removed existing {self.output_executable}")
            
            # Rename a.out to nroff
            temp_executable.rename(final_executable)
            self.logger.info(f"Created executable: {final_executable}")
            return True
            
        except OSError as e:
            self.logger.error(f"Failed to rename executable: {e}")
            return False
    
    def validate_build(self) -> bool:
        """
        Validate that the build was successful.
        
        Returns:
            True if validation passes, False otherwise
        """
        executable_path = self.build_dir / self.output_executable
        
        if not executable_path.exists():
            self.logger.error(f"Executable {self.output_executable} not found.")
            return False
        
        # Check if file is executable
        if not os.access(executable_path, os.X_OK):
            self.logger.error(f"File {self.output_executable} is not executable.")
            return False
        
        # Get file size
        file_size = executable_path.stat().st_size
        self.logger.info(f"Built executable: {executable_path} ({file_size} bytes)")
        
        # Optional: Test execution (just check if it starts)
        try:
            result = subprocess.run(
                [str(executable_path), "--help"],
                capture_output=True,
                timeout=5,
                cwd=self.build_dir
            )
            # Note: Many programs return non-zero for --help, so we don't check return code
            self.logger.debug("Executable appears to run correctly.")
        except (subprocess.TimeoutExpired, FileNotFoundError):
            self.logger.warning("Could not validate executable functionality.")
        
        return True
    
    def clean_build(self) -> bool:
        """
        Clean build artifacts.
        
        Returns:
            True if cleaning succeeds, False otherwise
        """
        self.logger.info("Cleaning build artifacts...")
        
        artifacts = ["a.out", self.output_executable]
        
        for artifact in artifacts:
            artifact_path = self.build_dir / artifact
            if artifact_path.exists():
                try:
                    artifact_path.unlink()
                    self.logger.info(f"Removed {artifact}")
                except OSError as e:
                    self.logger.error(f"Failed to remove {artifact}: {e}")
                    return False
        
        self.logger.info("Build artifacts cleaned.")
        return True
    
    def backup_existing_executable(self) -> bool:
        """
        Create a backup of existing executable before building.
        
        Returns:
            True if backup succeeds or no backup needed, False otherwise
        """
        if not self.backup_existing:
            return True
            
        executable_path = self.build_dir / self.output_executable
        if not executable_path.exists():
            return True  # No existing executable to backup
            
        backup_path = self.build_dir / f"{self.output_executable}.backup"
        
        try:
            if backup_path.exists():
                backup_path.unlink()
            executable_path.rename(backup_path)
            self.logger.info(f"Created backup: {backup_path}")
            return True
        except OSError as e:
            self.logger.error(f"Failed to create backup: {e}")
            return False
    
    def get_build_info(self) -> dict:
        """
        Get information about the build environment.
        
        Returns:
            Dictionary containing build environment information
        """
        info = {
            "build_dir": str(self.build_dir),
            "compiler": self.compiler,
            "compile_flags": self.compile_flags,
            "source_files": self.source_files,
            "object_files": self.object_files,
            "output_executable": self.output_executable,
            "strip_symbols": self.strip_symbols
        }
        
        # Add compiler version if available
        try:
            result = subprocess.run(
                [self.compiler, "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                info["compiler_version"] = result.stdout.strip().split('\n')[0]
        except (subprocess.TimeoutExpired, FileNotFoundError):
            info["compiler_version"] = "Unknown"
        
        return info
    
    def build(self) -> bool:
        """
        Execute the complete build process.
        
        Returns:
            True if build succeeds, False otherwise
        """
        self.logger.info("Starting NROFF build process...")
        
        # Backup existing executable if requested
        if not self.backup_existing_executable():
            return False
        
        # Check dependencies
        if not self.check_dependencies():
            return False
        
        # Compile and link
        if not self.compile_and_link():
            return False
        
        # Strip symbols
        if not self.strip_executable():
            return False
        
        # Rename executable
        if not self.rename_executable():
            return False
        
        # Validate build
        if not self.validate_build():
            return False
        
        self.logger.info("NROFF build completed successfully!")
        return True


def main():
    """
    Main entry point for the NROFF build script.
    """
    parser = argparse.ArgumentParser(
        description="Build script for NROFF text formatter",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 nrc                    # Build NROFF in current directory
  python3 nrc --verbose          # Build with verbose output
  python3 nrc --clean            # Clean build artifacts
  python3 nrc --build-dir /path  # Build in specific directory
  python3 nrc --info             # Show build environment info
        """,
    )

    parser.add_argument(
        "--build-dir",
        type=Path,
        help="Directory containing source files (default: current directory)",
    )

    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Enable verbose logging",
    )

    parser.add_argument(
        "--clean", action="store_true", help="Clean build artifacts and exit"
    )

    parser.add_argument(
        "--no-strip",
        action="store_true",
        help="Skip stripping debugging symbols",
    )

    parser.add_argument(
        "--compiler", default="cc", help="C compiler to use (default: cc)"
    )

    parser.add_argument("--flags", nargs="*", help="Additional compiler flags")

    parser.add_argument(
        "--info",
        action="store_true",
        help="Show build environment information and exit",
    )

    parser.add_argument(
        "--backup",
        action="store_true",
        default=True,
        help="Create backup of existing executable (default: enabled)",
    )

    parser.add_argument(
        "--no-backup",
        action="store_true",
        help="Skip creating backup of existing executable",
    )

    parser.add_argument(
        "--validate",
        action="store_true",
        default=True,
        help="Validate source files before building (default: enabled)",
    )

    args = parser.parse_args()

    # Create builder instance
    builder = NroffBuilder(build_dir=args.build_dir, verbose=args.verbose)

    # Override settings from command line
    if args.no_strip:
        builder.strip_symbols = False

    if args.compiler:
        builder.compiler = args.compiler

    if hasattr(args, "flags") and args.flags:
        builder.compile_flags.extend(args.flags)

    if hasattr(args, "no_backup") and args.no_backup:
        builder.backup_existing = False

    if hasattr(args, "validate") and not args.validate:
        builder.validate_sources = False

    # Handle info operation
    if hasattr(args, "info") and args.info:
        info = builder.get_build_info()
        print("Build Environment Information:")
        for key, value in info.items():
            print(f"  {key}: {value}")
        sys.exit(0)

    # Handle clean operation
    if args.clean:
        success = builder.clean_build()
        sys.exit(0 if success else 1)

    # Execute build
    success = builder.build()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
