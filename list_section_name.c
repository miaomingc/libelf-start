#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/vis.h>

int main(int argc, char **argv)
{
    int fd;
    Elf *e;
    Elf_Scn *scn;
    Elf_Data *data;
    GElf_Shdr shdr;
    size_t n, shstrndx, sz;
    char *name, *p, pc[(4 * sizeof(char)) + 1];

    // 检查命令行参数
    if (argc != 2)
        errx(EXIT_FAILURE, "usage : %s file-name", argv[0]);

    // 初始化ELF库
    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization failed : %s", elf_errmsg(-1));

    // 打开文件
    if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open %s failed", argv[1]);

    // 开始读取ELF文件
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "elf_begin() failed %s", elf_errmsg(-1));

    // 检查文件类型
    if (elf_kind(e) != ELF_K_ELF)
        errx(EXIT_FAILURE, "%s is not an ELF object.", argv[1]);

    // 获取节头字符串表索引
    if (elf_getshdrstrndx(e, &shstrndx) != 0)
        errx(EXIT_FAILURE, "elf_getshdrstrndx() failed %s", elf_errmsg(-1));

    // 遍历所有节
    scn = NULL;
    while ((scn = elf_nextscn(e, scn)) != NULL)
    {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            errx(EXIT_FAILURE, "getshdr() failed %s.", elf_errmsg(-1));

        if ((name = elf_strptr(e, shstrndx, shdr.sh_name)) == NULL)
            errx(EXIT_FAILURE, "elf_strptr() failed: %s", elf_errmsg(-1));

        // 打印节索引和名称
        printf("section %-4.4jd %s \n", (uintmax_t)elf_ndxscn(scn), name);
    }

    // 获取节头字符串表节
    if ((scn = elf_getscn(e, shstrndx)) == NULL)
        errx(EXIT_FAILURE, "getscn() failed %s.", elf_errmsg(-1));

    if (gelf_getshdr(scn, &shdr) != &shdr)
        errx(EXIT_FAILURE, "getshdr(shstrndx) failed %s", elf_errmsg(-1));

    // 打印节头字符串表的大小
    printf(".shstrab: size=%jd\n", (uintmax_t)shdr.sh_size);

    // 遍历节头字符串表的数据
    data = NULL;
    n = 0;
    while (n < shdr.sh_size && (data = elf_getdata(scn, data)) != NULL)
    {
        p = (char *)data->d_buf;
        while (p < (char *)data->d_buf + data->d_size)
        {
            if (vis(pc, *p, VIS_WHITE, 0))
                printf("%s", pc);
            n++;
            p++;
            putchar((n % 16) ? ' ' : '\n');
        }
    }
    putchar('\n');

    // 结束ELF处理
    elf_end(e);
    close(fd);
    return EXIT_SUCCESS;
}