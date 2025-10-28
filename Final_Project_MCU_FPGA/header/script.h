#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <stdint.h>
#include <stdbool.h>

/* אתחול פנימי (כרגע לא חובה, נשאיר לעתיד) */
void script_init(void);

/* מתחיל הרצת סקריפט לפי name_id. מחזיר true אם נמצא ונפתח בהצלחה */
bool script_start(uint8_t name_id);

/* צעד אחד – מפרק עד פקודה אחת ומריץ אותה.
   מחזיר true אם בוצעה עבודה (קראנו/הרצנו משהו), false אם אין מה לעשות כרגע. */
bool script_step(void);

/* האם יש סקריפט שרץ כרגע */
bool script_running(void);

/* ביטול מיידי (סגירת קובץ וניקוי מצב) */
void script_abort(void);


#define SCRIPT_HEADER_SKIP  16

#endif
