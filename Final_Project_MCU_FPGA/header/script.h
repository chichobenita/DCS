#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <stdint.h>
#include <stdbool.h>

/* ����� ����� (���� �� ����, ����� �����) */
void script_init(void);

/* ����� ���� ������ ��� name_id. ����� true �� ���� ����� ������ */
bool script_start(uint8_t name_id);

/* ��� ��� � ���� �� ����� ��� ����� ����.
   ����� true �� ����� ����� (�����/����� ����), false �� ��� �� ����� ����. */
bool script_step(void);

/* ��� �� ������ ��� ���� */
bool script_running(void);

/* ����� ����� (����� ���� ������ ���) */
void script_abort(void);


#define SCRIPT_HEADER_SKIP  16

#endif
