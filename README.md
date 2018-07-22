# Smol Moves
A lightweight game engine and series of sample games in C89 &amp; OpenGL

# Smol
The lightweight game engine Smol is part of this repo and is meant as an excercize in game engine design and architecture.
As of writing this the rational behind Smol is an engine that could be easily supported by HTML5 via EMScripten and Javascript Workers, to achieve this the multi-threadeing capacity of the engine takes the form of isolated environments with controlled communication.

These isolated environments take the form of a javascript scripting envrionment, and a message queue. The environments as of writing come in 3 flavours;

### The system environment
Handles rendering, input, networking and anything that may require platform specific features, this is not scriptable with no javascript environment available;

### The logic environment
Handles simulation and physics and as far users are concerned is the entry point of deployed programs, this is partially scriptable, with a javascript environment available but with limited access to the physics;

### The helper environment
Handles asynchronous processing such as unpacking assets, or simulations which are not worth holding up the logic for, this is almost entirely scriptable, with the intention being all loading needs are determined and handled here;

# Sample Games
The sample games are meant to demonstrate ease of use and capabilities of the engine, at time of writing none are currently made but one is targetted, these samples should highlight missing or malformed features during development and are as much a part of the development as that of the engine itself.

## Atlas
Atlas was a game I had previously developped as an exercise in game engine development for my end of third year university project. Atlas was a multiplayer ateroids, with a limitation of being a single stick shooter, it would emulate the vectrex style and be an exercise into C++ engine development and practices, as well as game design and juice.

The brief as of writing, is a variation of that game, with a publisher friendly goal in mind, as a savy portofolio piece. Atlas will be an exercise in single stick 2 button game design, with a focus on customizable ships, a procedural world and rogue like progression. Its unique aspect being a mixture of story telling and grandiose roaming boss battles.

# Build
Building requires GCC and Doxygen in path, building includes documentation generation and copying of anythign in the resources folder.

To build everything from scratch, this includes a clean step.

    make all

To build incrementally.

    make build

To build for debugging.

    make DEBUG=TRUE all

Or incrementally.

    make DEBUG=TRUE build

To run in the gnu debugger.

    make debug

Finally ofcourse, to clear what has been built, this is already a step in make all.

    make clean
