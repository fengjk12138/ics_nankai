#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <device/map.h>
#include <stdlib.h>
#include <time.h>

static uint8_t pmem[PMEM_SIZE]
PG_ALIGN = {};

void *guest_to_host(paddr_t addr) { return &pmem[addr]; }

paddr_t host_to_guest(void *addr) { return (void *) pmem - addr; }

IOMap *fetch_mmio_map(paddr_t addr);

void init_mem() {
#ifndef DIFF_TEST
    srand(time(0));
    uint32_t *p = (uint32_t *) pmem;
    int i;
    for (i = 0; i < PMEM_SIZE / sizeof(p[0]); i++) {
        p[i] = rand();
    }
#endif
}

static inline bool in_pmem(paddr_t addr) {
return (PMEM_BASE <= addr) && (addr <= PMEM_BASE + PMEM_SIZE - 1);
}

static inline word_t pmem_read(paddr_t addr, int len) {
void *p = &pmem[addr - PMEM_BASE];
switch (len) {
case 1: return *(uint8_t  *)p;
case 2: return *(uint16_t *)p;
case 4: return *(uint32_t *)p;
default: assert(0);
}
}

static inline void pmem_write(paddr_t addr, word_t data, int len) {
    void *p = &pmem[addr - PMEM_BASE];
    switch (len) {
        case 1:
            *(uint8_t *) p = data;
            return;
        case 2:
            *(uint16_t *) p = data;
            return;
        case 4:
            *(uint32_t *) p = data;
            return;
        default:
            assert(0);
    }
}

/* Memory accessing interfaces */

inline word_t paddr_read(paddr_t addr,int len) {
if (in_pmem(addr))
return pmem_read(addr, len);
else return map_read(addr, len, fetch_mmio_map(addr));
}

inline void paddr_write(paddr_t addr, word_t data, int len) {
    if (in_pmem(addr)) pmem_write(addr, data, len);
    else map_write(addr, data, len, fetch_mmio_map(addr));
}


word_t vaddr_read_cross_page(vaddr_t addr, int type, int len) {
    int len1 = PAGE_SIZE - (addr & 0xfff);
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    pg_base=pg_base& ~0xfff;
    paddr_t paddr = (pg_base & ~0xfff) | (addr & 0xfff);
    word_t low_part;
    if(len1==1||len1==2)
        low_part=paddr_read(paddr, len1);
    else if(len1==3)
        low_part=paddr_read(paddr, 2)+(paddr_read(paddr+2, 1)<<16);
    else assert(0);
    pg_base = isa_mmu_translate(addr + len, type, len) & ~0xfff;
    word_t high_part;
    if(len-len1==2||len-len1==1)
        high_part=paddr_read(pg_base, len-len1);
    else if(len-len1==3)
        high_part=paddr_read(pg_base, 2)+(paddr_read(pg_base + 2, 1)<<16);
    else assert(0);
    return (high_part<<(8*len1))+low_part;
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    if ((pg_base & 0xfff)==MEM_RET_OK) {
        paddr_t paddr = (pg_base & ~0xfff) | (addr & 0xfff);
//        assert(addr==paddr);
        return paddr_read(paddr, len);
    } else if ((pg_base & 0xfff)==MEM_RET_CROSS_PAGE) {
        return vaddr_read_cross_page(addr, type, len);
    }
    return 0;
}

void vaddr_write_cross_page(vaddr_t addr, word_t data, int len) {
    int len1 = PAGE_SIZE - (addr & 0xfff);
    paddr_t pg_base = isa_mmu_translate(addr, MEM_TYPE_WRITE, len);
    pg_base=pg_base& ~0xfff;
    paddr_t paddr = (pg_base & ~0xfff) | (addr & 0xfff);
    if(len1==1||len1==2)
        paddr_write(paddr, data&((1<<(len1*8))-1), len1);
    else if(len1==3) {
        paddr_write(paddr, data&0xffff, 2);
        paddr_write(paddr+2, (data>>16)&0xff, 1);
    }else assert(0);

    pg_base = isa_mmu_translate(addr + len, MEM_TYPE_WRITE, len) & ~0xfff;
    if(len-len1==1||len-len1==2)
        paddr_write(pg_base, data>>(len1*8), len-len1);
    else if(len-len1==3){
        paddr_write(pg_base, (data>>8) & 0xffff , 2);
        paddr_write(pg_base+2, (data>>24) & 0xff , 1);
    }else assert(0);
}

void vaddr_mmu_write(vaddr_t addr, word_t data, int len){
    paddr_t pg_base = isa_mmu_translate(addr, MEM_TYPE_WRITE, len);
    if ((pg_base & 0xfff)==MEM_RET_OK) {
        paddr_t paddr = (pg_base & ~0xfff) | (addr & 0xfff);
//        assert(addr==paddr);
        paddr_write(paddr, data, len);
    } else if ((pg_base & 0xfff)==MEM_RET_CROSS_PAGE) {
        vaddr_write_cross_page(addr, data, len);
    }
}


#define def_vaddr_template(bytes) \
word_t concat(vaddr_ifetch, bytes) (vaddr_t addr) { \
  int ret = isa_vaddr_check(addr, MEM_TYPE_IFETCH, bytes); \
  if (ret == MEM_RET_OK) return paddr_read(addr, bytes);   \
  else if (ret == MEM_RET_NEED_TRANSLATE) return vaddr_mmu_read(addr, bytes, MEM_TYPE_IFETCH);         \
  return 0; \
} \
word_t concat(vaddr_read, bytes) (vaddr_t addr) { \
  int ret = isa_vaddr_check(addr, MEM_TYPE_READ, bytes); \
  if (ret == MEM_RET_OK) return paddr_read(addr, bytes);   \
  else if (ret == MEM_RET_NEED_TRANSLATE) return vaddr_mmu_read(addr, bytes, MEM_TYPE_READ); \
  return 0; \
} \
void concat(vaddr_write, bytes) (vaddr_t addr, word_t data) { \
  int ret = isa_vaddr_check(addr, MEM_TYPE_WRITE, bytes); \
  if (ret == MEM_RET_OK)          \
  paddr_write(addr, data, bytes);   \
  else if (ret == MEM_RET_NEED_TRANSLATE) vaddr_mmu_write(addr, data, bytes);         \
  }


def_vaddr_template(1)

def_vaddr_template(2)

def_vaddr_template(4)
