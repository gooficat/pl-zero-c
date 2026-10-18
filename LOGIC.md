
```c
extern int num_len;
int num = 0;
int pow = 1;

while (num_len >= 0)
{
    dec num_len;
    int digit = num_buf[num_len];
    digit -= '0';
    digit *= pow;
    num += digit;
    pow *= 10;
}

```
