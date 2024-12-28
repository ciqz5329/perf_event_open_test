
static inline void disable_cntr_thru_clr(void)
{
        asm volatile("msr pmcntenclr_el0, %0" :: "r" BIT(31));
}

static inline void enable_cntr_thru_set(void)
{
        asm volatile("msr pmcntenset_el0, %0" :: "r" BIT(31));
}

static inline void enable_user_access(void)
{
        u64 val;
        asm volatile("mrs %0, pmuserenr_el0" : "=r"(val));
        printk("Before enable, pmuserenr_el0 = 0x%llx\n", val);

        val = BIT(0);
        asm volatile("msr pmuserenr_el0, %0" :: "r"(val));
        asm volatile("mrs %0, pmuserenr_el0" : "=r"(val));
        printk("After enable, pmuserenr_el0 = 0x%llx\n", val);
}

static inline u64 readcntr_thru_sel(void)
{
        u64 val = 0x1F; /// To read PMCCNTR;

        printk("Set PMSLER to 0x%llx\n", val);
        asm volatile("msr pmselr_el0, %0" :: "r"(val));

        printk("Now reading PMC through PMXEVCNTR_EL0\n");
        asm volatile("mrs %0, pmxevcntr_el0" : "=r"(val));
        printk("It is: 0x%llx\n", val);

        asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
        printk("pmccntr_el0: 0x%llx\n", val);

        return val;
}

static inline uint64_t
read_pmccntr(void)
{
        uint64_t val;
        int cpu = smp_processor_id();

        asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
        printk("%d: pmccntr_el0: 0x%llx\n", cpu, val);
        asm volatile("mrs %0, pmcr_el0" : "=r"(val));
        printk("%d: pmcr_el0: 0x%llx\n", cpu, val);

        asm volatile("mrs %0, pmcntenset_el0" :"=r"(val));
        printk("%d: pmcntenset_el0: 0x%llx\n", cpu, val);

        val |= (1 << 0);
        asm volatile("msr pmcr_el0, %0" : : "r"(val));
        asm volatile("mrs %0, pmcr_el0" : "=r"(val));
        printk("%d: pmcr_el0: 0x%llx\n", cpu, val);

        enable_cntr_thru_set();
        enable_user_access();

        asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
        printk("%d: pmccntr_el0: 0x%llx\n", cpu, val);
        return val;
}

static int __init pmu1_init(void)
{
        u64 rval = 0;

        asm volatile("mrs %0, ID_AA64DFR0_EL1" : "=r"(rval));
        printk("AA64DFR0_EL1 = 0x%llx\n", rval);
        read_pmccntr();

        readcntr_thru_sel();
        return 0;
}

static void __exit pmu1_exit(void)
{
}
module_init(pmu1_init);
module_exit(pmu1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tao.Wang");