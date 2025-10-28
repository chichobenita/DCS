#include  "../header/fs.h"
#include  "../header/LCD.h"
#include <string.h>
#include <msp430.h>           /* FlashCtl_*  */


/* === משתנים פנימיים =================================================== */
static fs_table_t  fat_ram;              /* FAT ב-RAM (64 B)              */
static uint16_t    file_alloc_ptr;       /* מצביע הכתיבה (top-down)      */

//typedef struct { uint16_t cur, left; } hnd_t;
//static hnd_t       handles[MAX_FILES];   /* ידיות פשוטות לקריאה          */
typedef struct {
    int8_t   idx;     // איזה קובץ פתוח
    uint16_t base;    // FILE_AREA_START + start*2  (כתובת התחלה מוחלטת בפלאש)
    uint16_t left;    // כמה נשאר לקרוא
    uint16_t size0;   // גודל מקורי לצורך חישוב offset = size0-left
} hnd_t;

static hnd_t hnd = { .idx = -1, .base = 0, .left = 0, .size0 = 0 };



bool fs_add_entry(const fs_entry_t *e)
{
    /* נסה למחזר סלוטים מחוקים (size==0) */
    uint8_t i;
    for (i=0; i<fat_ram.num; ++i) {
        if (fs_e_size(&fat_ram.ent[i]) == 0) {
            fat_ram.ent[i] = *e;
            fat_save();
            return true;
        }
    }
    /* אחרת – הוסף חדש אם יש מקום */
    if (fat_ram.num >= MAX_FILES) return false;
    fat_ram.ent[ fat_ram.num++ ] = *e;
    fat_save();
    return true;
}


void fs_set_alloc_ptr(uint16_t p)
{
    file_alloc_ptr = p;
}

/* === CRC -8/16 פשוטות ================================================= */
/* --- עזרים ל-CRC --- */

/* חישוב CRC של טבלת ה-FAT לפי מבנה: header 4B (magic,num,crc16) + ent[0..num-1] */
static uint16_t fat_crc_compute(const fs_table_t *t)
{
    uint16_t c = 0xFFFF;
    /* header: magic + num */
    c = crc16_update(c, &t->magic, 2);
    /* entries: רק num רשומות × 6B */
    if (t->num <= MAX_FILES) {
        c = crc16_update(c, (const uint8_t*)t->ent, (uint16_t)(6u * t->num));
    }
    return c;
}



static uint16_t crc16_update(uint16_t c, const uint8_t *p, uint16_t n)
{
    while (n--) {
        c ^= *p++;
        uint8_t i;
        for (i = 0; i < 8; ++i)
            c = (c & 1) ? (c >> 1) ^ 0xA001 : (c >> 1);
    }
    return c;
}
static uint8_t xor_bytes(const uint8_t *p, uint16_t n)
{
    uint8_t x = 0;
    while (n--) x ^= *p++;
    return x;
}

static inline uint16_t seg_align(uint16_t addr)
{
    return (addr >= 0x1000u && addr <= 0x10FFu) ? (addr & ~0x003Fu)   // Info: 64B
                                                 : (addr & ~0x01FFu); // Main: 512B
}
static void flash_erase(uint16_t anyAddr)
{
    uint16_t seg = seg_align(anyAddr);
    __disable_interrupt();
    FCTL2 = FWKEY | FSSEL_1 | FN0;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY | ERASE;
    *(volatile uint16_t *)seg = 0;      // trigger erase
    while (FCTL3 & BUSY) ;
    FCTL1 = FWKEY;
    FCTL3 = FWKEY | LOCK;
    __enable_interrupt();
}


/* כתיבת len בייטים, מאחה כל 2 בייטים למילת-16bit; אם נשאר בייט בודד  ממלא 0xFF בבייט הגבוה */
static void flash_write(const uint8_t *src, uint16_t dst, uint16_t len)
{
    __disable_interrupt();
    FCTL2 = FWKEY | FSSEL_1 | FN0;   // תדר כתיבה
    FCTL3 = FWKEY;                   // פתח LOCK
    FCTL1 = FWKEY | WRT;             // מצב כתיבה

    while (len) {
        uint16_t w = src[0];
        if (len > 1) {               // יש בייט שני
            w |= ((uint16_t)src[1]) << 8;
            src += 2;
            len -= 2;
        } else {                     // בייט אחרון  ריפוד
            w |= 0xFF00;
            src += 1;
            len -= 1;
        }
        *(volatile uint16_t *)dst = w;
        dst += 2;
        while (FCTL3 & BUSY) ;
    }

    FCTL1 = FWKEY;                   // נקה WRT
    FCTL3 = FWKEY | LOCK;            // נעל
    __enable_interrupt();
}




/* === FAT load/save ===================================================== */
static void fat_save(void)
{
    fat_ram.magic = 0x5A;
    fat_ram.crc16 = fat_crc_compute(&fat_ram);   /*  במקום 62 קבוע */
    flash_erase(FAT_ADDR);
    flash_write((uint8_t*)&fat_ram, FAT_ADDR, 64);
}
bool fs_init(void)
{
    memcpy(&fat_ram, (void*)FAT_ADDR, sizeof(fat_ram));

    bool bad = false;
    if (fat_ram.magic != 0x5A) bad = true;
    if (fat_ram.num > MAX_FILES) bad = true;
    if (!bad) {
        if (fat_crc_compute(&fat_ram) != fat_ram.crc16) bad = true;
    }
    if (bad) {
        memset(&fat_ram, 0, sizeof(fat_ram));
        fat_save();  /* טבלה ריקה חדשה */
    }

    /* חישוב file_alloc_ptr... כמו שהיה אצלך, אבל עם fs_e_size() */
    file_alloc_ptr = FILE_AREA_END + 1;
    uint8_t i;
    for (i=0; i<fat_ram.num; i++) {
        if (fs_e_size(&fat_ram.ent[i])) {
            uint16_t addr = FILE_AREA_START + fat_ram.ent[i].start*2;
            if (addr < file_alloc_ptr) file_alloc_ptr = addr;
        }
    }
    if (file_alloc_ptr > FILE_AREA_END + 1) file_alloc_ptr = FILE_AREA_END + 1;

    hnd.idx = -1;
    hnd.base = 0;
    hnd.left = 0;
    hnd.size0 = 0;
    return true;
}


/* === API: create ======================================================= */
int16_t fs_create(uint8_t name_id, uint8_t type, uint8_t flags,
                  const uint8_t *data, uint16_t size)
{
    if (fat_ram.num >= MAX_FILES)                    return -1;   /* מלא   */
    if (file_alloc_ptr - size < FILE_AREA_START)     return -2;   /* אין מקום */

    uint16_t dst = file_alloc_ptr - size;            /* כתיבת top-down */
    uint16_t cur = dst;
    uint16_t left = size;

    while (cur < file_alloc_ptr) {
        if ((cur & 0x1FF) == 0) flash_erase(cur);    /* erase segment */
        uint16_t chunk = ((cur | 0x1FF) - cur) + 1;  /* עד סוף סגמנט */
        if (chunk > left) chunk = left;
        flash_write(data, cur, chunk);
        data += chunk; cur += chunk; left -= chunk;
    }

    /* רשומה חדשה */
    fs_entry_t *e = &fat_ram.ent[fat_ram.num++];
    e->start = (dst - FILE_AREA_START) >> 1;
    fs_e_set(e, size, type);               /* במקום e->size / e->type */
    e->meta  = (flags << 4) | (name_id & 0x0F);
    e->crc8  = xor_bytes((uint8_t*)dst, size);

    fat_save();
    file_alloc_ptr = dst;
    return fat_ram.num-1;                             /* handle == index */
}

/* === open/read/close =================================================== */
int16_t fs_open(uint8_t name_id)
{
    uint8_t i;
    for (i = 0; i < fat_ram.num; ++i)
        {
        if ((fat_ram.ent[i].meta & 0x0F) == name_id && fat_ram.ent[i].size_type)
            {
            hnd.idx  = i;
            hnd.base = (uint16_t)(FILE_AREA_START + ((uint16_t)fat_ram.ent[i].start << 1));
            hnd.left = fs_e_size(&fat_ram.ent[i]);
            hnd.size0 = hnd.left;
            return i;
            }
        }
    return -1;                        // לא נמצא
}


int16_t fs_read(int16_t h, uint8_t *dst, uint16_t len)
{
    if (h != hnd.idx || hnd.idx < 0) return -1;
    if (hnd.left == 0)               return 0;

    if (len > hnd.left) len = hnd.left;

    uint16_t off = (uint16_t)(hnd.size0 - hnd.left);
    uint16_t cur = (uint16_t)(hnd.base + off);

    // שמירת טווח (תופס חריגות מיד)
    if (cur < FILE_AREA_START || (uint32_t)cur + len - 1 > FILE_AREA_END)
        return -2;

    memcpy(dst, (void*)cur, len);
    hnd.left = (uint16_t)(hnd.left - len);
    return (int16_t)len;
}



void fs_close(int16_t h)
{
    if (h == hnd.idx)
    {
        hnd.idx = -1;
        hnd.base = 0;
        hnd.left = 0;
        hnd.size0 = 0;
    }
}


/* === delete (סימון בלבד) ============================================== */
bool fs_delete(uint8_t name_id)
{
    uint8_t i;
    for (i=0;i<fat_ram.num;i++) {
        fs_entry_t *e = &fat_ram.ent[i];
        if ((e->meta & 0x0F) == name_id && fs_e_size(e)) {
            fs_e_set(e, 0, fs_e_type(e));   /* “מחיקה” = גודל 0 */
            fat_save();
            return true;
        }
    }
    return false;
}

/* === list helper ======================================================= */
void fs_list(void (*cb)(const fs_entry_t*))
{
    uint8_t i;
    for (i=0;i<fat_ram.num;i++)
        if (fs_e_size(&fat_ram.ent[i])) cb(&fat_ram.ent[i]);
}


bool fs_format(){
    flash_erase(FAT_ADDR);     // מוחק 64B של Info-B בלבד
    memset(&fat_ram, 0, sizeof(fat_ram));
    fat_save();                   // יוצר טבלה ריקה חדשהד
    fs_repack();
    return true;
}


/*-------------------------------------------------------------*/
/*  Compact all live files to the very top of FILE_AREA        */
/*  Returns true   – success                                   */
/*          false  – flash write error (very unlikely)         */
/*-------------------------------------------------------------*/
bool fs_repack(void)
{
    /* --- משתנים (C89: הצהרה בראש בלוק) --- */
    uint8_t  buf[64];
    uint16_t new_addr[MAX_FILES];
    int      order[MAX_FILES];
    int      live;
    int      i, a, b, k;

    uint16_t dst_ptr;
    uint16_t final_top;
    uint16_t scratchSeg;

    /* מעקב סגמנטים שנמחקו כדי לא למחוק פעמיים */
    uint8_t  erased[MAX_SEGS];

    /* אפס מערכים */
    for (i = 0; i < MAX_FILES; ++i) new_addr[i] = 0;
    for (i = 0; i < MAX_SEGS;  ++i) erased[i]   = 0;

    /* --- שלב 0: אסוף קבצים חיים וסדר לפי "חדשישן" --- */
    live = 0;
    for (i = 0; i < fat_ram.num; ++i) {
        if (fs_e_size(&fat_ram.ent[i])) order[live++] = i;
    }
    for (a = 0; a < live-1; ++a) {
        for (b = a+1; b < live; ++b) {
            if (order[a] < order[b]) { int t = order[a]; order[a] = order[b]; order[b] = t; }
        }
    }

    /* חשב יעד חדש לכל קובץ וצבור את הגובה הכולל */
    dst_ptr = FILE_AREA_END + 1u;
    for (k = 0; k < live; ++k) {
        fs_entry_t *e = &fat_ram.ent[ order[k] ];
        uint16_t sz = fs_e_size(e);
        dst_ptr -= sz;
        new_addr[ order[k] ] = dst_ptr;
        if (sz & 1u) dst_ptr -= 1u;      /* ריפוד FF לבייט בודד */
    }
    final_top = dst_ptr;                              /* תחילת האזור הדחוס */

    /* בחר סגמנט סקראץ': הסגמנט שמתחת ל-final_top */
    scratchSeg = SEG_START(final_top);
    if (scratchSeg >= (FILE_AREA_START + SEG_SIZE)) {
        scratchSeg -= SEG_SIZE;
    } else {
        /* אין מקום לסקראץ' בטוח */
        return false;
    }
    flash_erase(scratchSeg);                          /* נקה את הסקראץ' לפתיחה */

    /* --- שלב 1: עבור על סגמנטי-היעד מלמעלה (FDxx) עד ל-limit --- */
    {
        uint16_t dstSeg = SEG_START(FILE_AREA_END);
        uint16_t limit  = SEG_START(final_top);

        while (1) {
            int files_in_dst[MAX_FILES];
            int n_in_dst = 0;

            /* מי מהקבצים הדחוסים מתיישב בסגמנט היעד הזה */
            for (k = 0; k < live; ++k) {
                i = order[k];
                if (SEG_START(new_addr[i]) == dstSeg) {
                    files_in_dst[n_in_dst++] = i;
                }
            }

            if (n_in_dst > 0) {
                uint16_t s_ptr;
                uint16_t scratch_ofs[MAX_FILES];

                /* אפס טבלת scratch_ofs */
                for (i = 0; i < MAX_FILES; ++i) scratch_ofs[i] = 0;
                s_ptr = scratchSeg;

                /* 1.A – גבה לסקראץ' קבצים שמקורם באותו סגמנט (dstSeg) */
                for (k = 0; k < n_in_dst; ++k) {
                    uint16_t src;
                    uint16_t left;
                    uint16_t cur;
                    uint16_t d;
                    fs_entry_t *e2;

                    i  = files_in_dst[k];
                    e2 = &fat_ram.ent[i];
                    src  = (uint16_t)(FILE_AREA_START + (e2->start << 1));
                    if (SEG_START(src) == dstSeg) {
                        uint16_t sz2 = fs_e_size(e2);
                        left = sz2;
                        cur  = src;
                        d    = s_ptr;
                        while (left) {
                            uint16_t chunk = (left > sizeof(buf)) ? (uint16_t)sizeof(buf) : left;
                            memcpy(buf, (void*)cur, chunk);
                            flash_write(buf, d, chunk);
                            cur  += chunk;
                            d    += chunk;
                            left -= chunk;
                        }
                        if (sz2 & 1u) {
                            uint8_t ff = 0xFF;
                            flash_write(&ff, d, 1);
                            d += 1u;
                        }
                        scratch_ofs[i] = s_ptr;
                        s_ptr = d;
                    }
                }

                /* 1.B – מחק את סגמנט היעד (פעם אחת) */
                if (!erased[ SEG_INDEX(dstSeg) ]) {
                    flash_erase(dstSeg);
                    erased[ SEG_INDEX(dstSeg) ] = 1;
                }

                /* 1.C – כתוב את כל הקבצים הנ"ל אל היעדים שלהם */
                for (k = 0; k < n_in_dst; ++k) {
                    fs_entry_t *e3;
                    uint16_t dst;
                    uint16_t left;
                    uint16_t cur;

                    i   = files_in_dst[k];
                    e3  = &fat_ram.ent[i];
                    dst  = new_addr[i];
                    left = fs_e_size(e3);

                    if (scratch_ofs[i]) {
                        /* מקור גובה לסקראץ' */
                        cur = scratch_ofs[i];
                    } else {
                        /* מקור נשאר בסגמנט אחר – קרא ישירות */
                        cur = (uint16_t)(FILE_AREA_START + (e3->start << 1));
                    }

                    while (left) {
                        uint16_t chunk = (left > sizeof(buf)) ? (uint16_t)sizeof(buf) : left;
                        memcpy(buf, (void*)cur, chunk);
                        flash_write(buf, dst, chunk);
                        cur  += chunk;
                        dst  += chunk;
                        left -= chunk;
                    }
                    if (fs_e_size(e3) & 1u) {
                        uint8_t ff = 0xFF;
                        flash_write(&ff, dst, 1);
                    }

                    /* עדכון start16 ברשומה */
                    e3->start = (uint16_t)((new_addr[i] - FILE_AREA_START) >> 1);
                }

                /* 1.D – נקה את הסקראץ' לסיבוב הבא (לא חובה, אבל נקי) */
                flash_erase(scratchSeg);
            }

            if (dstSeg == limit) break;
            dstSeg = (uint16_t)(dstSeg - SEG_SIZE);
        }
    }

    /* --- שלב 2: עדכן מצביע הקצאה ושמור FAT --- */
    file_alloc_ptr = final_top;
    fat_save();
    return true;
}

/* --- הקשר פנימי של סשן העלאה (לא תלוי ב-API) --- */
typedef struct {
    uint8_t  active;
    uint8_t  seq;             /* ה-seq הבא שמצופה */
    uint16_t total, recv;
    uint16_t crc16_goal, crc16_run;
    uint8_t  crc8_run;

    uint8_t  type, meta;
    uint16_t dst;             /* כתובת יעד (בבתים) מלמעלה-למטה */

    uint16_t wpos;        /* מצביע כתיבה בפלאש (בבתים) */

    uint8_t  carry_valid; /* יש בייט “תלוי” שלא נכתב */
    uint8_t  carry;
} fs_up_ctx_t;

static fs_up_ctx_t up;
void fs_up_reset(void)
{
    memset(&up, 0, sizeof(up));
}
/* מאפשר ל-API לקבל את היעד כדי לעדכן file_alloc_ptr */
uint16_t fs_up_dst(void) { return up.dst; }


/* --- START: רזרבציה+מחיקת סגמנטים יעד --- */
fs_status_t fs_up_start(uint8_t ftype, uint8_t meta,
                        uint16_t total, uint16_t crc16_goal)
{
    uint16_t need, dst, end;

    if (up.active) return FS_ERR_BUSY;

    need = (uint16_t)(total + (total & 1u));      /* יישור למילה */

    if ((uint16_t)(file_alloc_ptr - need) < FILE_AREA_START)
        return FS_ERR_NOSPACE;

    dst = (uint16_t)(file_alloc_ptr - need);
    end = (uint16_t)(dst + need - 1u);

    /* --- מחיקה סלקטיבית של סגמנטים יעד --- */
    {
        uint16_t first    = SEG_START(dst);
        uint16_t last     = SEG_START(end);
        uint16_t frontier = SEG_START((uint16_t)(file_alloc_ptr - 1u));
        uint16_t a;

        for (a = first; ; a = (uint16_t)(a + SEG_SIZE)) {
            /* אם יש כבר קובץ קודם (file_alloc_ptr ירד) והסגמנט הוא “החזית” – אל תמחק */
            if (!((file_alloc_ptr != (FILE_AREA_END + 1u)) && (a == frontier))) {
                flash_erase(a);
            }
            if (a == last) break;
        }
    }

    /* אתחול הקשר */
    up.active     = 1;
    up.seq        = 0;
    up.total      = total;
    up.recv       = 0;
    up.crc16_goal = crc16_goal;
    up.crc16_run  = 0xFFFF;
    up.crc8_run   = 0;
    up.type       = ftype;
    up.meta       = meta;
    up.dst        = dst;
    up.wpos       = dst;          /*  חדש: מאתחלים מצביע כתיבה */
    up.carry_valid = 0;           /*  חדש */

    return FS_OK;
}


/* --- CHUNK: כתיבה רציפה לפלאש ואימות סדר --- */
fs_status_t fs_up_chunk(uint8_t seq, const uint8_t *data, uint8_t n)
{
    if (!up.active)         return FS_ERR_STATE;
    if (n == 0)             return FS_ERR_FORMAT;
    if (seq != up.seq) {
        uint8_t prev = (uint8_t)((up.seq + 0x0F) & 0x0F);
        if (seq == prev) return FS_OK;    /* דופליקט – אישור שקט */
        return FS_ERR_SEQ;
    }
    if ((uint16_t)(up.recv + n) > up.total) return FS_ERR_SIZE;

    const uint8_t *p = data;
    uint16_t left = n;

    /* אם יש carry מהצ’אנק הקודם – חבר עם הבייט הראשון וכתוב מילה אחת */
    if (up.carry_valid) {
        uint8_t pair[2] = { up.carry, *p };
        flash_write(pair, up.wpos, 2);
        up.wpos += 2;
        up.carry_valid = 0;
        p++; left--;
    }

    /* כתיבת “גוש” זוגי ישירות */
    uint16_t bulk = (uint16_t)(left & (uint16_t)~1u); /* even */
    if (bulk) {
        flash_write(p, up.wpos, bulk);
        up.wpos += bulk;
        p      += bulk;
        left   -= bulk;
    }

    /* אם נשאר בייט אחרון – שמור ל־carry, נכתוב רק בצ’אנק הבא / END */
    if (left == 1) {
        up.carry       = *p;
        up.carry_valid = 1;
    }

    /* סטטיסטיקות/CRC */
    up.recv       = (uint16_t)(up.recv + n);                  /* קיבלנו n בייט */
    up.crc16_run  = crc16_update(up.crc16_run, data, n);
    up.crc8_run  ^= xor_bytes(data, n);
    up.seq        = (uint8_t)((up.seq + 1) & 0x0F);

    return FS_OK;
}




fs_entry_t* fs_get_entry(uint8_t idx)
{
    if (idx >= fat_ram.num) return NULL;
    const fs_entry_t *e = &fat_ram.ent[idx];
    if (fs_e_size(e) == 0)
        return NULL;
    return e;
}



/* --- END: אימותים ובניית רשומת FAT --- */
fs_status_t fs_up_end(fs_entry_t *out_e)
{
    if (!up.active)              return FS_ERR_STATE;
    if (up.recv != up.total)     return FS_ERR_SIZE;
    if (up.total && up.crc16_run != up.crc16_goal)
        return FS_ERR_CRC;

    /* אם נשאר carry – כותבים אותו כרבע מילה מרופדת */
    if (up.carry_valid) {
        uint8_t pair[2] = { up.carry, 0xFF };
        flash_write(pair, up.wpos, 2);
        up.wpos       += 2;
        up.carry_valid = 0;
    }

    if (out_e) {
        out_e->start = (uint16_t)((up.dst - FILE_AREA_START) >> 1);
        fs_e_set(out_e, up.total, up.type);
        out_e->meta  = up.meta;
        out_e->crc8  = up.crc8_run;
    }

    up.active = 0;
    return FS_OK;
}



uint8_t fs_count(void)
{
    return fat_ram.num;
}


// מצב גלובלי
static uint8_t  file_iter_idx = 0;         // האינדקס של “הקובץ הבא”
volatile uint8_t btn_nextfile_flag = 0;    // דגל לחיצה

// מדפיס את 10 הבתים הראשונים של הקובץ החי הבא
void ui_show_first10_next(void)
{

    uint8_t nfiles = fs_count();
    if (nfiles == 0) {
        return;
    }

    // ננסה לכל היותר nfiles פעמים (דלג על מחוקים)
    uint8_t tries;
    for (tries = 0; tries < nfiles; ++tries)
    {
        if (file_iter_idx >= nfiles) file_iter_idx = 0;

        fs_entry_t* e = fs_get_entry(file_iter_idx);
        if (e != NULL) {
            uint16_t base = FILE_AREA_START + ((uint16_t)e->start << 1);
            uint16_t size = fs_e_size(e);
            uint8_t  n    = (size < 16u) ? (uint8_t)size : 16u;
            char  buf[16];
            uint8_t  i;

            for (i = 0; i < n; ++i)
            {
                buf[i] = *((char*)(uintptr_t)(base + i));//(char(&base + i));
            }

            lcd_puts(buf);
            //print_bytes_hex(buf, n);

            file_iter_idx++;   // התקדמות לקובץ הבא לפעם הבאה
            return;
        }

        // הקובץ הזה לא “חי”  עבור לאינדקס הבא
        file_iter_idx++;
    }

    // אם הגענו לכאן – אין רשומות חיות
    //print_line("אין קבצים חיים");

}


static uint8_t old_file_iter_idx = 0;

void read_text_from_file(void)
{
    fs_entry_t* e = fs_get_entry(file_iter_idx - 1);   // אם ה-UI שלך 1-based
    if (!e) return;

    // אם עברנו לקובץ אחר – מאפסים "מצביע קריאה" פנימי
    if (old_file_iter_idx != file_iter_idx) {
        e->currentPointer = 0;
        old_file_iter_idx = file_iter_idx;
    }

        uint16_t total = fs_e_size(e);           // גודל כולל (שם+תוכן)
    if (total <= 16) {
        lcd_clear();
        lcd_puts("--EMPTY FILE--");
        return;
    }

    uint16_t payload_len = (uint16_t)(total - 16);   // גודל התוכן בלבד
    if (e->currentPointer >= payload_len) {
        // הגיעו לסוף – הודעה וסגירה יפה
        lcd_goto_xy(1, 0);
        lcd_puts("----THE END----");
        e->currentPointer = 0;
        return;
    }

    // כתובת בסיס של הבלוק הבא לקריאה – בתוך התוכן בלבד
    uint16_t base = (uint16_t)(FILE_AREA_START + ((uint16_t)e->start << 1) + 16 + e->currentPointer);

    // כמה בייטים עוד נשארו בקובץ (תוכן)
    uint16_t remaining = (uint16_t)(payload_len - e->currentPointer);

    // נקרא עד 16 בייט, אבל לא יותר ממה שנשאר
    uint8_t n = (remaining >= 16u) ? 16u : (uint8_t)remaining;

    char buf[17];                   // +1 לסיום מחרוזת
    uint8_t i;
    for (i = 0; i < n; ++i) {
        buf[i] = *((const char*)(uintptr_t)(base + i));
    }
    buf[n] = '\0';                  // חשוב ל-lcd_puts

    lcd_puts(buf);

    e->currentPointer = (uint16_t)(e->currentPointer + n);

    if (e->currentPointer >= payload_len) {
        lcd_goto_xy(1, 0);
        lcd_puts("----THE END----");
        e->currentPointer = 0;
    }
}





/* החזרת כתובת בסיס (בבתים) של גוף הקובץ בפלאש */
uint16_t fs_e_base(const fs_entry_t *e)
{
    /* start הוא offset במילים, לכן <<1 והוספת תחילת אזור הקבצים */
    return (uint16_t)(FILE_AREA_START + ((uint16_t)e->start << 1));
}

/* חיפוש רשומת קובץ לפי name_id (0..15). מחזיר NULL אם לא נמצא/מחוק */
const fs_entry_t* fs_find_by_id(uint8_t name_id)
{
    uint8_t i;
    for (i = 0; i < fat_ram.num; ++i) {
        const fs_entry_t *e = &fat_ram.ent[i];
        /* meta: 4 סיביות נמוכות = name_id; ודא שהקובץ “חי” (size>0) */
        if ( (e->meta & 0x0F) == name_id && fs_e_size(e) ) {
            return e;
        }
    }
    return NULL;
}


/* count – מספר דגימות (<=31 כדי לא לחרוג מ-64B עם ה-header),
   L1/L2 – המערכים שמכילים את הדגימות (16-bit) */
void ldr_calib_commit(const uint16_t* L1, const uint16_t* L2, uint8_t count)
{
    if (!L1 || !L2 || count == 0) return;
    if (count > 32) count = 32;                 /* 32 דגימות × 2 בתים = 64B סגמנט מלא */

    /* ---- LDR1  Info C ---- */
    {
        uint8_t  buf[2 * 32];
        uint16_t nbytes = (uint16_t)(2 * count);
        uint8_t i;
        for (i = 0; i < count; ++i) {
            buf[2*i]     = (uint8_t)(L1[i] & 0xFF);
            buf[2*i + 1] = (uint8_t)(L1[i] >> 8);
        }
        flash_erase(INFO_C_START);              /* מוחק את כל הסגמנט (64B) */
        flash_write(buf, INFO_C_START, nbytes); /* כותב החל מתחילת הסגמנט */
        /* שאר הסגמנט נשאר 0xFF (לא נוגע בו) */
    }

    /* ---- LDR2 Info D ---- */
    {
        uint8_t  buf[2 * 32];
        uint16_t nbytes = (uint16_t)(2 * count);
        uint8_t i;
        for (i = 0; i < count; ++i) {
            buf[2*i]     = (uint8_t)(L2[i] & 0xFF);
            buf[2*i + 1] = (uint8_t)(L2[i] >> 8);
        }
        flash_erase(INFO_D_START);
        flash_write(buf, INFO_D_START, nbytes);
    }
}


void ReadSampleLDR(uint16_t* out1, uint16_t* out2, uint8_t idx)
{
    if (!out1 || !out2) return;

    if (idx > 9) idx = 9;                         /* שמירה בגבולות */
    uint16_t addr1 = (uint16_t)(INFO_C_START + ((uint16_t)idx * 2u));
    uint16_t addr2 = (uint16_t)(INFO_D_START + ((uint16_t)idx * 2u));

    /* קריאה “גלויה” בבייטים (LE) כדי לא להיות תלוים באנדיאניות */
    uint16_t v1 =  (uint16_t)(*(const volatile uint8_t*)(uintptr_t)(addr1 + 0))
                 | (uint16_t)((*(const volatile uint8_t*)(uintptr_t)(addr1 + 1)) << 8);
    uint16_t v2 =  (uint16_t)(*(const volatile uint8_t*)(uintptr_t)(addr2 + 0))
                 | (uint16_t)((*(const volatile uint8_t*)(uintptr_t)(addr2 + 1)) << 8);

    if (v1 == 0xFFFFu) v1 = 0;                    /* תא לא כתוב  0 */
    if (v2 == 0xFFFFu) v2 = 0;

    *out1 = v1;
    *out2 = v2;
}

