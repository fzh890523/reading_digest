linux C一站式学习读书笔记_yonka

#INDEX

* [Introduction](#Introduction)
* [Types and interfaces](#Types and interfaces)
* [The representation of an interface](#The representation of an interface)
* [The laws of reflection](#The laws of reflection)
    * [The first law of reflection](#The first law of reflection)
    * [The second law of reflection](#The second law of reflection)
    * [The third law of reflection](#The third law of reflection)
* [Structs](#Structs)
* [Conclusion](#Conclusion)

#变量和作用域

C中变量有： `局部变量` 和 `全局变量`

**局部变量**
1. 一个函数中定义的变量不能被另一个函数使用。
2. 每次调用函数时局部变量都表示不同的存储空间。局部变量在每次函数调用时分配存储空间，在每次函数返回时释放存储空间。

**全局变量**

定义在所有的函数体之外，它们在程序开始运行时分配存储空间，在程序结束时释放存储空间，在任何函数中都可以访问全局变量
> 正因为全局变量在任何函数中都可以访问，所以在程序运行过程中全局变量被读写的顺序从源代码中是看不出来的，源代码的书写顺序并不能反映函数的调用顺序。程序出现了Bug往往就是因为在某个不起眼的地方对全局变量的读写顺序不正确。因此，**虽然全局变量用起来很方便，但一定要慎用，能用函数传参代替的就不要用全局变量**。

##变量的初始化

**局部变量可以用类型相符的任意表达式来初始化，而全局变量只能用常量表达式（Constant Expression）初始化**
例如，全局变量pi这样初始化是合法的：
```C
double pi = 3.14 + 0.0016;
```
但这样初始化是不合法的：
```C
double pi = acos(-1.0);
```
然而局部变量这样初始化却是可以的。
因为：
> 程序开始运行时要用适当的值来初始化全局变量，所以初始值必须保存在编译生成的可执行文件中，因此初始值在编译时就要计算出来，然而上面第二种Initializer的值必须在程序运行时调用acos函数才能得到，所以不能用来初始化全局变量。

*为了简化编译器的实现，C语言从语法上规定全局变量只能用常量表达式来初始化*，因此下面这种全局变量初始化是不合法的：
```C
int minute = 360;
int hour = minute / 60;
```
虽然在编译时计算出hour的初始值是可能的，但是minute / 60不是常量表达式，不符合语法规定，所以编译器不必想办法去算这个初始值。

##变量的初始值

**如果全局变量在定义时不初始化则初始值是0，如果局部变量在定义时不初始化则初始值是不确定的**。所以，***局部变量在使用之前一定要先赋值***，如果基于一个不确定的值做后续计算肯定会引入Bug。


#函数

##函数声明

如void threeline(void)这一行，声明了一个函数的名字、参数类型和个数、返回值类型，这称为函数原型。在代码中可以单独写一个函数原型，后面加;号结束，而不写函数体，例如：
```C
void threeline(void);

// 或者
int absolute_value(int x);
// 或者
int absolute_value(int);
// 形参参数名可以省，但分号不能省
```
这种写法只能叫函数声明而不能叫函数定义，只有带函数体的声明才叫定义。
只有分配存储空间的变量声明才叫变量定义，其实函数也是一样，编译器只有见到函数定义才会生成指令，而指令在程序运行时当然也要占存储空间。
那么没有函数体的函数声明有什么用呢？它为编译器提供了有用的信息，编译器在翻译代码的过程中，只有见到函数原型（不管带不带函数体）之后才知道这个函数的名字、参数类型和返回值，这样碰到函数调用时才知道怎么生成相应的指令，所以**函数原型必须出现在函数调用之前，这也是遵循“先声明后使用”的原则**。

如：
```C
#include <stdio.h>

void newline(void);
void threeline(void);

int main(void)
{
	...
}

void newline(void)
{
	...
}

void threeline(void)
{
	...
}
```

> **由于有Old Style C语法的存在，并非所有函数声明都包含完整的函数原型**，例如void threeline();这个声明并没有明确指出参数类型和个数，所以不算函数原型，这个声明提供给编译器的信息只有函数名和返回值类型。如果在这样的声明之后调用函数，编译器不知道参数的类型和个数，就不会做语法检查，所以很容易引入Bug。了解这个知识点以便维护别人用Old Style C风格写的代码，但绝不应该按这种风格写新的代码。

###隐式声明
C允许但不建议 *隐式声明*。如：
```C
#include <stdio.h>

int main(void)
{
	printf("Three lines:\n");
	threeline();  // 编译器认为此处隐式声明了int threeline(void)
	printf("Another three lines.\n");
	threeline();
	return 0;
}

void newline(void)
{
	printf("\n");
}

void threeline(void)  // 看到threeline函数的原型是void threeline(void)，和先前的隐式声明的返回值类型不符，所以报警告
{
	newline();
	newline();
	newline();
}
```
```shell
编译时会报警告：

$ gcc main.c
main.c:17: warning: conflicting types for ‘threeline’
main.c:6: warning: previous implicit declaration of ‘threeline’ was here
```


##main函数

###标准main函数

其实操作系统在调用main函数时是传参数的，main函数最标准的形式应该是**int main(int argc, char \*argv[])**。
C标准也允许**int main(void)**这种写法，如果不使用系统传进来的两个参数也可以写成这种形式。
但除了这两种形式之外，定义main函数的其它写法都是错误的或不可移植的。

##return
* return 1 或 return(1) 都支持。
* 函数的返回值不是左值，或者说函数调用表达式不能做左值，因此下面的赋值语句是非法的：`is_even(20) = 1;`
* 按值传递： 即便返回语句写成return x;，返回的也是变量x的值，而非变量x本身，因为变量x马上就要被释放了
* 对于return后的dead code编译器不会报错； 同时对于if中未涉及的条件也不会报错（此时返回不确定值）

```C
int absolute_value(int x);

/* main: generate some simple output */
int main(void)
{
    printf("absolute_value of (%d) is %d\n", -5, absolute_value(-5));  // 5
    printf("absolute_value of (%d) is %d\n", 0, absolute_value(0));  // 28  = =，估计与编译相关，如果还有其他语句的话可能不是28
    return 0;
}

int absolute_value(int x)
{
    if (x < 0) {
        return -x;
    } else if (x > 0) {
        return x;
    }
```

##形参和实参
* C语言的传参规则是Call by Value，按值传递


#控制语句

##分支语句

###if（else）
**dangling-else问题**
类似`if (A) if (B) C; else D;`形式的语句怎么理解呢？可以理解成
```C
if (A)
	if (B)
		C;
else
	D;
```
也可以理解成
```C
if (A)
	if (B)
		C;
	else
		D;
```
> C语言规定，else总是和它上面最近的一个if配对，因此应该理解成else和if (B)配对，也就是按第二种方式理解

###switch
格式：
```C
switch (控制表达式) {
case 常量表达式： 语句列表
case 常量表达式： 语句列表
...
default： 语句列表
}
```
* 默认fallthrough，如果不希望的话需要显式break

switch语句不是必不可缺的，显然可以用一组if ... else if ... else if ... else ...代替，但是一方面用switch语句会使代码更清晰，另一方面，有时候编译器会对switch语句进行整体优化，使它比等价的if/else语句所生成的指令效率更高。

##循环语句

###while（do/while）

###for

###break、continue


###goto、label
goto语句过于强大了，从程序中的任何地方都可以无条件跳转到任何其它地方，只要在那个地方定义一个标号就行，唯一的限制是goto只能跳转到同一个函数中的某个标号处，而不能跳到别的函数中。
滥用goto语句会使程序的控制流程非常复杂，可读性很差。著名的计算机科学家Edsger W. Dijkstra最早指出编程语言中goto语句的危害，提倡取消goto语句。goto语句不是必须存在的，显然可以用别的办法替代。
```C
for (...)
	for (...) {
		...
		if (出现错误条件)
			goto error;
	}
error:
	出错处理;

// 可以改写为

int cond = 0; /* bool variable indicating error condition */
for (...) {
	for (...) {
		...
		if (出现错误条件) {
			cond = 1;
			break;
		}
	}
	if (cond)
		break;
}
if (cond)
	出错处理;
```

switch-case结构中的case/default后面也有:，实际上也是特殊的标号。

与标号有关的语法规则：
* 语句 → 标识符: 语句
* 语句 → case 常量表达式: 语句
* 语句 → default: 语句

#数据类型

##数据类型标志

##枚举
如：
```C
enum coordinate_type { RECTANGULAR, POLAR };
struct complex_struct {
	enum coordinate_type t;
	double a, b;
};
```
成员为常量，值由编译器自动分配。 默认从0开始分配，如果想改变该行为的话，可以：
```C
enum coordinate_type { RECTANGULAR = 1, POLAR };  // 从1开始分配
```
**枚举成员并不在单独的命名空间中，所以不能和外部其他变量/常量重名**

```C
int main(void)
{
	enum coordinate_type { RECTANGULAR = 1, POLAR };
	int RECTANGULAR;  // 冲突
	printf("%d %d\n", RECTANGULAR, POLAR);
	return 0;
}
```

##结构体

###嵌套结构体
如：
```C
struct segment {
	struct complex_struct start;
	struct complex_struct end;
};
```

####嵌套结构体的初始化
初始化，可以嵌套初始化，如`struct segment s = {{ 1.0, 2.0 }, { 4.0, 6.0 }};`;
也可以平坦初始化，如`struct segment s = { 1.0, 2.0, 4.0, 6.0 };`;
甚至可以两者混用，如`struct segment s = {{ 1.0, 2.0 }, 4.0, 6.0 };`，但可读性很差，应尽量避免。

C99中引入新特性memberwise initialization，可以对部分成员做初始化，如 `struct segment s = { .start.x = 1.0, .end.x = 2.0 };`

##数组
C中声明数组类型变量的方式是 ${type} ${varname}[${length}]，type可以是基本类型，也可以是结构体类型，如：
```C
int count[4];  // 四个元素在空间上连续

struct complex_struct {
	double x, y;
} a[4];  // ...在空间上连续
```

也可以把数组嵌入到结构体：
```C
struct {
	double x, y;
	int count[4];
} s;
```

**数组越界问题**
C编译器并不检查（即使明显的）数组越界。

###数组初始化
```C
int count[4] = { 3, 2, };  // 剩余的用零值初始化，这里为0

int count[] = { 3, 2, 1, };  // 未指明长度的根据实际元素数量确定长度

int count[4] = { [2] = 3 };  // C99新特性： memberwise initialization
```

**数组不能像结构体那样相互赋值或初始化**
```C
int a[5] = { 4, 3, 2, 1 };
int b[5] = a;  // 不合法

a = b;  // 不合法
```
***因此不能用数组作为参数或者返回值***
**数组类型做右值使用时，自动转换成指向数组首元素的指针**
```C
void foo(int \*a)
{
	...
}

int array[5] = {0};
foo(array);  // 合法，但实际传入的的是 *int
```

###多维数组
C的多维数据是“真”多维数组。 而java等高级语言里是“伪”多维数组，比如java的 int\[3\]\[2\] 中 \[0\]\[1\] 中存的是指向 int[2]类型数组的引用。
```C
int a[3][2] = { 1, 2, 3, 4, 5 };  // [0][0],[0][1]: 1,2, [1][0],[1][1]: 3,4, [2][0],[2][1]: 5,0 ---> row-major，有些语言如fortran中式column-major
// 或者
int a[][2] = { { 1, 2 },
		{ 3, 4 },
		{ 5, } };
```

*对于char[][]，也可以用字符串字面量来做初始化*，如：
```C
char days[8][10] = { "", "Monday", "Tuesday",
			     "Wednesday", "Thursday", "Friday",
			     "Saturday", "Sunday" };

```

###变长数组
VLA： variable length array
在数组声明时使用变量，C99新特性，很多编译器不支持，不建议使用。

##字符串
* C字符串实际就是byte array
* C字符串用\0来表示字符串结束，而并没有长度“属性”，使得获取长度为O(n)操作
* 因为上一点，所以C字符串中无法包含\0
* 和很多语言一样，C中的字符串也不能修改。 允许按索引访问（只读）
* 和数组一样，作为右值使用时自动转为指向首元素（byte）的指针

###字符串初始化
```C
char str[10] = "Hello"
// 相当于
char str[10] = { 'H', 'e', 'l', 'l', 'o', '\0' };  // 后面四个元素初始化为0，也即\0
// str这个字符数组和字符串的区别是，str可以修改

// 如果初始化长度超过数组长度的话，编译器会给警告
char str[10] = "Hello, World.\n"  // 警告
char str[] = "Hello, world.\n"  // OK

// 如果字符串长度正好比数组长度大1（也即不包括最后的\0就正好相等）的话，是允许的，此时最后的\0将被截掉
// [C99 Rationale]说这样规定是为程序员方便，以前的很多编译器都是这样实现的，不管它有理没理，C标准既然这么规定了我们也没办法，只能自己小心了。
char str[14] = "Hello, world.\n";  // OK
```

###字符串的打印
```C
printf("string: %s\n", "hello")
//
printf("string: %s\n", str)  // str 为 char array
```
**注意如果此时str中没有最后的\0的话，会一直遍历下去，导致发生数组越界...，结合前面的“允许截断”的场景 = =**

##浮点数
（由于浮点数规范原因）浮点型的精度有限，不适合用==运算符做精确比较。
```C
double i = 20.0;
double j = i / 7.0;
if (j * 7.0 == i)
	printf("Equal.\n");
else
	printf("Unequal.\n");
// 输出： Unequal (也许会依赖平台）
```

##结构体



#运算符

##关系运算符

##逻辑运算符
AND &&
OR ||
NOT !

#C标准
C标准主要由两部分组成，一部分描述C的语法，另一部分描述C标准库。C标准库定义了一组标准头文件，每个头文件中包含一些相关的函数、变量、类型声明和宏定义。要在一个平台上支持C语言，不仅要实现C编译器，还要实现C标准库，这样的实现才算符合C标准。不符合C标准的实现也是存在的，例如很多单片机的C语言开发工具中只有C编译器而没有完整的C标准库。

#标准库
在Linux平台上最广泛使用的C函数库是glibc，其中包括C标准库的实现，也包括本书第三部分介绍的所有系统函数。几乎所有C程序都要调用glibc的库函数，所以glibc是Linux平台C程序运行的基础。glibc提供一组头文件和一组库文件，最基本、最常用的C标准库函数和系统函数在libc.so库文件中，几乎所有C程序的运行都依赖于libc.so，有些做数学计算的C程序依赖于libm.so，以后我们还会看到多线程的C程序依赖于libpthread.so。以后我说libc时专指libc.so这个库文件，而说glibc时指的是glibc提供的所有库文件。
```
gcc默认是使用libc.so，也即类似 -lc 选项。 如果还使用其他库的话，需要显式指出，类似： gcc main.c -lm 来指示使用libm.so库。
```
glibc并不是Linux平台唯一的基础C函数库，也有人在开发别的C函数库，比如适用于嵌入式系统的uClibc。

#C风格

##缩进和空白
* 关键字if、while、for与其后的控制表达式的(括号之间插入一个空格分隔，但括号内的表达式应紧贴括号
* 双目运算符的两侧各插入一个空格分隔，单目运算符和操作数之间不加空格，例如i␣=␣i␣+␣1、++i、!(i␣<␣1)、-x、&a[1]等。
* 后缀运算符和操作数之间也不加空格，例如取结构体成员s.a、函数调用foo(arg1)、取数组成员a[i]。
* ,号和;号之后要加空格，这是英文的书写习惯，例如for␣(i␣=␣1;␣i␣<␣10;␣i++)、foo(arg1,␣arg2)。
* 以上关于双目运算符和后缀运算符的规则并没有严格要求，有时候为了突出优先级也可以写得更紧凑一些，例如for␣(i=1;␣i<10;␣i++)、distance␣=␣sqrt(x*x␣+␣y*y)等。但是省略的空格一定不要误导了读代码的人，例如a||b␣&&␣c很容易让人理解成错误的优先级。
* 由于UNIX系统标准的字符终端是24行80列的，接近或大于80个字符的较长语句要折行写，折行后用空格和上面的表达式或参数对齐
```C
if␣(sqrt(x*x␣+␣y*y)␣>␣5.0
    &&␣x␣<␣0.0
    &&␣y␣>␣0.0)
// 再比如：
foo(sqrt(x*x␣+␣y*y),
    a[i-1]␣+␣b[i-1]␣+␣c[i-1])
```
* 较长的字符串可以断成多个字符串然后分行书写
```C
printf("This is such a long sentence that "
   "it cannot be held within a line\n");
```

内核代码风格关于缩进的规则有以下几条
* 要用缩进体现出语句块的层次关系，使用Tab字符缩进，不能用空格代替Tab
* if/else、while、do/while、for、switch这些可以带语句块的语句，语句块的{或}应该和关键字写在同一行，用空格隔开，而不是单独占一行
```C
// 例如应该这样写：
if␣(...)␣{
       // →语句列表
}␣else␣if␣(...)␣{
       // →语句列表
}
// 但很多人习惯这样写：
if␣(...)
{
       // →语句列表
}
else␣if␣(...)
{
       // →语句列表
}
// 内核的写法和[K&R]一致，好处是不必占太多行，使得一屏能显示更多代码。这两种写法用得都很广泛，只要在同一个项目中能保持统一就可以了。
```
* 函数定义的{和}单独占一行，这一点和语句块的规定不同
```C
// 如：
int␣foo(int␣a,␣int␣b)
{
       // →语句列表
}
```
* switch和语句块里的case、default对齐写，也就是说语句块里的case、default标号相对于switch不往里缩进，但标号下的语句要往里缩进。
```
// 如：
      →switch␣(c)␣{
      →case 'A':
      →       →语句列表
      →case 'B':
      →       →语句列表
      →default:
      →       →语句列表
      →}
```
* 用于goto语句的自定义标号应该顶头写不缩进，而不管标号下的语句缩进到第几层
* 代码中每个逻辑段落之间应该用一个空行分隔开。例如每个函数定义之间应该插入一个空行，头文件、全局变量定义和函数定义之间也应该插入空行
* 一个函数的语句列表如果很长，也可以根据相关性分成若干组，用空行分隔。这条规定不是严格要求，通常把变量定义组成一组，后面加空行，return语句之前加空行，
```
int main(void)
{
       →int    →a, b;
       →double →c;

       →语句组1

       →语句组2

       →return 0;
}
```

##注释

###单行注释
单行注释应采用`/*␣comment␣*/`的形式，用空格把界定符和文字分开。


###多行注释
多行注释最常见的是这种形式：
```
/*
␣*␣Multi-line
␣*␣comment
␣*/
```
也有更花哨的形式：
```
/*************\
* Multi-line  *
* comment     *
\*************/
```

###不同场合的注释
* 整个源文件的顶部注释。说明此模块的相关信息，例如文件名、作者和版本历史等，顶头写不缩进
```C
/*
 *  kernel/sched.c
 *
 *  Kernel scheduler and related syscalls
 *
 *  Copyright (C) 1991-2002  Linus Torvalds
 *
 *  1996-12-23  Modified by Dave Grothe to fix bugs in semaphores and
 *              make semaphores SMP safe
 *  1998-11-19  Implemented schedule_timeout() and related stuff
 *              by Andrea Arcangeli
 *  2002-01-04  New ultra-scalable O(1) scheduler by Ingo Molnar:
 *              hybrid priority-list and round-robin design with
 *              an array-switch method of distributing timeslices
 *              and per-CPU runqueues.  Cleanups and useful suggestions
 *              by Davide Libenzi, preemptible kernel bits by Robert Love.
 *  2003-09-03  Interactivity tuning by Con Kolivas.
 *  2004-04-02  Scheduler domains code by Nick Piggin
 */
```
* 函数注释。说明此函数的功能、参数、返回值、错误码等，写在函数定义上侧，和此函数定义之间不留空行，顶头写不缩进。
* 相对独立的语句组注释。对这一组语句做特别说明，写在语句组上侧，和此语句组之间不留空行，与当前语句组的缩进一致。
* 代码行右侧的简短注释。对当前代码行做特别说明，一般为单行注释，和代码之间至少用一个空格隔开，一个源文件中所有的右侧注释最好能上下对齐。
```C
/**
 *      radix_tree_insert    -    insert into a radix tree
 *      @root:          radix tree root
 *      @index:         index key
 *      @item:          item to insert
 *
 *      Insert an item into the radix tree at position @index.
 */
int radix_tree_insert(struct radix_tree_root *root,
                        unsigned long index, void *item)
{
        struct radix_tree_node *node = NULL, *slot;
        unsigned int height, shift;
        int offset;
        int error;

        /* Make sure the tree is high enough.  */
        if ((!index && !root->rnode) ||
                        index > radix_tree_maxindex(root->height)) {
                error = radix_tree_extend(root, index);
                if (error)
                        return error;
        }

        slot = root->rnode;
        height = root->height;
        shift = (height-1) * RADIX_TREE_MAP_SHIFT;

        offset = 0;                     /* uninitialised var warning */
        do {
                if (slot == NULL) {
                        /* Have to add a child node.  */
                        if (!(slot = radix_tree_node_alloc(root)))
                                return -ENOMEM;
                        if (node) {
                                node->slots[offset] = slot;
                                node->count++;
                        } else
                                root->rnode = slot;
                }

                /* Go a level down */
                offset = (index >> shift) & RADIX_TREE_MAP_MASK;
                node = slot;
                slot = node->slots[offset];
                shift -= RADIX_TREE_MAP_SHIFT;
                height--;
        } while (height > 0);

        if (slot != NULL)
                return -EEXIST;

        BUG_ON(!node);
        node->count++;
        node->slots[offset] = item;
        BUG_ON(tag_get(node, 0, offset));
        BUG_ON(tag_get(node, 1, offset));

        return 0;
}
```
`函数内的注释要尽可能少用。写注释主要是为了说明你的代码“能做什么”（比如函数接口定义），而不是为了说明“怎样做”，只要代码写得足够清晰，“怎样做”是一目了然的，如果你需要用注释才能解释清楚，那就表示你的代码可读性很差，除非是特别需要提醒注意的地方才使用函数内注释。`
* 复杂的结构体定义比函数更需要注释。
```C
/*
 * This is the main, per-CPU runqueue data structure.
 *
 * Locking rule: those places that want to lock multiple runqueues
 * (such as the load balancing or the thread migration code), lock
 * acquire operations must be ordered by ascending &runqueue.
 */
struct runqueue {
        spinlock_t lock;

        /*
         * nr_running and cpu_load should be in the same cacheline because
         * remote CPUs use both these fields when doing load calculation.
         */
        unsigned long nr_running;
#ifdef CONFIG_SMP
        unsigned long cpu_load[3];
#endif
        unsigned long long nr_switches;

        /*
         * This is part of a global counter where only the total sum
         * over all CPUs matters. A task can increase this counter on
         * one CPU and if it got migrated afterwards it may decrease
         * it on another CPU. Always updated under the runqueue lock:
         */
        unsigned long nr_uninterruptible;

        unsigned long expired_timestamp;
        unsigned long long timestamp_last_tick;
        task_t *curr, *idle;
        struct mm_struct *prev_mm;
        prio_array_t *active, *expired, arrays[2];
        int best_expired_prio;
        atomic_t nr_iowait;
#ifdef CONFIG_SMP
        struct sched_domain *sd;

        /* For active balancing */
        int active_balance;
        int push_cpu;

        task_t *migration_thread;
        struct list_head migration_queue;
        int cpu;
#endif
#ifdef CONFIG_SCHEDSTATS
        /* latency stats */
        struct sched_info rq_sched_info;

        /* sys_sched_yield() stats */
        unsigned long yld_exp_empty;
        unsigned long yld_act_empty;
        unsigned long yld_both_empty;
        unsigned long yld_cnt;

        /* schedule() stats */
        unsigned long sched_switch;
        unsigned long sched_cnt;
        unsigned long sched_goidle;

        /* try_to_wake_up() stats */
        unsigned long ttwu_cnt;
        unsigned long ttwu_local;
#endif
};
```
* 复杂的宏定义和变量声明也需要注释
```C
/* TICK_USEC_TO_NSEC is the time between ticks in nsec assuming real ACTHZ and  */
/* a value TUSEC for TICK_USEC (can be set bij adjtimex)                */
#define TICK_USEC_TO_NSEC(TUSEC) (SH_DIV (TUSEC * USER_HZ * 1000, ACTHZ, 8))
/* some arch's have a small-data section that can be accessed register-relative
 * but that can only take up to, say, 4-byte variables. jiffies being part of
 * an 8-byte variable may not be correctly accessed unless we force the issue
 */
#define __jiffy_data  __attribute__((section(".data")))
/*
 * The 64-bit value is not volatile - you MUST NOT read it
 * without sampling the sequence number in xtime_lock.
 * get_jiffies_64() will do this for you as appropriate.
 */
extern u64 __jiffy_data jiffies_64;
extern unsigned long volatile __jiffy_data jiffies;
```

##标识符命名

* UNIX风格（K&R，或者说linux kernal风格），也即 a_b （大小写混合的命名方式是Modern C++风格所提倡的，在C++代码中很普遍，称为CamelCase），but not C）
* 标识符命名要清晰明了，可以使用**完整的单词和易于理解的缩写**。短的单词可以通过去元音形成缩写，较长的单词可以取单词的头几个字母形成缩写。看别人的代码看多了就可以总结出一些缩写惯例，例如count写成cnt，block写成blk，length写成len，window写成win，message写成msg，number写成nr，temporary可以写成temp，也可以进一步写成tmp
* 全局变量和全局函数的命名一定要详细，不惜多用几个单词多写几个下划线，例如函数名radix_tree_insert，因为它们在整个项目的许多源文件中都会用到，必须让使用者明确这个变量或函数是干什么用的。局部变量和只在一个源文件中调用的内部函数的命名可以简略一些，但不能太短。尽量不要使用单个字母做变量名，只有一个例外：用i、j、k做循环变量是可以的


##函数
每个函数都应该设计得尽可能简单，简单的函数才容易维护。应遵循以下原则：
* 实现一个函数**只是为了做好一件事情**，不要把函数设计成用途广泛、面面俱到的，这样的函数肯定会超长，而且往往不可重用，维护困难。
* 函数内部的**缩进层次不宜过多，一般以少于4层为宜**。如果缩进层次太多就说明设计得太复杂了，应考虑分割成更小的函数（Helper Function）来调用。
* 函数不要写得太长，建议**在24行的标准终端上不超过两屏**，太长会造成阅读困难，如果一个函数超过两屏就应该考虑分割函数了。`[CodingStyle]中特别说明，如果一个函数在概念上是简单的，只是长度很长，这倒没关系。例如函数由一个大的switch组成，其中有非常多的case，这是可以的，因为各case分支互不影响，整个函数的复杂度只等于其中一个case的复杂度，这种情况很常见，例如TCP协议的状态机实现。`
* 执行函数就是执行一个动作，函数名通常应包含**动词**，例如get_current、radix_tree_insert。
* 比较重要的函数定义上侧必须加注释，说明此函数的功能、参数、返回值、错误码等。
* 另一种度量函数复杂度的办法是看有多少个局部变量，**5到10个局部变量已经很多了**，再多就很难维护了，应该考虑分割成多个函数。


##ident工具

##Old Style C
> *main函数定义写成main(){...}的形式，不写返回值类型也不写参数列表*
>> **Old Style C规定不写返回值类型就表示返回int型，不写参数列表就表示参数类型和个数没有明确指出。**
>> 新标准仍然兼容，但不应该继续沿用。因为这样使得编译器无法检查程序中可能存在的bug，增加了调试难度。
>

#编译

**预处理**
CPP: C preprocessor
`cpp main.c` 可以看到预处理后的代码，`gcc -E main.c`也有同样的效果。
预处理做的事情有：
* 把头文件代码展开（到当前文件）
* 替换#define定义的标示符（预处理指示，preprocessing directive）
