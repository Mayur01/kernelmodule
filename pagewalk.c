#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/types.h>
#include<linux/sched.h>
#include<asm/pgtable.h>

MODULE_LICENSE("GPL");
int prid=0;
module_param(prid,int,S_IRUGO);

struct pid *pagewalk_pid;
struct task_struct *pid_struct;
struct mm_struct *pid_mm_struct;
struct vm_area_struct *vma;


int __init pagewalk_init(void)
{ 
    printk(KERN_INFO "Process ID: %d\n", prid);
   struct pid *pagewalk_pid;
   struct task_struct *pid_struct;
   struct mm_struct *pid_mm_struct;
   struct vm_area_struct *vma;
   unsigned long vaddr;
   unsigned long phy;
   // pte_t phy;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep;
    //pte_t pte; 
    pagewalk_pid = find_get_pid(prid);
    pid_struct = pid_task(pagewalk_pid, PIDTYPE_PID);
    pid_mm_struct = pid_struct->mm;
    //printk(KERN_INFO "Virtual address: %d\n", pgd);

    for (vma = pid_mm_struct->mmap; vma; vma= vma->vm_next)
    {
      for(vaddr= vma->vm_start; vaddr< vma->vm_end; vaddr++)
      {

        pgd = pgd_offset(pid_mm_struct, vaddr);
        if (!pgd_present(*pgd)){
          printk(KERN_INFO "PGD Not Found!\n");
            return 0;
          }

        p4d = p4d_offset(pgd, vaddr);
        if (!p4d_present(*p4d)){
          printk(KERN_INFO "P4D Not Found!\n");
            return 0;
        }

        pud = pud_offset(p4d, vaddr);
        if (!pud_present(*pud)){
          printk(KERN_INFO "PUD Not Found!\n");
            return 0;
        }

        pmd = pmd_offset(pud, vaddr);
        if (!pmd_present(*pmd)){
          printk(KERN_INFO "PMD Not Found!\n");
            return 0;
        }

        ptep = pte_offset_map(pmd, vaddr);
        if (!pte_present(*ptep)){
    //      printk(KERN_INFO "PTE Not Found!\n");
            return 0;
        }

        phy = pte_pfn(*ptep);
        if (!pfn_valid(phy)){
          printk(KERN_INFO "Physical address Not valid!\n");
            return 0;
        }
        printk(KERN_INFO "Virtual Addr %lX ,Physical address %lX\n",vaddr,phy);
      }

    }

 return 0;
}

void __exit pagewalk_exit(void)
{
  printk(KERN_ALERT "Cleaning resources... and exiting!!\n");

}
module_init(pagewalk_init);
module_exit(pagewalk_exit);

