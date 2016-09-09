/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <stdio.h>
#include <string.h>
#include <compiler.h>

#include "command.h"

#define HIST_SIZE 5
#define LINE_LEN 64
static char history[HIST_SIZE][LINE_LEN+1]; /* circ buf of nul terminated strings should be more effective */
static int hist_len = 0;
static int hist_pos = -1;

static char line[LINE_LEN+1];
static int pos;
static int len;

enum vt100_key {
	K_CTRL_C = 3,
	K_CTRL_H = 8,
	K_BACKSPACE = 8,
	K_CTRL_U = '\x15',
	//K_ENTER = '\r',
	K_ENTER = '\n',
	K_ESCAPE = '\x1b',
	K_BACKSPACE2 = '\x7f',

	/* below are our made up values */
	/* vt100 start */
	K_VT100_START = 0x100,
	K_UP,
	K_DOWN,
	K_RIGHT,
	K_LEFT,
	K_END,
	K_HOME,
	K_INSERT,
	K_DELETE,
	K_PGUP,
	K_PGDN,

	/* internal marks */
	K_PARTIAL = 0xfff0,
	K_INVALID,
};

struct vt100_seq {
	enum vt100_key key;
	const char *seq;
};

static const struct vt100_seq vt100_seqs[] = {
	{ .key = K_UP,     .seq = "[A" },
	{ .key = K_DOWN,   .seq = "[B" },
	{ .key = K_RIGHT,  .seq = "[C" },
	{ .key = K_LEFT,   .seq = "[D" },
	{ .key = K_END,    .seq = "[F" },
	{ .key = K_HOME,   .seq = "[H" },
	{ .key = K_INSERT, .seq = "[2~" },
	{ .key = K_DELETE, .seq = "[3~" },
	{ .key = K_PGUP,   .seq = "[5~" },
	{ .key = K_PGDN,   .seq = "[6~" },
};


static int vt100_cmp(const char *seq, int len, const char *ntseq)
{
	int i;
	for (i=0; i<len; i++)
		if (seq[i] != ntseq[i])
			return -1;

	/* full match */
	if (ntseq[i] == '\0')
		return 0;

	/* matched, but the sequence wasn't complete */
	return 1;
}

#if 0
static const char *vt100_seq(enum vt100_key key)
{
	int i;
	for (i=0; i<ALEN(vt100_seqs); i++) {
		if (vt100_seqs[i].key == key)
			return vt100_seqs[i].seq;
	}
	return NULL;
}
#endif

static enum vt100_key vt100_match(const char *seq, int len)
{
	int i;

	for (i=0; i<ALEN(vt100_seqs); i++) {
		int m;
		m = vt100_cmp(seq, len, vt100_seqs[i].seq);
		if (m == 0)
			return vt100_seqs[i].key;
		else if (m == 1)
			return K_PARTIAL;
	}
	return K_INVALID;
}


static void cbashell_command(const char *cmd)
{
	int i;

	for (i=0; command_list[i]; i++)
		if (command_list[i](cmd) != 0)
			break;
}


/*
 * readline like prompt handling
 * TODO history should be improved with a circ buf for string storage or sth,
 * to be more effective.
 */
static int cbashell_char(int c)
{
	if (hist_pos >= 0 && !(c == K_UP || c == K_DOWN)) {
		strcpy(line, history[hist_pos]);
		hist_pos = -1;
	}

	if (c >= ' ' && c < '\x7f') {
		/* guard against buffer overflow */
		if (len >= LINE_LEN)
			return 0;

		memmove(&line[pos+1], &line[pos], len-pos);
		line[pos++] = c;
		len++;

		line[len] = '\0';
		if (pos < len) {
			/* string (overwrites), move back x left */
			printf("%s\x1b[%iD", &line[pos-1], len-pos);
		} else {
			printf("%c", c);
		}
	} else
	if (c == K_ENTER) {
		if (len == 0) {
			strcpy(line, history[0]);
			goto repeat_last;
		}

		int i;
		if (hist_len >= HIST_SIZE)
			hist_len = HIST_SIZE-1;

		for (i=hist_len-1; i>=0; i--)
			strcpy(history[i+1], history[i]);
		strcpy(history[0], line);
		hist_len++;

 repeat_last:
		printf("\n");
		cbashell_command(line);

		printf("> ");

		len = pos = 0;
		line[len] = '\0';
	} else
	if (c == K_BACKSPACE || c == K_BACKSPACE2) {
		if (pos > 0) {
			memmove(&line[pos-1], &line[pos], len-pos);
			pos--;
			len--;

			if (pos < len) {
				line[len] = '\0';
				/* move left, clear line right, string, move x left */
				printf("\x1b[D\x1b[K%s\x1b[%iD", &line[pos], len-pos);
			} else {
				printf("\x1b[D\x1b[K");
			}
		}
	} else
	if (c == K_DELETE) {
		if (pos < len) {
			memmove(&line[pos], &line[pos+1], len-pos-1);
			len--;

			line[len] = '\0';
			if (pos < len) {
				/* clear line right, string, move x left */
				printf("\x1b[K%s\x1b[%iD", &line[pos], len-pos);
			} else
				printf("\x1b[K");
		}
	} else
	if (c == K_CTRL_C) {
		return -1;
	} else
	if (c == K_LEFT) {
		if (pos > 0) {
			pos--;
			printf("\x1b[D"); /* left */
		}
	} else
	if (c == K_RIGHT) {
		if (pos < len) {
			pos++;
			printf("\x1b[C"); /* right */
		}
	} else
	if (c == K_HOME) {
		if (pos > 0) {
			printf("\x1b[%iD", pos); /* left x */
			pos = 0;
		}
	} else
	if (c == K_END) {
		if (pos < len) {
			printf("\x1b[%iC", len-pos); /* right x */
			pos = len;
		}
	} else
	if (c == K_UP) {
		if (hist_pos < hist_len-1) {
			hist_pos++;
			if (pos > 0)
				printf("\x1b[%iD", pos); /* left x */
			printf("\x1b[K%s", history[hist_pos]); /* clear line right, print history */
			len = pos = strlen(history[hist_pos]);
		}
	} else
	if (c == K_DOWN) {
		if (hist_pos >= 0) {
			const char *newline = line;
			hist_pos--;
			if (hist_pos >= 0)
				newline = history[hist_pos];
			if (pos > 0)
				printf("\x1b[%iD", pos); /* left x */
			printf("\x1b[K%s", newline); /* clear line right, print history */
			len = pos = strlen(newline);
		}
	} else
	if (c == K_CTRL_U) {
		if (pos > 0)
			printf("\x1b[%iD", pos); /* left x */
		printf("\x1b[K"); /* clear line right */
		len = pos = 0;
		line[len] = '\0';
	} else
	if (c >= K_VT100_START) {
		/* debugging print */
		printf("vt100:%i ", c);
	}
	else {
		printf("%02x ", c);
	}
	return 0;
}

/* partial VT100 parsing, see
 * http://ascii-table.com/ansi-escape-sequences-vt-100.php
 */
int cbashell_charraw(int c)
{
	static int inside_esc;
	static char esc[8];
	static int esc_pos;

	/* VT100 stuff */
	if (inside_esc) {
		/* just a silly guard */
		if (esc_pos == sizeof(esc))
			esc_pos = 0;

		esc[esc_pos++] = c;
		c = vt100_match(esc, esc_pos);
		if (c == K_INVALID) {
			int i;
			int r = 0;
			r |= cbashell_char('^');
			r |= cbashell_char('[');
			for (i=0; i<esc_pos; i++)
				r |= cbashell_char(esc[i]);

			inside_esc = 0;
			return r;
		} else
		if (c == K_PARTIAL)
			return 0;

		inside_esc = 0;
		return cbashell_char(c);
	}

	if (c == K_ESCAPE) {
		inside_esc = 1;
		esc_pos = 0;
		return 0;
	}

	return cbashell_char(c);
}

void cbashell_init()
{
	printf("> ");

	len = pos = 0;
	line[len] = '\0';

	hist_len = 0;
	hist_pos = -1;
}
