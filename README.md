# MyCompiler
开发语言：C++

开发环境：Codeblocks 13.12


对一些未规定问题的处理（更新中）

1.数组越界的处理方式： 暂时不做任何处理，若因此产生了运行时错误，则出错信息由其他语法方面的报错方式报出

2. 数据类型转换：int 可以转 float，反之不行

3.读语句只读普通变量，不读数组

4.运算类型转换表

| 栈顶数\次栈顶数  | int  |  float | char |
| - | :-: | :-: | -: |
| int | int  |  float  | int |
| float | float  | float  | float |
| cahr | int | float  | int |
		
5.因为在项目中使用了正则表达式，所以应该在编译选项中使用 -std=c++0x or -std=gnu++0x

设置方法：Settings->Compilers->Global Compiler Settings 中选择 GNU GCC Compiler, 然后在 Categories 中勾选 Have g++ follow the C++11 ISO language standard...
