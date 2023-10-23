/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

void userland()
{
    debug("Called Userland\n");
    asm volatile("mov %eax, %cr0");
}

void print_gdt_content(gdt_reg_t gdtr_ptr)
{
    seg_desc_t *gdt_ptr;
    gdt_ptr = (seg_desc_t *)(gdtr_ptr.addr);
    int i = 0;
    while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit))
    {
        uint32_t start = gdt_ptr->base_3 << 24 | gdt_ptr->base_2 << 16 | gdt_ptr->base_1;
        uint32_t end;
        if (gdt_ptr->g)
        {
            end = start + ((gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1) << 12) + 4095;
        }
        else
        {
            end = start + (gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1);
        }
        debug("%d ", i);
        debug("[0x%x ", start);
        debug("- 0x%x] ", end);
        debug("seg_t: 0x%x ", gdt_ptr->type);
        debug("desc_t: %d ", gdt_ptr->s);
        debug("priv: %d ", gdt_ptr->dpl);
        debug("present: %d ", gdt_ptr->p);
        debug("avl: %d ", gdt_ptr->avl);
        debug("longmode: %d ", gdt_ptr->l);
        debug("default: %d ", gdt_ptr->d);
        debug("gran: %d ", gdt_ptr->g);
        debug("\n");
        gdt_ptr++;
        i++;
    }
}

void tp()
{
    // TODO
    /*
    Q1
    Rappeler comment fonctionne l'instruction `SGDT` (rôle, paramètres,
    etc.), puis trouver dans [`kernel/include/segmem.h`
    la macro qui l'utilise pour afficher l'adresse
    de base de la GDT en cours d'utilisation ainsi que sa "limite" (type utile :
    */
    gdt_reg_t gdt;
    get_gdtr(gdt);
    /* printf("GDT address: %lx\n GDT limit: %x\n", gdt.addr, gdt.limit);
    print_gdt_content(gdt); */
    tss_t tss;
    get_tr(tss);
    /* printf("CS: %x\n", tss.cs);
    printf("SS: %x\n", tss.ss);
    printf("DS: %x\n", tss.ds);
    printf("ES: %x\n", tss.es);
    printf("FS: %x\n", tss.fs);
    printf("GS: %x\n", tss.gs); */
    // Q5
    seg_desc_t my_gdt[7];
    my_gdt[0].raw.bhigh = 0;
    my_gdt[0].raw.blow = 0;
    my_gdt[1].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
    my_gdt[1].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
    my_gdt[1].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
    my_gdt[1].type = 11;        // Code,RX //:4;      /* segment type */
    my_gdt[1].s = 1;            //: 1;      /* descriptor type */
    my_gdt[1].dpl = 0;          // ring0    //:2;      /* descriptor privilege level */
    my_gdt[1].p = 1;            //: 1;      /* segment present flag */
    my_gdt[1].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
    my_gdt[1].avl = 1;          //: 1;      /* available for fun and profit */
    my_gdt[1].l = 0;            // 32bits     //:1;      /* longmode */
    my_gdt[1].d = 1;            //: 1;      /* default length, depend on seg type */
    my_gdt[1].g = 1;            //: 1;      /* granularity */
    my_gdt[1].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */
    my_gdt[2].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
    my_gdt[2].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
    my_gdt[2].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
    my_gdt[2].type = 3;         // data,RW //:4;      /* segment type */
    my_gdt[2].s = 1;            //: 1;      /* descriptor type */
    my_gdt[2].dpl = 0;          // ring0    //:2;      /* descriptor privilege level */
    my_gdt[2].p = 1;            //: 1;      /* segment present flag */
    my_gdt[2].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
    my_gdt[2].avl = 1;          //: 1;      /* available for fun and profit */
    my_gdt[2].l = 0;            // 32 bits   //:1;      /* longmode */
    my_gdt[2].d = 1;            //: 1;      /* default length, depend on seg type */
    my_gdt[2].g = 1;            //: 1;      /* granularity */
    my_gdt[2].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */
    // end Q5

    gdt_reg_t my_gdtr;
    my_gdtr.addr = (long unsigned int)my_gdt;
    my_gdtr.limit = sizeof(my_gdt) - 1;
    set_gdtr(my_gdtr);
    // Pour les selecteurs, on mets 1 et 2, comme ça pour sélectionner dans la gdt on fait my_gdt[tss.cs/ds]
    // Demander si c'est ça ^^^, cs = 16, pourquoi ???
    tss.cs = 1;
    tss.ds = 2;
    debug("%d\n", get_ds());
    get_gdtr(gdt);
    /* printf("GDT address: %lx\n GDT limit: %x\n", gdt.addr, gdt.limit);
    print_gdt_content(gdt); */

    char src[64];
    char *dst = 0;

    memset(src, 0xff, 64);
    _memcpy8(dst, src, 32);

    my_gdt[3].limit_1 = 0x20;  //: 16;     /* bits 00-15 of the segment limit */
    my_gdt[3].base_1 = 0x0000; //: 16;     /* bits 00-15 of the base address */
    my_gdt[3].base_2 = 0x60;   //: 8;      /* bits 16-23 of the base address */
    my_gdt[3].type = 3;        // data,RW //:4;      /* segment type */
    my_gdt[3].s = 1;           //: 1;      /* descriptor type */
    my_gdt[3].dpl = 0;         // ring0    //:2;      /* descriptor privilege level */
    my_gdt[3].p = 1;           //: 1;      /* segment present flag */
    my_gdt[3].limit_2 = 0xf;   //: 4;      /* bits 16-19 of the segment limit */
    my_gdt[3].avl = 1;         //: 1;      /* available for fun and profit */
    my_gdt[3].l = 0;           // 32 bits   //:1;      /* longmode */
    my_gdt[3].d = 1;           //: 1;      /* default length, depend on seg type */
    my_gdt[3].g = 1;           //: 1;      /* granularity */
    my_gdt[3].base_3 = 0x00;   //: 8;      /* bits 24-31 of the base address */

    seg_sel_t my_es;
    my_es.index = 3;
    my_es.ti = 0;
    my_es.rpl = 0;
    set_es(my_es);
    _memcpy8(dst, src, 32);

    //_memcpy8(dst, src, 64); // jsp pourquoi ça marche
    // Q12
    my_gdt[4].limit_1 = 0xffff;  //: 16;     /* bits 00-15 of the segment limit */
    my_gdt[4].base_1 = 0x0000; //: 16;     /* bits 00-15 of the base address */
    my_gdt[4].base_2 = 0x00;   //: 8;      /* bits 16-23 of the base address */
    my_gdt[4].type = 11;        // data,RX //:4;      /* segment type */
    my_gdt[4].s = 1;           //: 1;      /* descriptor type */
    my_gdt[4].dpl = 3;         // ring3    //:2;      /* descriptor privilege level */
    my_gdt[4].p = 1;           //: 1;      /* segment present flag */
    my_gdt[4].limit_2 = 0xf;   //: 4;      /* bits 16-19 of the segment limit */
    my_gdt[4].avl = 1;         //: 1;      /* available for fun and profit */
    my_gdt[4].l = 0;           // 32 bits   //:1;      /* longmode */
    my_gdt[4].d = 1;           //: 1;      /* default length, depend on seg type */
    my_gdt[4].g = 1;           //: 1;      /* granularity */
    my_gdt[4].base_3 = 0x00;   //: 8;      /* bits 24-31 of the base address */

    my_gdt[5].limit_1 = 0xffff;  //: 16;     /* bits 00-15 of the segment limit */
    my_gdt[5].base_1 = 0x0000; //: 16;     /* bits 00-15 of the base address */
    my_gdt[5].base_2 = 0x00;   //: 8;      /* bits 16-23 of the base address */
    my_gdt[5].type = 4;        // data,RW //:4;      /* segment type */
    my_gdt[5].s = 1;           //: 1;      /* descriptor type */
    my_gdt[5].dpl = 3;         // ring3    //:2;      /* descriptor privilege level */
    my_gdt[5].p = 1;           //: 1;      /* segment present flag */
    my_gdt[5].limit_2 = 0xf;   //: 4;      /* bits 16-19 of the segment limit */
    my_gdt[5].avl = 1;         //: 1;      /* available for fun and profit */
    my_gdt[5].l = 0;           // 32 bits   //:1;      /* longmode */
    my_gdt[5].d = 1;           //: 1;      /* default length, depend on seg type */
    my_gdt[5].g = 1;           //: 1;      /* granularity */
    my_gdt[5].base_3 = 0x00;   //: 8;      /* bits 24-31 of the base address */

    set_ds(gdt_usr_seg_sel(5));
    set_es(gdt_usr_seg_sel(5));
    set_fs(gdt_usr_seg_sel(5));
    set_gs(gdt_usr_seg_sel(5));
    //set_ss(gdt_usr_seg_sel(5)); // CA PLANTE LETS GO
    debug("Jumping to userland...\n\n");
    fptr32_t fptr = {.segment = gdt_usr_seg_sel(4), .offset = (uint32_t)userland}; 
    farjump(fptr);  // plante, #GP
}
