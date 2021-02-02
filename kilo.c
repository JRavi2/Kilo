#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disableRawMode);

	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);

	// IXON -> bitflag to disable/enable control charaters (Ctrl + S, Ctrl + Q)
	// ICRNL -> bitflag to disble/enable tranlation of carriage return ('\r') to newline ('\n')
	raw.c_iflag &= ~(IXON | ICRNL);

	// ECHO -> bitflag to disable/enable echoing of the input
	// ICANON -> bitflag to disable/enable  canonical mode
	// ISIG -> bitflag to disable/enable signals (Ctrl + C, Ctrl + Z, etc)
	// IEXTEN -> bitflag to disable/enable extended functions (Ctrl + V)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();
	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		if (iscntrl(c)) {
			printf("%d\n", c);
		} else {
			printf("%d (%c)\n", c, c);
		}
	}
	return 0;
}
