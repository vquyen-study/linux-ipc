# Inter Process Communication on Unix

## 1. Build on windows.
* By default the cmake generate nmake... It is neccessary to reconfig to makefile for gnu make as bellow.

```shell
cmake -S . -B build -G "Unix Makefiles"
```

## 2. IPC Overview

> - Taxonomy of Unix IPC facilities.
> 
> ![Image](_repo\Unix-IPC.jpg "Taxonomy of Unix IPC facilities")

> - Identifies and handles for various of IPC > facilities.
> 
> ![Image](_repo\Identifier-and-handles-for-IPC-facilities.JPG "Identifiers & Handles")

> - Accessibility and persistence for IPC.
> 
> ![Image](_repo\accessibility-persistence-ipc-facilities.JPG "Identifiers & Handles")



## 10. NOTES
> - **Not01: Running IPC on Cygwin** Be careful when using cygwin to test IPCs solution. By default the cygwin does not start some service, it is resulted in lack of service to serve ipc request so errors will be occurred during test. It is best to test on native unix evironment like linux.
> - **So01:** Starting cygserver service on cigwin to serve ipc service. It is required following steps.
> ```Shell
> ### This command to configuration cygserver service.
> $ /usr/bin/cygserver-config       # After running it produce a configuration for cygserver at /etc/cygserver.conf
> 
>  ###The service has been installed under LocalSystem account.
>  ###To start it, call `net start cygserver' or `cygrunsrv -S cygserver'.
> net start cygserver
> ```