#include <iostream> // For basic output/errors
#include "roff_context.hpp"       // For the new RoffContext
#include "roff.hpp" // For legacy functions and init_globals

// Forward declare a conceptual main roff processing loop if its actual name isn't known
// or if it's not yet refactored to take RoffContext.
// This function would ideally be in the otroff::roff_legacy namespace.
// namespace otroff { namespace roff_legacy {
// void start_legacy_roff_processing(otroff::roff::RoffContext& context);
// }}

int main(int argc, char *argv[]) {
    // 1. Create the RoffContext instance
    otroff::roff::RoffContext roff_context;

    // Store argc and argv in the context if they are needed globally by legacy code
    // (This part of refactoring legacy globals like 'argc' and 'argp' from roff8.cpp
    // would eventually move them into RoffContext)
    // For now, we assume they might be set directly if init_globals still uses them,
    // or set on context if context is already designed for it.
    // roff_context.argc = argc;
    // roff_context.argv = argv;

    std::cout << "Legacy Troff Executable (troff_exe) - Initializing..." << std::endl;

    // 2. Call legacy init_globals (for globals not yet in RoffContext).
    // This is a temporary step. As RoffContext takes over all globals,
    // its constructor will handle all initialization.
    // The legacy init_globals also sets up pointers to global static arrays (word, line etc.)
    // which RoffContext now owns as std::array and provides pointers to.
    // We need to be careful here. For now, let's assume RoffContext's constructor
    // has initialized the subset of globals it owns.
    // The original init_globals might try to re-initialize things or expect C-style arrays.

    // Call the namespaced init_globals
    otroff::roff_legacy::init_globals();

    // If legacy argc/argv globals are still used by init_globals or other parts:
    // Note: These are still global in otroff::roff_legacy for now.
    // This will be resolved when argc/argv are moved to RoffContext.
    // Note: argc/argp not defined in roff_legacy namespace
    // otroff::roff_legacy::argc = argc; // Example of direct global access (still legacy)
    // otroff::roff_legacy::argp = argv; // Example of direct global access (still legacy)


    // 3. Conceptual call to a top-level processing function.
    //    This function would need to be refactored to accept `roff_context`.
    //    For now, we'll just print a message.
    //
    // Example (if refactored):
    // otroff::roff_legacy::start_legacy_roff_processing(roff_context);

    std::cout << "Legacy Roff System initialized with RoffContext (partially)." << std::endl;
    std::cout << "Page number from context: " << roff_context.pn << std::endl;
    std::cout << "Line length from context: " << roff_context.ll << std::endl;

    // Placeholder for actual processing loop using legacy functions
    // For example, a simplified command loop:
    // while (true) {
    //   int cmd_char = otroff::roff_legacy::getch(); // Needs context if refactored
    //   if (cmd_char == 0 || cmd_char == EOF) break;
    //   // ... process command ...
    //   // Call a refactored command dispatcher:
    //   // dispatch_command_contextual(roff_context, cmd_char);
    // }

    std::cout << "Processing would occur here." << std::endl;
    std::cout << "Legacy Troff Executable finished." << std::endl;

    return 0;
}
