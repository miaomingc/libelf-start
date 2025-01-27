#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define PRINT_FMT "   %-20s 0x%jx"
#define PRINT_FIELD(N)                            \
    do                                            \
    {                                             \
        printf(PRINT_FMT, #N, (uintmax_t)phdr.N); \
    } while (0)
#define NL()          \
    do                \
    {                 \
        printf("\n"); \
    } while (0)

// 打印程序头类型
void print_ptype(size_t pt)
{
    char *s;
#define C(V)     \
    case PT_##V: \
        s = #V;  \
        break
    switch (pt)
    {
        C(NULL);
        C(LOAD);
        C(DYNAMIC);
        C(INTERP);
        C(NOTE);
        C(SHLIB);
        C(PHDR);
        C(TLS);
        C(SUNWBSS);
        C(SUNWSTACK);
        C(GNU_EH_FRAME);
        C(GNU_PROPERTY);
        C(GNU_RELRO);
        C(GNU_SFRAME);
        C(GNU_STACK);
        //        C(SUNWDTRACE);
        //        C(SUNWCAP);   PT_SUNWBSS and PT_SUNWSTACK not found in linux platform
    default:
        s = "unknown";
        break;
    }
#undef C
    printf("  %s  ", s);
}

int main(int argc, char **argv)
{
    int i, fd;
    Elf *e;
    char *id, byte[5];
    size_t n;
    GElf_Phdr phdr;

    // 检查命令行参数
    if (argc != 2)
        errx(EXIT_FAILURE, "usage : %s file-name", argv[0]);

    // 初始化ELF库
    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization failed: %s", elf_errmsg(-1));

    // 打开文件
    if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open %s failed", argv[1]);

    // 开始读取ELF文件
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "\"%s\" is not an ELF object", argv[1]);

    // 获取程序头数量
    if (elf_getphdrnum(e, &n) != 0)
        errx(EXIT_FAILURE, "elf_getphdrnum() failed %s", elf_errmsg(-1));

    // 遍历所有程序头
    for (i = 0; i < n; i++)
    {
        if (gelf_getphdr(e, i, &phdr) != &phdr)
            errx(EXIT_FAILURE, "getphdr() failed : %s.", elf_errmsg(-1));

        printf("PHDR %d :\n", i);
        PRINT_FIELD(p_type);
        print_ptype(phdr.p_type);
        NL();
        PRINT_FIELD(p_offset);
        NL();
        PRINT_FIELD(p_vaddr);
        NL();
        PRINT_FIELD(p_paddr);
        NL();
        PRINT_FIELD(p_filesz);
        NL();
        PRINT_FIELD(p_memsz);
        NL();
        PRINT_FIELD(p_flags);
        NL();

        // 打印程序头标志
        printf(" [ ");
        if (phdr.p_flags & PF_X)
            printf(" execute ");
        if (phdr.p_flags & PF_R)
            printf(" read ");
        if (phdr.p_flags & PF_W)
            printf(" write ");
        printf(" ]");
        NL();
        PRINT_FIELD(p_align);
        NL();
    }

    // 结束ELF处理
    elf_end(e);
    close(fd);
    return EXIT_SUCCESS;
}