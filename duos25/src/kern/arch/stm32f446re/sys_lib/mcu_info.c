#include<mcu_info.h>
#include<sys_bus_matrix.h>
#include<kstring.h>
#include<kstdio.h>
//static mcu_type_def mcuinfo;
static uint8_t product_id[20]={0};
void show_system_info(void)
{
    uint16_t pkg = ((MCUINFO->PKG_TYPE & ~(0xFC7FUL)) >>0x8UL);
    store_product_id();
    kprintf("Package Type: ");
    switch ((pkg))
    {
    case 0x00UL /* constant-expression */:
        /* code */
        kprintf("LQFP64");
        break;
    case 0x01UL:
        kprintf("LQFP100");
    case 0x02UL:
        kprintf("WLCSP81");
    case 0x03UL:
        kprintf("LQFP144");
    default: kprintf("UNKNOWN");
        break;
    }
    kprintf(", Flash Memory %d KB",MCUINFO->F_SIZE);
    kprintf("\n");
    kprintf("Product ID: %s\n",product_id);
}

void store_product_id(void)
{
    
    uint32_t j=0;
    uint8_t *ptr;
    ptr=(uint8_t*)&MCUINFO->UIDLOTH;
    for(uint32_t i=0;i<4;i++)
    {
        product_id[j]=ptr[i];
        j++;
    }
    ptr=(uint8_t*)&MCUINFO->UIDLOTL;
    for(uint32_t i=0;i<4;i++)
    {
        product_id[j]=ptr[i];
        j++;
    }
    ptr=(uint8_t*)&MCUINFO->UIDW;
    for(uint32_t i=0;i<4;i++)
    {
        product_id[j]=(ptr[i] & 0xF0)+0x30;
        j++;
        product_id[j]=(ptr[i]& 0x0F)+0x30;
        j++;
    }
    product_id[j]=0;
}

uint8_t *get_product_id(void)
{
    return product_id;
}
