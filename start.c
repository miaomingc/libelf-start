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
    if (argc != 2)
    {
        errx(EXIT_FAILURE, "usage:%s file-name", argv[0]);
    }
    if (elf_version(EV_CURRENT) == EV_NONE)
    {
        errx(EXIT_FAILURE, "ELF library initialization failed %s", elf_errmsg(-1));
    }
    if ((fd = open(argv[1], O_RDONLY, 0) )<0)
    {
        err(EXIT_FAILURE, "open %s failed", argv[1]);
    }
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
    {
        errx(EXIT_FAILURE, "elf_begin() failed %s %d", elf_errmsg(-1),fd);
    }
    ek = elf_kind(e);
    switch (ek)
    {
    case ELF_K_AR:
        k = "ar(1) archive";
        break;
        case ELF_K_ELF:
        k="elf object";
        break;
        case ELF_K_NONE:
        k="data";
        break;
        default:
        k="unrecognized";
    }
    printf("%s :%s \n",argv[1],k);
    elf_end(e);
    close(fd);
    return 0;
}