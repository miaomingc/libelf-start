#include <err.h>
#include <fcntl.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint32_t hash_words[] ={0x01234567,0x89abcdef,0xdeadc0de};
char string_table[]={'\0','.','f','o','o','\0','.','s','h','s','t','r','t','a','b','\0'};

int main(int argc,char **argv)
{
    int fd;
    Elf *e;
    Elf_Scn *scn;
    Elf_Data *data;
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    Elf32_Shdr *shdr;

    // 检查命令行参数
    if(argc!=2)
        errx(EXIT_FAILURE,"usage : %s file-name",argv[0]);

    // 初始化ELF库
    if(elf_version(EV_CURRENT)==EV_NONE)
        errx(EXIT_FAILURE,"ELF library initialization failed %s",elf_errmsg(-1));

    // 打开文件
    if((fd=open(argv[1],O_WRONLY|O_CREAT,0777))<0)
        err(EXIT_FAILURE,"open %s failed",argv[1]);

    // 开始写入ELF文件
    if((e=elf_begin(fd,ELF_C_WRITE,NULL))==NULL)
        errx(EXIT_FAILURE,"elf_begin() failed: %s",elf_errmsg(-1));

    // 创建新的ELF头
    if((ehdr=elf32_newehdr(e))==NULL)
        errx(EXIT_FAILURE,"elf32_newehdr() failed %s",elf_errmsg(-1));
    ehdr->e_ident[EI_DATA]=ELFDATA2MSB;
    ehdr->e_machine=EM_PPC;
    ehdr->e_type=ET_EXEC;

    // 创建新的程序头
    if((phdr=elf32_newphdr(e,1))==NULL)
        errx(EXIT_FAILURE,"elf32_newphdr() failed: %s.",elf_errmsg(-1));

    // 创建新的节
    if((scn=elf_newscn(e))==NULL)
        errx(EXIT_FAILURE,"elf_newscn() failed: %s",elf_errmsg(-1));

    // 创建新的数据
    if((data=elf_newdata(scn))==NULL)
        errx(EXIT_FAILURE,"elf_newdata() failed: %s.",elf_errmsg(-1));
    data->d_align=4;
    data->d_off=0LL;
    data->d_buf=hash_words;
    data->d_type=ELF_T_WORD;
    data->d_size=sizeof(hash_words);
    data->d_version=EV_CURRENT;

    // 获取节头
    if((shdr=elf32_getshdr(scn))==NULL)
        errx(EXIT_FAILURE,"elf32_getshdr() failed %s.",elf_errmsg(-1));
    shdr->sh_name=1;
    shdr->sh_type=SHT_HASH;
    shdr->sh_flags=SHF_ALLOC;
    shdr->sh_entsize=0;

    // 创建新的节
    if((scn=elf_newscn(e))==NULL)
        errx(EXIT_FAILURE,"elf_newscn() failed",elf_errmsg(-1));

    // 创建新的数据
    if((data=elf_newdata(scn))==NULL)
        errx(EXIT_FAILURE,"elf32_getshdr() failed: %s",elf_errmsg(-1));
    data->d_align=1;
    data->d_buf=string_table;
    data->d_off=0LL;
    data->d_size=sizeof(string_table);
    data->d_type=ELF_T_BYTE;
    data->d_version=EV_CURRENT;

    // 获取节头
    if((shdr=elf32_getshdr(scn))==NULL)
        errx(EXIT_FAILURE,"elf32_getshdr() failed: %s.",elf_errmsg(-1));
    shdr->sh_name=6;
    shdr->sh_type=SHT_STRTAB;
    shdr->sh_flags=SHF_STRINGS|SHF_ALLOC;
    shdr->sh_entsize=0;
    shdr->sh_addralign = 1;
    ehdr->e_shstrndx=elf_ndxscn(scn);

    // 更新ELF文件
    if(elf_update(e,ELF_C_NULL)<0)
        errx(EXIT_FAILURE,"elf_update() failed :%s",elf_errmsg(-1));

    // 设置程序头
    phdr->p_type=PT_PHDR;
    phdr->p_offset=ehdr->e_shoff;
    phdr->p_filesz=elf32_fsize(ELF_T_PHDR,1,EV_CURRENT);
    elf_flagphdr(e,ELF_C_SET,ELF_F_DIRTY);

    // 写入ELF文件
    if(elf_update(e,ELF_C_WRITE)<0)
        errx(EXIT_FAILURE,"elf_update() failed %s",elf_errmsg(-1));

    // 结束ELF处理
    elf_end(e);
    close(fd);
    return EXIT_SUCCESS;
}