# MyCompiler
开发语言：C++
开发环境：Codeblocks 13.12

测试样例目前存在的问题
1.函数定义里面的常量声明和变量声明没有完全覆盖
2.缺少循环语句与情况语句的嵌套使用
3.情况语句中的情况子语句过于简单
4.缺少对实数类型的各种情况的覆盖
5.缺少错误的样例
6.数组下标的表达式不够复杂，要有函数调用的，数组传值嵌套的

对一些未规定问题的处理（更新中）
1.
Q：数组越界怎么处理
A：暂时不做任何处理，好像也没有办法处理

2.
数据类型转换：int可以转float，反之不行

3.
读语句只读普通变量，不读数组

3.运算类型转换表
栈顶数\次栈顶数 int 	float 	char
		int		int 	float 	int
		float	float 	float 	float
		char	int		float	int
		
4.因为在项目中使用了正则表达式，所以应该在编译选项中使用
-std=c++0x or -std=gnu++0x
设置方法；Settings->Compilers->Global Compiler Settings
中选择GNU GCC Compiler,然后在Categories中勾选
Have g++ follow the C++11 ISO language standard...
