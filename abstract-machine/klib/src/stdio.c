#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。

    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum

    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位

    }while(unum);//直至unum为0退出循环

    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。

    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整

    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }

    return str;//返回转换后的字符串

}
int printf(const char *fmt, ...) {
    int cnt=0;
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        const char *tmp_char;
        int tmp_int;
        if (*fmt == '%') {
            fmt++;
            char tmp[55];
            switch (*fmt) {
                case 's':
                    tmp_char = va_arg(ap, const char *);
                    putstr(tmp_char);
                    break;
                case 'd':
                    tmp_int = va_arg(ap, int);
                    itoa(tmp_int, tmp, 10);
                    putstr(tmp);
                    break;
                case 'x':
                    tmp_int = va_arg(ap, int);
                    itoa(tmp_int, tmp, 16);
                    putstr(tmp);
                    break;
                case 'p':
                    tmp_int = va_arg(ap, int);
                    itoa(tmp_int, tmp, 16);
                    putstr(tmp);
                    break;
                case 'c':
                    tmp_int = va_arg(ap, int);
                    putch(tmp_int);
                    break;
                default:
                    return -1;
            }
            cnt++;
        } else {
           putch(*fmt);
        }
        fmt++;
        cnt++;
    }
    return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    panic_on(1, "no support vsprintf--------\n");
    return 0;
}



int sprintf(char *out, const char *fmt, ...) {
//  return 0;
    int cnt=0;
    va_list ap;
    *out = '\0';
    va_start(ap, fmt);
    while (*fmt) {
        const char *tmp_char;
        int tmp_int;
        if (*fmt == '%') {
            fmt++;
            char tmp[55];
            switch (*fmt) {
                case 's':
                    tmp_char = va_arg(ap, const char *);
                    strcat(out, tmp_char);
                    break;
                case 'd':
                    tmp_int = va_arg(ap, int);
                    strcat(out, itoa(tmp_int, tmp, 10));
                    break;
                case 'x':
                    tmp_int = va_arg(ap, int);
                    strcat(out, itoa(tmp_int, tmp, 16));
                    break;
                case 'p':
                    tmp_int = va_arg(ap, int);
                    strcat(out, itoa(tmp_int, tmp, 16));
                    break;
                case 'c':
                    tmp_int = va_arg(ap, int);
                    tmp[0]=tmp_int;
                    tmp[1]='\0';
                    strcat(out, tmp);
                    break;
                default:
                    return -1;
            }
            for(;*out!='\0';out++,cnt++);
        } else {
            *out = *fmt;
            out++;
            *out = '\0';
            cnt+=1;
        }
        fmt++;
    }
    return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    panic_on(1, "no support snprintf--------\n");
    return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    panic_on(1, "no support vsnprintf--------\n");
    return 0;
}

#endif
