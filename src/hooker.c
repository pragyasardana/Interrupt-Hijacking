#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/desc.h>
#define DIVIDE_ERROR 0x00
struct desc_ptr newidtr, oldidtr;
gate_desc *newidt, *oldidt;
static int counter = 0;
unsigned long old_stub ;
extern asmlinkage void new_stub(void);
static void load_IDTR(void *addr)
{
    asm volatile("lidt %0"::"m"(*(unsigned short *)addr));
}
void my_func(void)
{
	++counter;
}
unsigned long new_fn=(unsigned long)&my_func; 
void my_dummy(void)
{
        __asm__ (
        ".globl new_stub    \n\t"
        ".align 4, 0x90     \n\t"
        "new_stub:      \n\t"
        "call *%0       \n\t"
        "jmp *%1      \n\t"
         ::"m"(new_fn), "m"(old_stub) );
}
int __init hook_init(void){
    store_idt(&oldidtr);
    oldidt = (gate_desc *)oldidtr.address;
    old_stub = (unsigned long)oldidtr.address;
    newidtr.address = __get_free_page(GFP_KERNEL);
    if(!newidtr.address){
	printk(KERN_INFO "exiting");
	return -1;
    }
    newidtr.size = oldidtr.size;
    newidt = (gate_desc *)newidtr.address; 
    memcpy(newidt, oldidt, oldidtr.size);
    pack_gate(&newidt[DIVIDE_ERROR], GATE_INTERRUPT, (unsigned long)new_stub, 0, 0, __KERNEL_CS);
    load_IDTR((void *)&newidtr);
    printk(KERN_INFO "Pragya Sardana: hijacked interrupt_0\r\n");
    return 0; 
} 
void __exit hook_exit(void){
    printk(KERN_ALERT "Pragya Sardana: recovered interrupt_0 \r\n");
    printk(KERN_ALERT "Pragya Sardana: Interrupt_0 handled during hijacking = %d \r\n", counter);
    load_IDTR(&oldidtr);
    if(newidtr.address)
        free_page(newidtr.address); 
}
  
module_init(hook_init);
module_exit(hook_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pragya Sardana");
