/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// For error handling
void die(char *s) {
	perror(s);
	exit(1);
}

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("disableRawMode: tcsetattr");
}

void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("enableRawMode: tsgetattr");
	atexit(disableRawMode);

	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);

	// IXON -> bitflag to disable/enable control charaters (Ctrl + S, Ctrl + Q)
	// ICRNL -> bitflag to disble/enable tranlation of carriage return ('\r') to newline ('\n')
	raw.c_iflag &= ~(IXON | ICRNL);

	// OPOST -> bitflag to disable/enable output processing
	raw.c_oflag &= ~(OPOST);

	// ECHO -> bitflag to disable/enable echoing of the input
	// ICANON -> bitflag to disable/enable  canonical mode
	// ISIG -> bitflag to disable/enable signals (Ctrl + C, Ctrl + Z, etc)
	// IEXTEN -> bitflag to disable/enable extended functions (Ctrl + V)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

	// c_cc -> control characters (an array of characters which contorl various terminal settings)
	// VMIN -> Minimum no. of bytes needed before read() can return
	// VTIME -> Maximum time before read() can return
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("enableRawMode: tcsetattr");
}

/*** init ***/

int main() {
	enableRawMode();
	char c;
	while (1) {
		c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("main: read");
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d (%c)\r\n", c, c);
		}
		if (c == 'q') break;
	}
	return 0;
}
