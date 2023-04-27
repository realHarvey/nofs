/*
 *  memory.c
 *  Copyright(c) 2023  Harvey
 */
#include "unistd.h"
#include "arm.h"
#include "../.config"
/*
 * 链式块存储
 *
 * malloc
 *      申请内存的时候会多申请12bytes用于存储该区块的size used *prev *next等描述
 *      采取最佳适配的方式切割内存, 用malloc效率换内存利用率
 *      遍历整个内存池, 获取到可用的内存块时, 标记其为best_match_block
 *      当寻找到匹配度更高的可用内存块时(超过需求量的程度越低, 匹配度越高), 更新best_match_block
 *
 *      如果用户启用强制malloc ( usage : #define FORCE_MALLOC )
 *      当由于碎片过多导致内存不足时, malloc会调用memory_defrag整理内存
 *      但这个时候会消耗过多的时间
 * free
 *      每次释放前都会尝试合并相邻的碎片
 *      相邻碎片只可能出现在defrag阶段, 而且最多只存在两块
 */

/* 设置系统内存池heap大小 : byte_t */
#if MM_SIZE == 10
  #define MEMORY_POOL_SIZE ( 2 * 1024 )
#elif MM_SIZE == 20
  #define MEMORY_POOL_SIZE ( 6 * 1024 )
#else
  #error "警告 : nofs暂未适配"
#endif

/*
 * 内存块
 *      used => 1 : 已用,  0 : 空闲
 */
struct mm_block_desc {
    unsigned short         size;
    unsigned short         used;
    struct mm_block_desc * prev;
    struct mm_block_desc * next;
};
/*
 * 内存池 :
 *      使用数组占用一块连续的内存作为内存池
 *      当申请内存时, 将内存池切割出需要的空间后链接
 */
unsigned char mm_pool[MEMORY_POOL_SIZE];
struct mm_block_desc * mm_pool_list = (void *)mm_pool;

static inline void init_pool()
{
        mm_pool_list->size = MEMORY_POOL_SIZE - 2 * sizeof(struct mm_block_desc);
        mm_pool_list->used = 0;
        mm_pool_list->prev = 0;
        mm_pool_list->next = 0;
}

void memset(void *addr, unsigned char val, int size) {
        int i;
        char *ptr;

        ptr = (char *)addr;
        for (i = 0; i < size; i++) {
                ptr[i] = val;
        }
}

/*
 * 内存切割
 */
static inline void memory_slice(struct mm_block_desc * memory, size_t size)
{
        struct mm_block_desc * restBlock = memory + size * 4 / sizeof(struct mm_block_desc); // 偏移后地址
        restBlock->size = memory->size - size;
        restBlock->used = 0;
        restBlock->prev = memory;
        restBlock->next = memory->next;
        memory->size = size;
        memory->used = 1;
        memory->next = restBlock;
}

/*
 * 获取内存池剩余内存大小 : byte_t
 */
static size_t get_rest_memory()
{
        struct mm_block_desc * mm_tranv;
        size_t mm_counter = 0;
        for (mm_tranv = mm_pool_list; mm_tranv; mm_tranv = mm_tranv->next) {
            if (!mm_tranv->used)
                mm_counter += mm_tranv->size + sizeof(struct mm_block_desc);
        }
        return mm_counter;
}

/*
 * 内存碎片整理
 *      返回整理后位于内存池最后区域空闲内存块的描述
 *      (!)非常消耗系统资源
 */
static struct mm_block_desc * memory_defrag()
{
        struct mm_block_desc * ffBlock = 0; // final-free-block
        struct mm_block_desc * mm_tranv;
        struct mm_block_desc * backup;
        unsigned int * defrager = (unsigned int*)mm_pool;
        size_t lenth;
        size_t this, next;
        for (mm_tranv = mm_pool_list; mm_tranv; mm_tranv = mm_tranv->next) {
                if ( mm_tranv->used ) {
                        defrager += sizeof(struct mm_block_desc) + mm_tranv->size;
                        continue;
                }
                // 数据前迁
                backup = mm_tranv->next;
                lenth  = backup->size;
                this   = sizeof(struct mm_block_desc) + mm_tranv->size;
                next   = this + mm_tranv->size + sizeof(struct mm_block_desc);
                for (; this < lenth; this++) {
                        defrager[this] = defrager[next];
                }
                ffBlock->size += mm_tranv->size;
                ffBlock->prev = mm_tranv;
                ffBlock->next = backup->next;
                mm_tranv->size = backup->size;
                mm_tranv->used = 1;
                mm_tranv->next = ffBlock;
                mm_tranv = ffBlock;
                // 条件合并下一块相邻碎片
                if ( !ffBlock->next->used ) {
                        ffBlock->next = ffBlock->next->next;
                        ffBlock->size += sizeof(struct mm_block_desc) + ffBlock->next->size;
                }
        }
        return ffBlock;
}

/* 4字节对齐 */
static inline size_t align4(size_t size)
{
        if ( size % 4 ) {
                size = ( size / 4 + 1 ) * 4;
        }
        return size;
}

#ifdef FORCE_MALLOC
static inline void force_inline(struct mm_block_desc *bmb, size_t size)
{
        if (bmb == mm_pool_list && get_rest_memory() >= size)
                bmb = memory_defrag();
}
#else
static inline void force_inline(struct mm_block_desc *bmb, size_t size) { }
#endif

void* malloc(size_t size)
{
        if ( !mm_pool_list->size && !mm_pool_list->used)
                init_pool();
        size_t _blockMsg   = sizeof(struct mm_block_desc);
        // 对齐申请, 保留储存内存块结构信息的空间
        size_t _memorySize = align4(size) + _blockMsg;
        struct mm_block_desc * mm_tranv = mm_pool_list;
        struct mm_block_desc * best_matchBlock = 0;
        /**
         * 现在开始从内存池中寻找最适合的block
         */
        cli(); // 避免竞态条件
        // 从池基地址开始查询
        for (; mm_tranv; mm_tranv = mm_tranv->next) {
                if (mm_tranv->used || mm_tranv->size < _memorySize)
                        continue;
                if ( mm_tranv->size == _memorySize ) {
                        best_matchBlock = mm_tranv;
                        break;
                }
                if ( mm_tranv->size > _memorySize ) {
                // 初始化最佳适配块为遍历到的第一块可用内存
                if (!best_matchBlock) {
                        best_matchBlock = mm_tranv;
                        continue;
                }
                if ((mm_tranv->size - _memorySize) < best_matchBlock->size)
                        best_matchBlock = mm_tranv;
                }
        }
        force_inline(best_matchBlock, _memorySize);
        memory_slice(best_matchBlock, _memorySize);
        sti();
        return ((void *)best_matchBlock + _blockMsg);
}

void free(void *obj)
{
        obj -= sizeof(struct mm_block_desc);
        // 检查ptr是否在内存池中
        if ((obj < (void *)mm_pool) ||
            (obj > (void *)(mm_pool + MEMORY_POOL_SIZE)) )
                oops("pointer is out of range");
        cli();
        struct mm_block_desc * usedBlock = obj;
        struct mm_block_desc * prevBlock = usedBlock->prev;
        // 如果上块内存不存在/已占用, 则直接标记obj未使用
        // 否则, 合并内存块
        if ( !prevBlock || prevBlock->used )
                usedBlock->used = 0;
        else {
                prevBlock->next = usedBlock->next;
                prevBlock->size += usedBlock->size + sizeof(struct mm_block_desc);
        }
        sti();
}