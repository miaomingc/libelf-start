#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void print_ptype(size_t pt)
{
    char *s;
#define C(V) case PT_##V: s = #V;break
    switch(pt)
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
//        C(SUNWDTRACE);
//        C(SUNWCAP);
        default :
            s="unknown";
            break;
    }
#undef C
}
int main(int argc,char **argv)
{
    int i,fd;
    Elf *e;
    char *id,byte[5];
    size_t n;
    GElf_Phdr phdr;
    if(argc!=2)
        errx(EXIT_FAILURE,"usage : %s file-name",argv[0]);
    if(elf_version(EV_CURRENT)==EV_NONE)
        errx(EXIT_FAILURE,"ELF library initialization failed: %s",elf_errmsg(-1));
    if((fd=open(argv[1],O_RDONLY,0))<0)
        err(EXIT_FAILURE,"open %s failed",argv[1]);
    if((e=elf_begin(fd,ELF_C_READ,NULL))==NULL)
        errx(EXIT_FAILURE,"\"%s\" is not an ELF object",argv[1]);
    if(elf_getphdrnum(e,&n)!=0)
        errx(EXIT_FAILURE,"elf_getphdrnum() failed %s",elf_errmsg(-1));
    for(i=0;i<n;i++)
    {
        if(gelf_getphdr(e,i,&phdr)!=&phdr)
            errx(EXIT_FAILURE,"getphdr() failed : %s.",elf_errmsg(-1));
        print("PHDR %d :\n",i);
    }
    return EXIT_SUCCESS;
}