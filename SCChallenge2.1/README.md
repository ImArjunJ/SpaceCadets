# Space Cadets Challenge 2 (Version 2)

[Challenge Reference](https://secure.ecs.soton.ac.uk/student/wiki/w/COMP1202/Space_Cadets/SCChallengeBareBones)

This is an interpreter Bare Bones language outlined in [Computer Science: An Overview](http://www.amazon.co.uk/Computer-Science-Overview-Glenn-Brookshear/dp/0321544285/ref=sr_1_1?ie=UTF8&s=books&qid=1225741559&sr=8-1).

However, this is not a normal "interpreter". It is more similar to a JIT compiler. This is because it generates assembly machine code at runtime, and then proceeds to execute it. This is done by allocating a 1 page RWX region in memory, and writing the relevant assembly code to it based on the input Bare Bones code.

For example, for the `incr` instruction, the relevant x64 assembly for it is `add dword ptr [rcx+offset], 1`

These assembly instructions need to be represented in bytecode rather than just assembly "text", hence the existence of mysterious bytecode arrays in my code. I have tried to comment where relevant, but if anything is unclear let me know.

If you're surprised that this works... well me too man.

Also, this is how C# works. It uses JIT compilation since the geniuses at microsoft decided oh wait lets make the .NET language family (including C#) compile to MSIL their intermediate language. Oh and wait, lets also slap on a JIT compiler so it actually functions.

Anyways, enjoy the continuation of my dubious solutions.

# Compilation

To compile:

```bash
cmake -B build
cmake --build build
```

That should be it, if you dont have build tools installed, then unfortunate. Also this program will only work on windows due to its VirtualAlloc and etc dependencies. However, it's not that hard to port to POSIX.
