#!/usr/bin/env python3
"""
neqnrc - NEQN Equation Formatter Build Script

This script builds the NEQN equation formatter from grammar and C source files.
It replaces the original shell script with a modern Python implementation
that provides better error handling, logging, and cross-platform compatibility.

Original shell script functionality:
- yacc ne.g                # Parse grammar file with yacc
- cc -O -n -s ne*.c        # Compile source files with optimization
- rm -f *.o y.tab.c        # Clean intermediate files
- echo "done"              # Report completion

Enhanced Python features:
- Cross-platform compatibility
- Comprehensive error handling
- Detailed logging and progress reporting
- if __name__ == "__main__":
    main()dency checking and validation
- Build artifact management
- Grammar file validation
- Parser generation management
- Configurable build options
"""

import argparse
import logging
import os
import subprocess
import sys
from pathlib import Path
from typing import List, Optional, Tuple, Dict


class NeqnBuilder:
    """
    Builder class for NEQN equation formatter.
    
    Handles grammar parsing, compilation, linking, and validation of the NEQN
    executable with comprehensive error handling and logging.
    """
    
    def __init__(self, build_dir: Optional[Path] = None, verbose: bool = False):
        """
        Initialize the NEQN builder.
        
        Args:
            build_dir: Directory containing source files (default: current directory)
            verbose: Enable verbose logging
        """
        self.build_dir = build_dir or Path.cwd()
        self.verbose = verbose
        self.logger = self._setup_logging()
        
        # Define grammar file and source patterns
        self.grammar_file = "ne.g"
        self.source_pattern = "ne*.c"
        self.output_executable = "neqn"
        
        # Parser generation settings
        self.parser_generator = "yacc"
        self.generated_files = ["y.tab.c", "y.tab.h"]
        
        # Compiler settings (matching original shell script)
        self.compiler = "cc"
        self.compile_flags = ["-O", "-n", "-s"]
        
        # Cleanup settings
        self.cleanup_objects = True
        self.cleanup_generated = True
        self.intermediate_files = ["*.o", "y.tab.c"]
        
        # Additional build configurations
        self.backup_existing = True
        self.validate_sources = True
        self.validate_grammar = True
    
    def _setup_logging(self) -> logging.Logger:
        """
        Set up logging configuration.
        
        Returns:
            Configured logger instance
        """
        logger = logging.getLogger("neqn_builder")
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
        Check if all required files and tools exist.
        
        Returns:
            True if all dependencies are present, False otherwise
        """
        self.logger.info("Checking build dependencies...")
        
        missing_items = []
        
        # Check grammar file
        grammar_path = self.build_dir / self.grammar_file
        if not grammar_path.exists():
            missing_items.append(f"Grammar file: {grammar_path}")
        elif self.validate_grammar:
            # Basic validation of grammar file
            try:
                with open(grammar_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    if not content.strip():
                        self.logger.warning(f"Grammar file {self.grammar_file} appears to be empty")
                    elif not any(keyword in content for keyword in ['%token', '%type', '%%', ':']):
                        self.logger.warning(f"Grammar file {self.grammar_file} may not be valid yacc grammar")
            except (UnicodeDecodeError, IOError) as e:
                self.logger.warning(f"Could not validate grammar file {self.grammar_file}: {e}")
        
        # Check for source files matching pattern
        source_files = list(self.build_dir.glob(self.source_pattern))
        if not source_files:
            missing_items.append(f"Source files matching pattern: {self.source_pattern}")
        else:
            self.logger.debug(f"Found source files: {[f.name for f in source_files]}")
            
            if self.validate_sources:
                # Validate source files
                for source_file in source_files:
                    try:
                        with open(source_file, 'r', encoding='utf-8') as f:
                            content = f.read(200)  # Read first 200 chars
                            if not content.strip():
                                self.logger.warning(f"Source file {source_file.name} appears to be empty")
                            elif not any(keyword in content for keyword in ['#include', 'int ', 'char ', 'void ']):
                                self.logger.warning(f"Source file {source_file.name} may not be valid C code")
                    except (UnicodeDecodeError, IOError) as e:
                        self.logger.warning(f"Could not validate source file {source_file.name}: {e}")
        
        # Check for required tools
        tools_to_check = [self.parser_generator, self.compiler]
        for tool in tools_to_check:
            if not self._check_tool_availability(tool):
                missing_items.append(f"Required tool: {tool}")
        
        if missing_items:
            self.logger.error("Missing required dependencies:")
            for item in missing_items:
                self.logger.error(f"  - {item}")
            return False
        
        self.logger.info("All dependencies found.")
        return True
    
    def _check_tool_availability(self, tool: str) -> bool:
        """
        Check if a command-line tool is available.
        
        Args:
            tool: Name of the tool to check
            
        Returns:
            True if tool is available, False otherwise
        """
        try:
            result = subprocess.run(
                [tool, "--version"],
                capture_output=True,
                timeout=5
            )
            return True
        except (subprocess.TimeoutExpired, FileNotFoundError):
            try:
                # Try alternative check for some tools
                result = subprocess.run(
                    ["which", tool],
                    capture_output=True,
                    timeout=5
                )
                return result.returncode == 0
            except (subprocess.TimeoutExpired, FileNotFoundError):
                return False
    
    def run_command(self, command: List[str], description: str, 
                   check_return_code: bool = True) -> Tuple[bool, str]:
        """
        Execute a shell command with error handling.
        
        Args:
            command: Command and arguments to execute
            description: Human-readable description of the command
            check_return_code: Whether to treat non-zero exit as error
            
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
                check=check_return_code
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
    
    def generate_parser(self) -> bool:
        """
        Generate parser from grammar file using yacc.
        
        Returns:
            True if parser generation succeeds, False otherwise
        """
        # Construct yacc command
        yacc_command = [self.parser_generator, self.grammar_file]
        
        # Execute yacc
        success, output = self.run_command(
            yacc_command,
            "Generating parser from grammar"
        )
        
        if success:
            # Verify that expected files were generated
            for generated_file in self.generated_files:
                file_path = self.build_dir / generated_file
                if file_path.exists():
                    file_size = file_path.stat().st_size
                    self.logger.debug(f"Generated file: {generated_file} ({file_size} bytes)")
                else:
                    self.logger.warning(f"Expected generated file not found: {generated_file}")
        
        return success
    
    def compile_sources(self) -> bool:
        """
        Compile all source files.
        
        Returns:
            True if compilation succeeds, False otherwise
        """
        # Get all source files matching the pattern
        source_files = list(self.build_dir.glob(self.source_pattern))
        
        if not source_files:
            self.logger.error(f"No source files found matching pattern: {self.source_pattern}")
            return False
        
        # Construct compilation command
        compile_command = [self.compiler] + self.compile_flags
        compile_command.extend([str(f) for f in source_files])
        
        # Execute compilation
        success, output = self.run_command(
            compile_command,
            f"Compiling {len(source_files)} source files"
        )
        
        return success
    
    def cleanup_intermediate_files(self) -> bool:
        """
        Clean up intermediate files after compilation.
        
        Returns:
            True if cleanup succeeds, False otherwise
        """
        if not (self.cleanup_objects or self.cleanup_generated):
            self.logger.info("Skipping cleanup (disabled).")
            return True
        
        self.logger.info("Cleaning up intermediate files...")
        
        files_removed = []
        files_failed = []
        
        for pattern in self.intermediate_files:
            matching_files = list(self.build_dir.glob(pattern))
            for file_path in matching_files:
                try:
                    file_path.unlink()
                    files_removed.append(file_path.name)
                    self.logger.debug(f"Removed: {file_path.name}")
                except OSError as e:
                    files_failed.append(f"{file_path.name}: {e}")
                    self.logger.warning(f"Failed to remove {file_path.name}: {e}")
        
        if files_removed:
            self.logger.info(f"Removed {len(files_removed)} intermediate files")
        
        if files_failed:
            self.logger.warning(f"Failed to remove {len(files_failed)} files:")
            for failure in files_failed:
                self.logger.warning(f"  - {failure}")
            return False
        
        return True
    
    def rename_executable(self) -> bool:
        """
        Rename the compiled executable to the final name.
        
        Returns:
            True if renaming succeeds, False otherwise
        """
        # Check for common output names
        possible_outputs = ["a.out", "ne"]
        temp_executable = None
        
        for output_name in possible_outputs:
            candidate = self.build_dir / output_name
            if candidate.exists():
                temp_executable = candidate
                break
        
        if not temp_executable:
            self.logger.error("No compiled executable found (looked for: {})".format(
                ", ".join(possible_outputs)
            ))
            return False
        
        final_executable = self.build_dir / self.output_executable
        
        try:
            # Remove existing executable if it exists
            if final_executable.exists():
                final_executable.unlink()
                self.logger.debug(f"Removed existing {self.output_executable}")
            
            # Rename to final name
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
        
        # Get file size and basic info
        file_stat = executable_path.stat()
        file_size = file_stat.st_size
        self.logger.info(f"Built executable: {executable_path} ({file_size} bytes)")
        
        # Optional: Test execution (just check if it starts)
        try:
            result = subprocess.run(
                [str(executable_path)],
                input="",
                capture_output=True,
                timeout=2,
                text=True,
                cwd=self.build_dir
            )
            # NEQN might exit with non-zero for empty input, so we don't check return code
            self.logger.debug("Executable appears to run correctly.")
        except subprocess.TimeoutExpired:
            self.logger.debug("Executable test timed out (may be waiting for input)")
        except FileNotFoundError:
            self.logger.warning("Could not test executable functionality.")
        
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
    
    def clean_build(self) -> bool:
        """
        Clean all build artifacts and intermediate files.
        
        Returns:
            True if cleaning succeeds, False otherwise
        """
        self.logger.info("Cleaning build artifacts...")
        
        # Define all possible artifacts
        artifacts = [
            self.output_executable,
            "a.out",
            "ne",
            "*.o",
            "y.tab.c",
            "y.tab.h",
            f"{self.output_executable}.backup"
        ]
        
        files_removed = []
        files_failed = []
        
        for pattern in artifacts:
            if '*' in pattern:
                # Handle glob patterns
                matching_files = list(self.build_dir.glob(pattern))
                for file_path in matching_files:
                    try:
                        file_path.unlink()
                        files_removed.append(file_path.name)
                    except OSError as e:
                        files_failed.append(f"{file_path.name}: {e}")
            else:
                # Handle individual files
                file_path = self.build_dir / pattern
                if file_path.exists():
                    try:
                        file_path.unlink()
                        files_removed.append(pattern)
                    except OSError as e:
                        files_failed.append(f"{pattern}: {e}")
        
        if files_removed:
            self.logger.info(f"Removed {len(files_removed)} artifacts: {', '.join(files_removed)}")
        else:
            self.logger.info("No artifacts found to remove.")
        
        if files_failed:
            self.logger.warning(f"Failed to remove {len(files_failed)} items:")
            for failure in files_failed:
                self.logger.warning(f"  - {failure}")
            return False
        
        return True
    
    def get_build_info(self) -> Dict[str, str]:
        """
        Get information about the build environment.
        
        Returns:
            Dictionary containing build environment information
        """
        info = {
            "build_dir": str(self.build_dir),
            "grammar_file": self.grammar_file,
            "source_pattern": self.source_pattern,
            "parser_generator": self.parser_generator,
            "compiler": self.compiler,
            "compile_flags": " ".join(self.compile_flags),
            "output_executable": self.output_executable,
            "cleanup_enabled": str(self.cleanup_objects and self.cleanup_generated)
        }
        
        # Add tool versions if available
        for tool in [self.parser_generator, self.compiler]:
            try:
                result = subprocess.run(
                    [tool, "--version"],
                    capture_output=True,
                    text=True,
                    timeout=5
                )
                if result.returncode == 0:
                    version_line = result.stdout.strip().split('\n')[0]
                    info[f"{tool}_version"] = version_line
                else:
                    info[f"{tool}_version"] = "Unknown"
            except (subprocess.TimeoutExpired, FileNotFoundError):
                info[f"{tool}_version"] = "Not found"
        
        # Add source file count
        source_files = list(self.build_dir.glob(self.source_pattern))
        info["source_files_found"] = str(len(source_files))
        
        return info
    
    def build(self) -> bool:
        """
        Execute the complete build process.
        
        Returns:
            True if build succeeds, False otherwise
        """
        self.logger.info("Starting NEQN build process...")
        
        # Backup existing executable if requested
        if not self.backup_existing_executable():
            return False
        
        # Check dependencies
        if not self.check_dependencies():
            return False
        
        # Generate parser from grammar
        if not self.generate_parser():
            return False
        
        # Compile all sources
        if not self.compile_sources():
            return False
        
        # Rename executable if needed
        if not self.rename_executable():
            # Try to continue without renaming if a.out exists
            if (self.build_dir / "a.out").exists():
                self.logger.warning("Using default executable name 'a.out'")
            else:
                return False
        
        # Clean up intermediate files
        if not self.cleanup_intermediate_files():
            self.logger.warning("Build succeeded but cleanup had issues")
        
        # Validate build
        if not self.validate_build():
            return False
        
        self.logger.info("NEQN build completed successfully!")
        return True


def main():
    """
    Main entry point for the NEQN build script.
    """
    parser = argparse.ArgumentParser(
        description="Build script for NEQN equation formatter",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 neqnrc                    # Build NEQN in current directory
  python3 neqnrc --verbose          # Build with verbose output
  python3 neqnrc --clean            # Clean build artifacts
  python3 neqnrc --build-dir /path  # Build in specific directory
  python3 neqnrc --info             # Show build environment info
  python3 neqnrc --no-cleanup       # Skip cleaning intermediate files
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
        "--clean",
        action="store_true",
        help="Clean build artifacts and exit",
    )
    
    parser.add_argument(
        "--no-cleanup",
        action="store_true",
        help="Skip cleaning intermediate files after build",
    )
    
    parser.add_argument(
        "--parser-generator",
        default="yacc",
        help="Parser generator to use (default: yacc)",
    )
    
    parser.add_argument(
        "--compiler",
        default="cc",
        help="C compiler to use (default: cc)",
    )
    
    parser.add_argument(
        "--flags",
        nargs="*",
        help="Additional compiler flags",
    )
    
    parser.add_argument(
        "--info",
        action="store_true",
        help="Show build environment information and exit",
    )
    
    parser.add_argument(
        "--no-backup",
        action="store_true",
        help="Skip creating backup of existing executable",
    )
    
    parser.add_argument(
        "--no-validate",
        action="store_true",
        help="Skip source and grammar file validation",
    )
    
    args = parser.parse_args()
    
    # Create builder instance
    builder = NeqnBuilder(
        build_dir=args.build_dir,
        verbose=args.verbose
    )
    
    # Override settings from command line
    if args.no_cleanup:
        builder.cleanup_objects = False
        builder.cleanup_generated = False
    
    if args.parser_generator:
        builder.parser_generator = args.parser_generator
    
    if args.compiler:
        builder.compiler = args.compiler
    
    if hasattr(args, 'flags') and args.flags:
        builder.compile_flags.extend(args.flags)
    
    if args.no_backup:
        builder.backup_existing = False
    
    if args.no_validate:
        builder.validate_sources = False
        builder.validate_grammar = False
    
    # Handle info operation
    if args.info:
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
    
    # Print completion message (like original script)
    if success:
        print("done")
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main() ne.g
cc -O -n -s ne*.c   
rm -f *.o y.tab.c
echo "done"
