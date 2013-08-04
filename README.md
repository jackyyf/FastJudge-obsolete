FastJudge
=========

  FastJudge is a judge kernel under linux. Unlike many other judges, FastJudge does not use ptrace to trace system call. Instead, it uses [chroot](http://linux.die.net/man/2/chroot) for filesystem control, [setuid](http://linux.die.net/man/2/setuid)/[setgid](http://linux.die.net/man/2/setgid) for permission control, [setrlimit](http://linux.die.net/man/2/setrlimit) for limiting resources like CPU and Memory. No ptrace means no cost for switching between judge and the running problem.

  Any issues about this project, please submit an issue or send an email to [root@jackyyf.com](mailto:root@jackyyf.com)
