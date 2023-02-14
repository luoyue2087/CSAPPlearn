long mult2(long, long); 
void multstore(long x, long y, long *dest) { 
    long t = mult2(x, y); 
    *dest = t; 
}
/*
生成该文件的同名汇编代码文件
gcc -Og -S mstore.c
该文件中包含
multstore:
	pushq	%rbx
	movq	%rdx, %rbx
	call	mult2@PLT
	movq	%rax, (%rbx)
	popq	%rbx
	ret


生成该文件的同名目标代码文件(二进制格式)
gcc -Og -c mstore.c

该文件中包含
53 48 89 d3 e8 00 00 00 00 48 89 03 5b c3
目标文件的内容可以得出：
机器执行的程序只是一个字节序列，他是对一系列指令的编码


将生成的目标代码文件反汇编出汇编语言代码（会在终端中显示结果）
objdump -d mstore.o 

终端输出
Disassembly of section .text:
0000000000000000 <multstore>:
   0:   53                      push   %rbx
   1:   48 89 d3                mov    %rdx,%rbx
   4:   e8 00 00 00 00          callq  9 <multstore+0x9>
   9:   48 89 03                mov    %rax,(%rbx)
   c:   5b                      pop    %rbx
   d:   c3                      retq   
*/