#ifndef	__PRINT_H
#define __PRINT_H

//printf具体使用方法见附录1：本头文件末尾部分
//

#define printf   myprintf
#define sprintf  mysprintf

typedef char *  _va_list;

//以下函数用于对不确定参数数量的函数进行参数指针定位
//将n的长度化为int长度的整数倍
#define _INTSIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
//初始定位变参数函数指针位置
#define _crt_va_start(ap,v) ( ap = (_va_list)&(v) + _INTSIZEOF(v) )
//依次读取变参数的数值
#define _crt_va_arg(ap,t)   ( *(t *)((ap+=_INTSIZEOF(t)),ap-_INTSIZEOF(t)) )
//放空ap指针为下一次调用做准备
#define _crt_va_end(ap)     ( ap = (_va_list)0 )

//用于传递字符串参数的信息结构体
typedef struct {
	int dest;                         //确定信息结构体的类型：默认类型或字符串类型
	void (*func)(char);       //链接输出功能函数
	char *loc;                        //若在字符串模式用于存储字符尾地址
} PRINTK_INFO;


//======================================================================
//函数名称：myprintf
//功能概要：根据输入的参数进行解析然后将数据通过串口打出
//参数说明：const char *:打印参数字符串：对输入的字符串进行解析并按格式输出
//       例如： printf("c=%c\n",c); 将c按char型进行输出
//函数返回：字符串长度
//======================================================================
int myprintf(const char *, ...);
int mysprintf (char *s, const char *fmt, ...);

#define DEST_CONSOLE    (1)
#define DEST_STRING     (2)
//用于分辨%f与%lf的宏定义字符
#define LENMOD_l  1
#define LENMOD_L  2
//用于判断%后输出格式的宏定义
#define FLAGS_MINUS     (0x01)
#define FLAGS_PLUS      (0x02)
#define FLAGS_SPACE     (0x04)
#define FLAGS_ZERO      (0x08)
#define FLAGS_POUND     (0x10)

#define IS_FLAG_MINUS(a)    (a & FLAGS_MINUS)
#define IS_FLAG_PLUS(a)     (a & FLAGS_PLUS)
#define IS_FLAG_SPACE(a)    (a & FLAGS_SPACE)
#define IS_FLAG_ZERO(a)     (a & FLAGS_ZERO)
#define IS_FLAG_POUND(a)    (a & FLAGS_POUND)


#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#endif


//======================================================================
/*  附录一
 *
 *  C.1 prinf调用的一般格式
 *  printf函数是一个标准库函数，我们这里将其进行了重新整理。
 *  printf函数调用的一般形式为：printf(“格式控制字符串”, 输出表列)。
 *  其中格式控制字符串用于指定输出格式。格式控制串可由格式字符串和非格式字符串两种组成。格式字符串是以%开头
 *  的字符串，在%后面跟有各种格式字符，以说明输出数据的类型、形式、长度、小数位数等。如：
 *     ” %d”表示按十进制整型输出；
 *     ” %ld”表示按十进制长整型输出；
 *     ” %f”表示浮点型输出。
 *     ” %lf”表示double型输出。
 *     ” %c”表示按字符型输出。
 *     ” \n”表示换行符等。
 *         非格式字符串原样输出，在显示中起提示作用。输出表列中给出了各个输出项，要求格式字符串和各输出项在数量
 *  和类型上应该一一对应。
 *  C.1.1 格式字符串
 *         格式字符串的一般形式为：[标志][输出最小宽度][精度][长度]类型，其中方括号[]中的项为可选项。
 *
 *  以下说明各项的意义。
 *  1. 类型
 *  类型字符用以表示输出数据的类型，其格式符和意义如表C-1所示。
 *  表C-1 类型字符
 *  格式字符	                             意义
 *    d	             以十进制形式输出带符号整数(正数不输出符号)
 *    o	             以八进制形式输出无符号整数(不输出前缀0)
 *   x,X	   以十六进制形式输出无符号整数(不输出前缀Ox)
 *    u	             以十进制形式输出无符号整数
 *    f	             以小数形式输出单、双精度实数
 *   e,E	   以指数形式输出单、双精度实数
 *   g,G	   以%f或%e中较短的输出宽度输出单、双精度实数
 *    c	             输出单个字符
 *    s	             输出字符串
 *  2. 标志
 *  标志字符为 -、+、# 和空格四种，其意义如表C-2所示。
 *  标志	                           意义
 *   -	            结果左对齐，右边填空格
 *   +	            输出符号(正号或负号)
 *  3. 输出最小宽度
 *          用十进制整数来表示输出的最少位数。若实际位数多于定义的宽度，则按实际位数输出，若实际位数少于定义
 *  的宽度则补以空格或0。
 *  4. 精度
 *          精度格式符以“.”开头，后跟十进制整数。本项的意义是：如果输出数字，则表示小数的位数；如果输出的
 *  是字符，则表示输出字符的个数；若实际位数大于所定义的精度数，则截去超过的部分。
 *  5. 长度
 *  长度格式符为h、l两种，h表示按短整型量输出，l表示按长整型量输出。
 *  
 *  浮点数精度说明：
 *    经过测试发现浮点数的精度为小数点后5位。
 *    浮点数的最后两位存在随机误差这与KEA128芯片的内核有关,经过测试发现与其同内核的KL25存在同样的问题。
 *
 */

/*
 *    实验测试用例：
 * 	  char c,s[20];
 *    int a;
 *    float f;
 *    double x;
 *    a=1234;
 *    f=3.14159322;
 *    x=0.123456789123456789;
 *    c='A';
 *    strcpy(s,"Hello,World");
 *    uart_init(UART_Debug,9600);      //串口2使用总线时钟20MHz
 *    printf("苏州大学嵌入式实验室KEA128-printf构件测试用例!\n");
 *    //整数数类型数据输出测试
 *    printf("整型数据输出测试:\n");
 *    printf("整数a=%d\n", a);        //按照十进制整数格式输出，显示 a=1234
 *    printf("整数a=%d%%\n", a);      //输出%号 结果 a=1234%
 *    printf("整数a=%6d\n", a);       //输出6位十进制整数 左边补空格，显示 a= 1234
 *    printf("整数a=%06d\n", a);      //输出6位十进制整数 左边补0，显示 a=001234
 *    printf("整数a=%2d\n", a);       //a超过2位，按实际输出 a=1234
 *    printf("整数a=%-6d\n", a);      //输出6位十进制整数 右边补空格，显示 a=1234
 *    printf("\n");
 *    //浮点数类型数据输出测试
 *    printf("浮点型数据输出测试:\n");
 *    printf("浮点数f=%f\n", f);                  //浮点数有效数字是7位，结果f=3.14159297
 *    printf("浮点数fhavassda  =  %6.4f\n", f);   //输出6列，小数点后4位，结果 f=3.1416
 *    printf("double型数x=%lf\n", x);            //输出长浮点数 x=0.12345678912345678
 *    printf("double型数x=%18.15lf\n", x);       //输出18列，小数点后15位，x=0.123456789123456
 *    printf("\n");
 *    //字符类型数据输出测试
 *    printf("字符类型数据输出测试:\n");
 *    printf("字符型c=%c\n", c);           //输出字符 c=A
 *    printf("ASCII码c=%x\n", c);         //以十六进制输出字符的ASCII码 c=41
 *    printf("字符串s[]=%s\n", s);         //输出数组字符串s[]=Hello,World
 *    printf("字符串s[]=%6.9s\n", s);      //输出最多9个字符的字符串 s[]=Hello,World
 */

