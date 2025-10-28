#ifndef FS_H_
#define FS_H_

#include <stdint.h>
#include <stdbool.h>
#include  "../header/radar_protocol.h"


/* === קבועי פלטפורמה ==================================================== */
#define FILE_AREA_START     0xF600u           /* תחילת S4            */
#define FILE_AREA_END       0xFDFFu           /* סוף    S1           */
#define FILE_AREA_SIZE      0x0800u           /* 2 kB הכוללים       */
#define SCRATCH_SEG         FILE_AREA_START


#define INFO_D_START     0x1000u
#define INFO_C_START     0x1040u
#define INFO_B_START     0x1080u
#define INFO_A_START     0x10C0u

#define FAT_ADDR         INFO_B_START      /* טבלת FAT ב-Info B */

/* --- עזרים לאזורי FLASH (מאקרו, לא פונקציה) --- */
#ifndef SEG_SIZE
#define SEG_SIZE          0x0200u   /* 512B */
#endif

/* חשוב: FILE_AREA_START/END כבר מוגדרים אצלך (למשל 0xF600..0xFDFF) */

#define SEG_START(a)      ((uint16_t)((a) & (uint16_t)~0x01FFu))
#define SAME_SEG(a,b)     (SEG_START(a) == SEG_START(b))
#define SEG_INDEX(a)      ((uint16_t)(((uint16_t)(SEG_START(a) - FILE_AREA_START)) / SEG_SIZE))

/* כמה סגמנטים יש באזור הקבצים */
#define MAX_SEGS          ((uint16_t)((FILE_AREA_END - FILE_AREA_START + 1u) / SEG_SIZE))
#define NAME_TBL_SIZE       16                /* 16 שמות בני 16 תו */


enum {
    FTYPE_TXT = 1,
    FTYPE_SCR = 2
};

/* === API =============================================================== */
bool     fs_init(void);                               /* קריאה באתחול     */
int16_t  fs_create(uint8_t name_id,
                   uint8_t type,
                   uint8_t flags,
                   const uint8_t *data,
                   uint16_t size);
int16_t  fs_open(uint8_t name_id);                    /* מחזיר handle     */
int16_t  fs_read(int16_t h, uint8_t *dst, uint16_t len);
void     fs_close(int16_t h);
bool     fs_delete(uint8_t name_id);                  /* סימון size=0     */

bool     fs_format();
bool fs_repack(void);


/* --- סטטוס כללי להפעלה מה-API --- */
typedef enum {
    FS_OK = 0,
    FS_ERR_BUSY,
    FS_ERR_FORMAT,
    FS_ERR_NOSPACE,
    FS_ERR_SEQ,
    FS_ERR_STATE,
    FS_ERR_SIZE,
    FS_ERR_CRC,
    FS_ERR_TABLE_FULL
} fs_status_t;

/* ===== פריסת FAT: 4B header + 10×6B =====
   header: magic(1), num(1), crc16(2)
   entry : start16(2), size_type(2), meta(1), crc8(1) = 6B
*/


typedef struct {
    uint16_t start;       /* offset במילים (×2) מ- FILE_AREA_START */
    uint16_t size_type;   /* [15:12]=type (0..15), [11:0]=size bytes (0..4095) */
    uint8_t  meta;        /* flags<<4 | name_id */
    uint8_t  crc8;        /* XOR על גוף הקובץ */
    uint16_t currentPointer;
} fs_entry_t;

/* חשוב: crc16 מיד אחרי num (offset=2) */
typedef struct {
    uint8_t   magic;      /* 0x5A */
    uint8_t   num;        /* # רשומות פעילות (0..10) */
    uint16_t  crc16;      /* CRC-16 על (magic,num, ent[0..num-1]*6B) */
    fs_entry_t ent[MAX_FILES];
} fs_table_t;

/* מאקרואים לגישה לשדות הדחוסים */
static inline uint16_t fs_e_size (const fs_entry_t *e)
{ return (uint16_t)(e->size_type & 0x0FFFu); }

static inline uint8_t  fs_e_type (const fs_entry_t *e)
{ return (uint8_t)((e->size_type >> 12) & 0x0Fu); }

static inline void fs_e_set(fs_entry_t *e, uint16_t sz, uint8_t ty)
{ e->size_type = (uint16_t)(((uint16_t)(ty & 0x0F) << 12) | (sz & 0x0FFF)); }



void     fs_list(void (*cb)(const fs_entry_t*));      /* הדפסת FAT        */


/* מנוע ההעלאה */
fs_status_t fs_up_start(uint8_t ftype, uint8_t meta, uint16_t total, uint16_t crc16); // return the status error
fs_status_t fs_up_chunk(uint8_t seq, const uint8_t *data, uint8_t n); // return the status error
fs_status_t fs_up_end(fs_entry_t *out_e);// return the status error

/* עזר: יעד הכתיבה (כתובת בבתים) כדי שה-API יעדכן file_alloc_ptr */
uint16_t fs_up_dst(void);

/* הוספת רשומה לטבלה (קיימת/פשוטה) */
bool fs_add_entry(const fs_entry_t *e);
void fs_set_alloc_ptr(uint16_t p);
void fs_up_reset(void);



// קריאה של 10 בתים ראשונים בכל קובץ
extern uint8_t fs_count(void);
fs_entry_t* fs_get_entry(uint8_t idx);
extern void    ui_show_first10_next(void);
extern void read_text_from_file(void);

/* === Public helpers for scripts / higher layers === */
const fs_entry_t* fs_find_by_id(uint8_t name_id);

/* בסיס הנתונים של קובץ בפלאש (כתובת בבתים) */
uint16_t fs_e_base(const fs_entry_t *e);

extern void ldr_calib_commit(const uint16_t* L1, const uint16_t* L2, uint8_t count);
extern void ReadSampleLDR(uint16_t* out1, uint16_t* out2, uint8_t idx);

#endif /* FS_H_ */
