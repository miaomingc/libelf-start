#include <err.h>
#include <fcntl.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    uint32_t fd;
    Elf *e;
    char *k;
    Elf_Kind ek;

    // 检查命令行参数
    if (argc != 2)
    {
        errx(EXIT_FAILURE, "usage:%s file-name", argv[0]);
    }

    // 初始化ELF库
    if (elf_version(EV_CURRENT) == EV_NONE)
    {
        errx(EXIT_FAILURE, "ELF library initialization failed %s", elf_errmsg(-1));
    }

    // 打开文件
    if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
    {
        err(EXIT_FAILURE, "open %s failed", argv[1]);
    }

    // 开始读取ELF文件
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
    {
        errx(EXIT_FAILURE, "elf_begin() failed %s %d", elf_errmsg(-1), fd);
    }

    // 获取文件类型
    ek = elf_kind(e);
    switch (ek)
    {
    case ELF_K_AR:
        k = "ar archive";
        break;
    case ELF_K_ELF:
        k = "elf object";
        break;
    case ELF_K_NONE:
        k = "data";
        break;
    default:
        k = "unrecognized";
    }

    // 打印文件类型
    printf("%s :%s \n", argv[1], k);

    // 结束ELF处理
    elf_end(e);
    close(fd);
    return 0;
}