// a cholesterol-free printf()
// supports %[csdx] with modifiers [0-9lu]
// build with CFLAGS=-std=c99 -Wpedantic -Wall -Wextra -Werror
// code size with CFLAGS+=-mthumb -Os: 338 bytes
// code size with CFLAGS+=-mthumb -O0: 704 bytes

// #include <stdarg.h>
typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

// #include <stdio.h>
typedef void FILE;
int fputc(int,FILE *);

// numeric format flags
#define IS_LONG (1<<0)
#define IS_UNSIGNED (1<<1)
#define IS_HEX (1<<2)

static void _fputs(const char *restrict str, FILE *fd) {
  int ch;
  while ((ch = *(str++))) { fputc(ch, fd); }
}

static void _fputlu(unsigned long val, FILE *fd, int base, int width) {
  unsigned long denom;
  int ndigits = (val==0);
  for (denom = 1; denom <= val; denom *= base) { ndigits++; }
  while ((ndigits++)<width) { fputc(' ',fd); }
  if (!val) { fputc('0',fd); } // HACK
  while ((denom/=base)) {
    int digit = val/denom;
    if (digit > 9) { fputc('a' + digit - 10,fd); }
    else { fputc('0' + digit, fd); }
    val -= digit*denom;
  }
}

static void _fputl(long val, FILE *fd, int base, int width) {
  if (val<0) { fputc('-',fd); val=-val; width-=width?1:0; }
  _fputlu((unsigned long)val, fd, base, width);
}

#define _fputiu(val,fd,base,width) _fputlu((unsigned long)val,fd,base,width)
#define _fputi(val,fd,base,width) _fputl((long)val,fd,base,width)

__attribute__((format(printf,2,3)))
int _fprintf(FILE *restrict fd, const char *restrict fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int ch;
  while ((ch = *(fmt++))) {
    // normal output
    if (ch != '%') {
      fputc(ch,fd);
      continue;
    }
    ch = *(fmt++);

    // escaped '%'

    if (ch == '%') {
      fputc(ch, fd);
      continue;
    }

    // character or string format specifiers

    if (ch == 'c') {
      int fmt_c = va_arg(ap, int);
      fputc(fmt_c,fd);
      continue;
    }
    if (ch == 's') {
      char *fmt_s = va_arg(ap, char *);
      _fputs(fmt_s,fd);
      continue;
    }

    // numeric format specifiers

    int width;
    int flags = 0;

    for (width=0; ch>='0' && ch<='9'; ch=*(fmt++)) {
      width = 10*width + ch-'0';
    }

    if (ch == 'l') {
      flags |= IS_LONG;
      ch = *(fmt++);
    }

    if (ch == 'u') {
      flags |= IS_UNSIGNED;
      ch = *(fmt++);
    }

    if (ch == 'x') {
      flags |= IS_HEX;
    } else if (ch != 'd') {
      --fmt;
      if (!flags) {
        // the base is only optional if some
        // other format specifier is given
        fputc('%',fd);
        continue;
      }
    }

    // finally: print the number

    unsigned int fmt_u;
    long fmt_l;
    unsigned long fmt_lu;
    int fmt_i;

    int base = (flags & IS_HEX) ? 16 : 10;
    switch(flags & (IS_UNSIGNED | IS_LONG)) {
      case IS_UNSIGNED:
        fmt_u = va_arg(ap, unsigned int);
        _fputiu(fmt_u, fd, base, width);
        break;
      case IS_LONG:
        fmt_l = va_arg(ap, long);
        _fputl(fmt_l, fd, base, width);
        break;
      case (IS_UNSIGNED | IS_LONG):
        fmt_lu = va_arg(ap, unsigned long);
        _fputlu(fmt_lu, fd, base, width);
        break;
      default:
        fmt_i = va_arg(ap, int);
        _fputi(fmt_i, fd, base, width);
        break;
    }
  }
  va_end(ap);
  return 0;
}
