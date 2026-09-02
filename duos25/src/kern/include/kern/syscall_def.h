/*
 * Copyright (c) 2022 
 * Computer Science and Engineering, University of Dhaka
 * Credit: CSE Batch 25 (starter) and Prof. Mosaddek Tushar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
#ifndef __SYSCALL_DEF_H
#define __SYSCALL_DEF_H
/*
* Define syscall number 
*/
//Process creation deletion and state
#define SYS_fork         0
#define SYS_vfork        1
#define SYS_execv        2
#define SYS__exit        3
#define SYS_waitpid      4
#define SYS_getpid       5
#define SYS_getppid      6

//(virtual memory)
#define SYS_sbrk         7
#define SYS_mmap         8
#define SYS_munmap       9
#define SYS_mprotect     10

//(security/credentials)
#define SYS_umask        17
#define SYS_issetugid    18
#define SYS_getresuid    19
#define SYS_setresuid    20
#define SYS_getresgid    21
#define SYS_setresgid    22
#define SYS_getgroups    23
#define SYS_setgroups    24
#define SYS___getlogin   25
#define SYS___setlogin   26

// (signals)
#define SYS_kill         27
#define SYS_sigaction    28
#define SYS_sigpending   29
#define SYS_sigprocmask  30
#define SYS_sigsuspend   31
#define SYS_sigreturn    32

//-- File-handle-related --
#define SYS_open         45
#define SYS_pipe         46
#define SYS_dup          47
#define SYS_dup2         48
#define SYS_close        49
#define SYS_read         50
#define SYS_pread        51

#define SYS_getdirentry  54
#define SYS_write        55
#define SYS_pwrite       56

#define SYS_lseek        59
#define SYS_flock        60
#define SYS_ftruncate    61
#define SYS_fsync        62
#define SYS_fcntl        63
#define SYS_ioctl        64
#define SYS_select       65
#define SYS_poll         66

//-- Pathname-related --
#define SYS_link         67
#define SYS_remove       68
#define SYS_mkdir        69
#define SYS_rmdir        70
#define SYS_mkfifo       71
#define SYS_rename       72
#define SYS_access       73

//(current directory)
#define SYS_chdir        74
#define SYS_fchdir       75
#define SYS___getcwd     76

//(symbolic links)
#define SYS_symlink      77
#define SYS_readlink     78

//(mount)
#define SYS_mount        79
#define SYS_unmount      80

//-- Any-file-related --
#define SYS_stat         81
#define SYS_fstat        82
#define SYS_lstat        83

//(timestamps)
#define SYS_utimes       84
#define SYS_futimes      85
#define SYS_lutimes      86

//(security/permissions)
#define SYS_chmod        87
#define SYS_chown        88
#define SYS_fchmod       89
#define SYS_fchown       90
#define SYS_lchmod       91
#define SYS_lchown       92

//-- Sockets and networking --
#define SYS_socket       98
#define SYS_bind         99
#define SYS_connect      100
#define SYS_listen       101
#define SYS_accept       102
#define SYS_shutdown     104
#define SYS_getsockname  105
#define SYS_getpeername  106
#define SYS_getsockopt   107
#define SYS_setsockopt   108

//-- Time-related --
#define SYS___time       113
#define SYS___settime    114
#define SYS_nanosleep    115

//-- Other reboot scheduling etc. --
#define SYS_sync         118
#define SYS_reboot       119
#define SYS_yield        120	

#endif /*End of SYSCALL_DEF_H */
