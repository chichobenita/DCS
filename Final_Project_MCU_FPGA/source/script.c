#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../header/fs.h"       /* fs_open, fs_read, fs_close, fs_entry_t */
#include "../header/script.h"

/* נשתמש בפונקציית ה-API שלך כדי להריץ את הפקודות
   (שימי לב: החתימה אצלך היא api_handle_msg(const char* msg, uint8_t len_r)) */
void api_handle_msg(const char* msg, uint8_t len_r);

/* פרמטרים */
#define SCR_BUF_MAX   8   /* גודל בלוק קריאה מה-FS */
#define TOK_BUF_MAX   7   /* טוקן בודד (פקודה + ארגומנטים). מספיק לפקודות קצרות */
#define DELIM         0x2C /* ',' מפריד בין פקודות */

/*  הקשר פנימי של הרצת סקריפט */
//typedef struct {
//    int      h;                         /* ידית קובץ מ-fs_open */
//    uint8_t  running;                   /* 1=ריצה פעילה */
//    uint8_t  buf[SCR_BUF_MAX];          /* באפר קריאה מה-Flash */
//    uint8_t  buf_len, buf_pos;
//
//    uint8_t  tok[TOK_BUF_MAX];          /* באפר טוקן מופרד בפסיק */
//    uint8_t  tok_len;
//} script_ctx_t;
//
//static script_ctx_t ctx = {0};
typedef struct {
    uint8_t  running;

    /* מצביע סקריפט פרטי (ללא hnd גלובלי) */
    uint16_t base;   // כתובת בסיס בפלאש
    uint16_t size;   // גודל כולל
    uint16_t pos;    // כמה קראנו עד כה
    uint16_t left;   // כמה נשאר לקרוא

    /* באפרים קיימים שלך */
    uint8_t  buf[SCR_BUF_MAX];
    uint8_t  buf_len, buf_pos;
    uint8_t  tok[TOK_BUF_MAX];
    uint8_t  tok_len;
} script_ctx_t;

static script_ctx_t ctx;


/* עזר: בדיקה אם כל התווים בטוקן נראים ASCII-HEX והאורך זוגי */
static uint8_t is_hex_digit(uint8_t c)
{
    return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f');
}
static int looks_ascii_hex_even(const uint8_t *p, uint8_t n)
{
    uint8_t i;
    if (n<2 || (n & 1)) return 0;
    for (i=0;i<n;i++) if (!is_hex_digit(p[i])) return 0;
    return 1;
}
static uint8_t hex_val(uint8_t c)
{
    if (c>='0' && c<='9') return (uint8_t)(c - '0');
    if (c>='A' && c<='F') return (uint8_t)(c - 'A' + 10);
    if (c>='a' && c<='f') return (uint8_t)(c - 'a' + 10);
    return 0;
}

/* ממיר טוקן ל־bytes.
   אם הטוקן ASCII-HEX זוגי  ממיר זוגות תווים לבייטים.
   אחרת  משתמש בביטים כמות שהם.
   מחזיר אורך היציאה או 0 אם לא יציב. */
static uint8_t token_to_bytes(const uint8_t *in, uint8_t in_n,
                              uint8_t *out, uint8_t out_cap)
{
    uint8_t i, o;

    if (in_n == 0) return 0;

    if (looks_ascii_hex_even(in, in_n)) {
        /* ASCII-HEX: כל 2 תווים = בייט אחד */
        uint8_t need = (uint8_t)(in_n >> 1);
        if (need > out_cap) return 0;
        o = 0;
        for (i=0; i<in_n; i+=2) {
            uint8_t b = (uint8_t)((hex_val(in[i]) << 4) | hex_val(in[i+1]));
            out[o++] = b;
        }
        return o;
    }

    /* בינארי גולמי */
    if (in_n > out_cap) return 0;
    memcpy(out, in, in_n);
    return in_n;
}

/* מריץ פקודה אחת (הטוקן שכבר הומר לבייטים): בונה מסר קטן
   בפורמט שה-api_handle_msg שלך מצפה לו: [cmd][len][payload…] */
static void dispatch_cmd_bytes(const uint8_t *b, uint8_t n)
{
    /* דרישה מינימלית: לפחות בייט ראשון כ־cmd */
    if (n == 0) return;

    /* בונים מסר מסוג cmd+len+payload (ללא CRC, כי אנחנו קוראים ישירות, לא מה-UART) */
    uint8_t msg[2 + TOK_BUF_MAX];
    uint8_t pay_n = (uint8_t)((n>1) ? (n-1) : 0);

    msg[1] = b[0];     /* cmd */
    msg[2] = pay_n;    /* len = גודל ה-payload */
    if (pay_n) memcpy(&msg[3], &b[1], pay_n);

    /* קריאה ישירה ל-API שלך (ה-switch-case הקיים ירוץ כרגיל) */
    api_handle_msg((const char*)msg, (uint8_t)(2 + pay_n));
}

/*  API ציבורי */

void script_init(void)
{
    memset(&ctx, 0, sizeof(ctx));
}

//bool script_start(uint8_t name_id)
//{
//    memset(&ctx, 0, sizeof(ctx));
//    ctx.h = fs_open(name_id);
//    if (ctx.h < 0) return false;
//
//    ctx.running = 1;
//
//    /* דילוג על 16 הבייט הראשונים (Header שם קובץ) */
//    uint8_t trash[SCRIPT_HEADER_SKIP];
//    uint16_t left = SCRIPT_HEADER_SKIP;
//    while (left) {
//        uint16_t n = (left > sizeof(trash)) ? sizeof(trash) : left;
//        int r = fs_read(ctx.h, trash, n);
//        if (r <= 0) {           /* קובץ קצר מדי / שגיאת קריאה */
//            script_abort();
//            return false;
//        }
//        left -= (uint16_t)r;
//    }
//    return true;
//}

bool script_start(uint8_t name_id)
{
    const fs_entry_t* e = fs_find_by_id(name_id);
    if (!e) return false;

    uint16_t base = fs_e_base(e);
    uint16_t size = fs_e_size(e);

    /* אם גודל הקובץ כולל 16B שם – דלג עליהם */
    if (size >= 16) {
        base += 16;
        size -= 16;
    } else {
        /* קובץ קצר מדי – אין Payload להריץ */
        return false;
    }

    ctx.base = base;
    ctx.size = size;
    ctx.pos  = 0;
    ctx.left = size;

    ctx.buf_len = 0;
    ctx.buf_pos = 0;
    ctx.tok_len = 0;
    ctx.running = 1;
    return true;
}


bool script_running(void)
{
    return ctx.running ? true : false;
}

//void script_abort(void)
//{
//    if (ctx.running) {
//        fs_close(ctx.h);
//        memset(&ctx, 0, sizeof(ctx));
//    }
//}

///* צעד יחיד: קורא מה-FS לפי הצורך, אוסף טוקן עד פסיק,
//   ממיר לבייטים ומריץ פקודה אחת דרך api_handle_msg. */

//bool script_step(void)
//{
//    if (!ctx.running) return false;
//
//    // נממש פקודה אחת לכל step לכל היותר
//    uint8_t did = 0;
//
//    for (;;)
//    {
//        // צריך עוד נתונים?
//        if (ctx.buf_pos >= ctx.buf_len) {
//            int n = fs_read(ctx.h, ctx.buf, SCR_BUF_MAX);   // 8B
//            if (n < 0) { script_abort(); return false; }
//            if (n == 0) {
//                // EOF: אם נשאר טוקן אחרון בלי פסיק – המרץ והרץ
//                if (ctx.tok_len > 0) {
//                    uint8_t b[TOK_BUF_MAX];
//                    uint8_t bn = token_to_bytes(ctx.tok, ctx.tok_len, b, sizeof(b));
//                    ctx.tok_len = 0;
//                    if (bn) { dispatch_cmd_bytes(b, bn); did = 1; }
//                }
//                fs_close(ctx.h);
//                memset(&ctx, 0, sizeof(ctx));
//                return did ? true : false;
//            }
//            ctx.buf_len = (uint8_t)n;
//            ctx.buf_pos = 0;
//        }
//
//        // עיבוד בייט אחד
//        uint8_t c = ctx.buf[ctx.buf_pos++];
//
//        if (c == DELIM) {                 // ',' = 0x2C
//            if (ctx.tok_len > 0) {
//                uint8_t b[TOK_BUF_MAX];
//                uint8_t bn = token_to_bytes(ctx.tok, ctx.tok_len, b, sizeof(b));
//                ctx.tok_len = 0;
//                if (bn) { dispatch_cmd_bytes(b, bn); did = 1; }
//            }
//            // מריצים לכל היותר פקודה אחת ב-step
//            return did ? true : false;
//        } else {
//            if (ctx.tok_len < TOK_BUF_MAX)
//                ctx.tok[ctx.tok_len++] = c;
//            else
//                ctx.tok_len = 0; // טוקן חריג באורך – ננקה ונמשיך
//        }
//    }
//}

bool script_step(void)
{
    uint8_t did = 0;
    if (!ctx.running) return false;

    /* refill: אם נגמר – קרא ישירות מהפלאש לפי base+pos */
    if (ctx.buf_pos >= ctx.buf_len) {
        if (ctx.left == 0) {
            /* EOF – טוקן אחרון ללא פסיק? */
            if (ctx.tok_len > 0) {
                uint8_t b[TOK_BUF_MAX];
                uint8_t bn = token_to_bytes(ctx.tok, ctx.tok_len, b, sizeof(b));
                if (bn) dispatch_cmd_bytes(b, bn);
            }
            memset(&ctx, 0, sizeof(ctx));
            return (ctx.tok_len > 0);
        }

        uint8_t n = (ctx.left > SCR_BUF_MAX) ? SCR_BUF_MAX : (uint8_t)ctx.left;
        /* קריאה ישירה מהפלאש: memcpy מהכתובת המוחלטת */
        memcpy(ctx.buf, (const void *)(uintptr_t)(ctx.base + ctx.pos), n);
        ctx.buf_len = n;
        ctx.buf_pos = 0;
        ctx.pos    += n;
        ctx.left   -= n;
    }

    /* אסוף עד פסיק/סוף באפר והרץ פקודה אחת לכל step */
    while (ctx.buf_pos < ctx.buf_len) {
        uint8_t c = ctx.buf[ctx.buf_pos++];

        if (c == DELIM) {
            if (ctx.tok_len > 0) {
                uint8_t b[TOK_BUF_MAX];
                uint8_t bn = token_to_bytes(ctx.tok, ctx.tok_len, b, sizeof(b));
                ctx.tok_len = 0;
                if (bn) {
                    dispatch_cmd_bytes(b, bn);
                    did = 1;
                    break; /* פקודה אחת ב-step */
                }
            }
        } else {
            if (ctx.tok_len < TOK_BUF_MAX) ctx.tok[ctx.tok_len++] = c;
            else ctx.tok_len = 0; /* overflow token  נקה */
        }
    }

    return did ? true : false;
}

