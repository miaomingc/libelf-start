#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/vis.h>

#define PRINT_FMT "    %-20s 0x%jx\n"
#define PRINT_FIELD(N)                            \
    do                                            \
    {                                             \
        printf(PRINT_FMT, #N, (uintmax_t)ehdr.N); \
    } while (0)

int main(int argc, char **argv)
{
    int i, fd;
    Elf *e;
    char *id, bytes[5];
    GElf_Ehdr ehdr;
    size_t n;

    // 检查命令行参数
    if (argc != 2)
        errx(EXIT_FAILURE, "usage: %s file name", argv[0]);

    // 初始化ELF库
    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization failed %s", elf_errmsg(-1));

    // 打开文件
    if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open %s failed", argv[1]);

    // 开始读取ELF文件
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "elf_begin() failed:%s", elf_errmsg(-1));

    // 检查文件类型
    if (elf_kind(e) != ELF_K_ELF)
        errx(EXIT_FAILURE, "\"%s\" is not an ELF object", argv[1]);

    // 获取ELF头
    if (gelf_getehdr(e, &ehdr) == NULL)
        errx(EXIT_FAILURE, "getehdr() failed %s", elf_errmsg(-1));

    // 获取ELF类
    if ((i = gelf_getclass(e)) == ELFCLASSNONE)
        errx(EXIT_FAILURE, "getclass() failed: %s", elf_errmsg(-1));

    printf("%s: %d-bit ELF object\n", argv[1], i == ELFCLASS32 ? 32 : 64);

    // 获取ELF标识
    if ((id = elf_getident(e, NULL)) == NULL)
        errx(EXIT_FAILURE, "getident() failed %s", elf_errmsg(-1));

    printf("%3s e_ident[0..%1d] %7s", " ", EI_ABIVERSION, " ");
    for (i = 0; i <= EI_NIDENT; i++)
    {
        vis(bytes, id[i], VIS_WHITE, 0);
        printf(" ['%s' %X]", bytes, id[i]);
    }
    printf("\n");

    // 打印ELF头字段
    PRINT_FIELD(e_type);
    PRINT_FIELD(e_machine);
    PRINT_FIELD(e_version);
    PRINT_FIELD(e_entry);
    PRINT_FIELD(e_phoff);
    PRINT_FIELD(e_shoff);
    PRINT_FIELD(e_flags);
    PRINT_FIELD(e_ehsize);
    PRINT_FIELD(e_phentsize);
    PRINT_FIELD(e_shentsize);

    // 获取节头数量
    if (elf_getshdrnum(e, &n) != 0)
        errx(EXIT_FAILURE, "getshdrnum() failed : %s.", elf_errmsg(-1));
    printf(PRINT_FMT, "(shnum)", (uintmax_t)n);

    // 获取节头字符串表索引
    if (elf_getshdrstrndx(e, &n) != 0)
        errx(EXIT_FAILURE, "getshdrstrndx() failed: %s", elf_errmsg(-1));
    printf(PRINT_FMT, "(shstrndx)", (uintmax_t)n);

    // 获取程序头数量
    if (elf_getphdrnum(e, &n) != 0)
        errx(EXIT_FAILURE, "getphdrnum() failed: %s", elf_errmsg(-1));
    printf(PRINT_FMT, "(phnum)", (uintmax_t)n);

    // 结束ELF处理
    (void)elf_end(e);
    (void)close(fd);
    return EXIT_SUCCESS;
}