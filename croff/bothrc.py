#!/usr/bin/env python3
"""
Build script for troff components.

This script assembles assembly source files (.s) into object files (.o)
using the system assembler. It maintains compatibility with the original
bash script while providing enhanced error handling and validation.

Original functionality:
- Assembles t.s -> t.o
- Assembles hytab.s -> hytab.o
- Provides comprehensive error checking
- Cleans existing output files before building

Author: Converted from bash script
License: Compatible with original troff licensing
"""

import os
import subprocess
import sys
from pathlib import Path
from typing import List, Optional, Tuple


class AssemblyBuildError(Exception):
    """Custom exception for assembly build failures."""

    pass


class TroffBuilder:
    """
    A robust builder for troff assembly components.
    
    This class handles the assembly of source files with comprehensive
    error checking, validation, and logging.
    """

    def __init__(self, working_directory: Optional[str] = None) -> None:
        """
        Initialize the TroffBuilder.
        
        Args:
            working_directory: Directory to perform builds in.
                             Defaults to current directory.
        """
        self.working_dir = Path(working_directory) if working_directory else Path.cwd()
        self.assembler_command = "as"
        self.temp_output = "a.out"
        
        # Validate working directory
        if not self.working_dir.exists():
            raise AssemblyBuildError(f"Working directory does not exist: {self.working_dir}")
        
        if not self.working_dir.is_dir():
            raise AssemblyBuildError(f"Working directory is not a directory: {self.working_dir}")

    def _validate_assembler(self) -> None:
        """
        Validate that the assembler is available in the system PATH.
        
        Raises:
            AssemblyBuildError: If assembler is not found or not executable.
        """
        try:
            result = subprocess.run(
                [self.assembler_command, "--version"],
                capture_output=True,
                text=True,
                timeout=10,
                cwd=self.working_dir
            )
            if result.returncode != 0:
                # Some assemblers don't support --version, try --help
                result = subprocess.run(
                    [self.assembler_command, "--help"],
                    capture_output=True,
                    text=True,
                    timeout=10,
                    cwd=self.working_dir
                )
        except (subprocess.TimeoutExpired, FileNotFoundError) as e:
            raise AssemblyBuildError(
                f"Assembler '{self.assembler_command}' not found or not working. "
                f"Please ensure it's installed and in PATH. Error: {e}"
            ) from e

    def _validate_source_file(self, source_file: Path) -> None:
        """
        Validate that a source file exists and is readable.
        
        Args:
            source_file: Path to the source file to validate.
            
        Raises:
            AssemblyBuildError: If file doesn't exist, isn't readable, or is empty.
        """
        if not source_file.exists():
            raise AssemblyBuildError(f"Source file not found: {source_file}")
        
        if not source_file.is_file():
            raise AssemblyBuildError(f"Source path is not a file: {source_file}")
        
        if not os.access(source_file, os.R_OK):
            raise AssemblyBuildError(f"Source file is not readable: {source_file}")
        
        # Check if file is empty
        if source_file.stat().st_size == 0:
            raise AssemblyBuildError(f"Source file is empty: {source_file}")

    def _clean_temp_files(self) -> None:
        """Remove temporary output files from previous builds."""
        temp_file = self.working_dir / self.temp_output
        if temp_file.exists():
            try:
                temp_file.unlink()
                print(f"Cleaned temporary file: {temp_file}")
            except OSError as e:
                print(f"Warning: Could not remove temporary file {temp_file}: {e}")

    def assemble_file(self, source_file: str, output_file: str) -> bool:
        """
        Assemble a single source file into an object file.
        
        Args:
            source_file: Name of the assembly source file (e.g., "t.s").
            output_file: Name of the desired output file (e.g., "t.o").
            
        Returns:
            bool: True if assembly was successful, False otherwise.
            
        Raises:
            AssemblyBuildError: If assembly fails or files cannot be processed.
        """
        source_path = self.working_dir / source_file
        output_path = self.working_dir / output_file
        temp_path = self.working_dir / self.temp_output
        
        # Validate source file
        self._validate_source_file(source_path)
        
        # Clean any existing temporary output
        self._clean_temp_files()
        
        print(f"Assembling {source_file}...")
        
        try:
            # Run the assembler
            result = subprocess.run(
                [self.assembler_command, str(source_path)],
                capture_output=True,
                text=True,
                timeout=60,  # 60 second timeout for assembly
                cwd=self.working_dir
            )
            
            if result.returncode != 0:
                error_msg = (
                    f"Assembly of {source_file} failed with return code {result.returncode}.\n"
                    f"STDOUT: {result.stdout}\n"
                    f"STDERR: {result.stderr}"
                )
                raise AssemblyBuildError(error_msg)
            
            # Check if assembler created the expected output
            if not temp_path.exists():
                raise AssemblyBuildError(
                    f"Assembler did not create expected output file: {temp_path}"
                )
            
            # Move temporary output to final location
            try:
                if output_path.exists():
                    output_path.unlink()  # Remove existing output file
                
                temp_path.rename(output_path)
                print(f"Created {output_file}")
                
                # Verify the output file was created successfully
                if not output_path.exists() or output_path.stat().st_size == 0:
                    raise AssemblyBuildError(f"Output file {output_file} was not created properly")
                
                return True
                
            except OSError as e:
                raise AssemblyBuildError(
                    f"Failed to rename {self.temp_output} to {output_file}: {e}"
                ) from e
                
        except subprocess.TimeoutExpired as e:
            raise AssemblyBuildError(f"Assembly of {source_file} timed out") from e
        except subprocess.SubprocessError as e:
            raise AssemblyBuildError(f"Subprocess error during assembly of {source_file}: {e}") from e

    def clean_output_files(self, output_files: List[str]) -> None:
        """
        Clean existing output files before building.
        
        Args:
            output_files: List of output file names to clean.
        """
        print("Cleaning existing output files...")
        for output_file in output_files:
            output_path = self.working_dir / output_file
            if output_path.exists():
                try:
                    output_path.unlink()
                    print(f"Removed existing {output_file}")
                except OSError as e:
                    print(f"Warning: Could not remove {output_file}: {e}")

    def build_all(self) -> Tuple[bool, List[str]]:
        """
        Build all troff components.
        
        Returns:
            Tuple[bool, List[str]]: (success, list_of_created_files)
            
        Raises:
            AssemblyBuildError: If validation or assembly fails.
        """
        # Configuration for files to build
        build_targets = [
            ("t.s", "t.o"),
            ("hytab.s", "hytab.o"),
        ]
        
        output_files = [target[1] for target in build_targets]
        created_files = []
        
        try:
            # Validate environment
            self._validate_assembler()
            
            # Clean existing output files
            self.clean_output_files(output_files)
            
            # Build each target
            for source_file, output_file in build_targets:
                if self.assemble_file(source_file, output_file):
                    created_files.append(output_file)
                else:
                    return False, created_files
            
            print("Build completed successfully")
            return True, created_files
            
        except AssemblyBuildError:
            # Clean up any partial builds on failure
            self._clean_temp_files()
            raise


def main() -> int:
    """
    Main entry point for the build script.
    
    Returns:
        int: Exit code (0 for success, 1 for failure).
    """
    try:
        # Create builder instance
        builder = TroffBuilder()
        
        # Perform the build
        success, created_files = builder.build_all()
        
        if success:
            print(f"\nSuccessfully created files: {', '.join(created_files)}")
            return 0
        else:
            print("\nBuild failed")
            return 1
            
    except AssemblyBuildError as e:
        print(f"Build error: {e}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("\nBuild interrupted by user", file=sys.stderr)
        return 130  # Standard exit code for SIGINT
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
