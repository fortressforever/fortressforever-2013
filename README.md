# Fortress Forever (Source SDK 2013)

## Git

We will be using the branching model laid out here: http://nvie.com/posts/a-successful-git-branching-model/

* __TODO__ Specific instructions on how to work within that model (information about when to use rebase, when to merge, etc)

### Committing

* Always make sure the code you are committing compiles
* Try to commit features and changes separately, rather than 20 files at once. This means we can easily revert certain pieces if we don't like them, rather than going through manually to remove stuff
* Be descriptive in your revision comments.
* If you aren't sure you're doing something the best way, comment it in code and possibly comment ("First pass, needs cleaning up!" for e.g.)

## Code

### Syntax

#### Variable, Function, File Names and Directory Structure

* __TODO__ Decide on a naming convention
* Always make variable and function names as descriptive as possible (if 'i' stands for 'currentPlayerIndex' then use 'currentPlayerIndex')

### Adding/Removing Files

Solution and makefiles are no longer stored on the repo, they are generated using [VPC](https://developer.valvesoftware.com/wiki/Valve_Project_Creator). To add/remove files from the project, you must edit game/client/client_ff.vpc and/or game/server/server_ff.vpc files and then execute createallprojects(.bat) in the src/ directory. To remove a non-FF-specific file from the project (like HL2DM files), add exactly what you would to add the file (or copy the line from the .vpc that includes it), but put a - before "$File".

## Documentation

### DOCUMENTATION.md

For documentation of general Source engine things or of implemented features, commit to the develop branch. For each feature branch, document the in-development feature as it is worked on; the documentation will get merged along with the feature once it is complete.

### Doxygen Inline Commenting

Note: only use Doxygen commenting *as necessary* (see section *Variable, Function, File Names and Directory Structure*)

To describe classes/functions/files ([List of available @ commands](http://www.stack.nl/~dimitri/doxygen/manual/commands.html#cmdreturns)):

    /// Brief description. (optional)
    /** Detailed description. 

        @param parameterName Description of the param
        @returns Description of the return value
    */

To describe member variables:

    int var; ///< Detailed description of the member variable
             ///< and more if needed