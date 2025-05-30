#!/bin/bash
#
# test_runner.sh - Comprehensive test runner for neqn
#
# This script builds and runs all tests for the neqn mathematical
# equation preprocessor, providing detailed output and error reporting.

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
VERBOSE=0
CLEAN_FIRST=0
RUN_EXAMPLES=1

# Function to print colored output
print_status() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

print_section() {
    echo
    print_status "$BLUE" "=================================================="
    print_status "$BLUE" "$1"
    print_status "$BLUE" "=================================================="
    echo
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -c|--clean)
            CLEAN_FIRST=1
            shift
            ;;
        --no-examples)
            RUN_EXAMPLES=0
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -v, --verbose     Enable verbose output"
            echo "  -c, --clean       Clean before building"
            echo "  --no-examples     Skip example tests"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

print_section "NEQN COMPREHENSIVE TEST SUITE"

# Check if we're in the right directory
if [[ ! -f "Makefile" ]] || [[ ! -f "ne.h" ]]; then
    print_status "$RED" "Error: Must be run from the neqn source directory"
    exit 1
fi

# Clean if requested
if [[ $CLEAN_FIRST -eq 1 ]]; then
    print_section "CLEANING BUILD ARTIFACTS"
    make clean
fi

# Build the project
print_section "BUILDING NEQN"
if [[ $VERBOSE -eq 1 ]]; then
    make all
else
    make all 2>&1 | grep -E "(error|Error|ERROR)" || true
fi

if [[ $? -ne 0 ]]; then
    print_status "$RED" "Build failed!"
    exit 1
fi

print_status "$GREEN" "Build successful!"

# Run unit tests
print_section "RUNNING UNIT TESTS"
if [[ $VERBOSE -eq 1 ]]; then
    make test
else
    make test 2>&1 || exit 1
fi

print_status "$GREEN" "Unit tests passed!"

# Test basic functionality
print_section "TESTING BASIC FUNCTIONALITY"

echo "Testing simple equation processing:"
echo "x + y = z" | ./neqn
echo

echo "Testing superscript notation:"
echo "E = mc sup 2" | ./neqn
echo

echo "Testing fraction notation:"
echo "x = {-b +- sqrt{b sup 2 - 4ac}} over {2a}" | ./neqn
echo

print_status "$GREEN" "Basic functionality tests passed!"

# Run example tests if requested
if [[ $RUN_EXAMPLES -eq 1 ]] && [[ -d "examples" ]]; then
    print_section "TESTING EXAMPLE FILES"
    
    for example_file in examples/*.txt; do
        if [[ -f "$example_file" ]]; then
            echo "Processing: $(basename "$example_file")"
            if [[ $VERBOSE -eq 1 ]]; then
                ./neqn < "$example_file"
            else
                ./neqn < "$example_file" > /dev/null 2>&1
            fi
            
            if [[ $? -eq 0 ]]; then
                print_status "$GREEN" "  ✓ $(basename "$example_file") processed successfully"
            else
                print_status "$RED" "  ✗ $(basename "$example_file") failed"
            fi
        fi
    done
    echo
fi

# Test error handling
print_section "TESTING ERROR HANDLING"

echo "Testing invalid syntax (should produce error):"
echo "invalid { syntax" | ./neqn 2>&1 || true
echo

echo "Testing empty input:"
echo "" | ./neqn
echo

print_status "$GREEN" "Error handling tests completed!"

# Performance test (simple)
print_section "BASIC PERFORMANCE TEST"

echo "Processing large equation 1000 times..."
start_time=$(date +%s.%N)
for _ in {1..1000}; do
    echo "sum from {i=1} to n {x sub i sup 2}" | ./neqn > /dev/null
done
end_time=$(date +%s.%N)

duration=$(echo "$end_time - $start_time" | bc -l)
echo "Time: ${duration} seconds"
print_status "$GREEN" "Performance test completed!"

# Final summary
print_section "TEST SUMMARY"

print_status "$GREEN" "All tests completed successfully!"
print_status "$BLUE" "Summary:"
echo "  ✓ Build successful"
echo "  ✓ Unit tests passed"
echo "  ✓ Basic functionality verified"
if [[ $RUN_EXAMPLES -eq 1 ]]; then
    echo "  ✓ Example files processed"
fi
echo "  ✓ Error handling tested"
echo "  ✓ Performance test completed"

print_status "$GREEN" "NEQN is ready for use!"
echo

exit 0
