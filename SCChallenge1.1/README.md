# Space Cadets Challenge 1 (The fun solution)

[Challenge Reference](https://secure.ecs.soton.ac.uk/student/wiki/w/COMP1202/Space_Cadets/SCChallengeEmail)

Now, I already created a solution to this [here](https://github.com/ImArjunJ/SpaceCadets/tree/master/SCChallenge1) which i WILL be referencing to probably as the "original solution".

Even after making the original solution, I still wasn't satisfied. Hence, I decided to go... lower.

# Explanation

This windows-specific program works using a method known as [IAT Hooking](https://en.wikipedia.org/wiki/Hooking#Internal_IAT_hooking). This method is one of many function hooking methods. It allows me to intercept any function calls that any process internally makes.

In this case, I am specifically working with the default windows notepad application that everyone "loves". To be specific I am hooking two functions. [CreateWindowExW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw) and [DefWindowProcW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowprocw).

The `notepad.exe` executable is launched from the default `System32` directory via a custom injector, this injector launches the process suspended, allowing the CreateWindowExW hook to work before the initial window is created. This allows me to manipulate the default Notepad window. From this, I add a new control within the `File` section of the menu bar called `SpaceCadets` with the Id `0x1337`.

Using the `DefWindowProcW` hook allows me to intercept `WndProc` hence allowing me to see all the window events recieved. This includes the new `0x1337` event, meaning I can detect when its clicked.

From here, open a pipe to retrieve console output buffer as an internal string, calling the original solution to obtain the output.

```c++
std::string Command = "SCChallenge1.exe " + UserId;
```

This is almost done, from this piped output into a string, we need to convert the string into a wide string, this can easily be done using `std::wstring`. Then we write to the notepad edit buffer, populating the "editor" with the obtained output from the main solution.

# Showcase

This might be impossible to get working on many machines without knowledge of how it works, or without low level experience, hence I have created a showcase video to simplify the process and save you the headaches.

![Showcase Video](https://streamable.com/e/x7zl71)

Another thing that you will notice is the fact that a console appears. Once again, this is NOT a separate executable. This is all done IN the notepad.exe executable abusing `AllocConsole` and friends to attach a console and redirect console out and input, hence allowing the user to input a user-id to fetch.

# If you care how IAT works

If you genuinely care about how IAT hooking works, heres an explanation.

The Portable Executable (PE) file format is used by Windows for executables and various other things.

When a Windows program imports functions from a DLL (like user32.dll or kernel32.dll), it uses the Import Address Table (IAT) to manage these imports dynamically. IAT Hooking is a method in which the memory addresses that correspond to each function in the Import Address Table are modified to your own function hence redirecting the flow to your own code. From there you can choose to redirect the code back to the original function after having executed your own code. Or not... depends what you want.

I could spend ages explaining Windows PE, but I wont. To make it simple, the Windows PE file format has various sections. One of which is the IMAGE_DATA_DIRECTORY. This includes structures such as the import, and export table.

Here's what each IAT entry can look like

| Function        | Address    |
| --------------- | ---------- |
| MessageBoxW     | 0x77D834C0 |
| CreateWindowExW | 0x77D78940 |

IAT Hooking is when we modify these addresses to our own needs. In my case I modified the entries for `CreateWindowExW` and `DefWindowProcW` as I mentioned before.

- Locate the IAT: The IAT is located using the Import Directory in the PE header.

  - The Import Descriptor (pointed to by the Import Directory in the Data Directory) contains a list of DLLs and their associated functions.
  - Use IMAGE_IMPORT_DESCRIPTOR to locate the IAT.

- Find the Target Function: Iterate through the IAT to find the function you want to hook. The table contains the addresses of imported functions, which are resolved at runtime.

- Modify the IAT Entry: Once the target function is found, you replace its address in the IAT with the address of your custom function. This allows you to intercept the call and redirect it to your hook.

- Restore the Original Function (or just dont?): You can store the original function pointer so you can call the original function from your hook if needed, hence returning the flow back to the original IAT function which the process intended to use
