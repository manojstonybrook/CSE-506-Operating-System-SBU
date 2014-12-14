#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int rfd, wfd;
	char buf[512];
	char buf1[512];
	char buf2[512] = "manoj alwani!!!";
	
	int n, r;
	if ((rfd = open("/newmotd", O_RDONLY)) < 0)
		panic("open /newmotd: %e", rfd);
	if ((wfd = open("/motd", O_RDWR)) < 0)
		panic("open /motd: %e", wfd);
	cprintf("file descriptors %d %d\n", rfd, wfd);
	if (rfd == wfd)
		panic("open /newmotd and /motd give same file descriptor");

	cprintf("OLD MOTD\n===\n");
	while ((n = read(wfd, buf, sizeof buf-1)) > 0)
		sys_cputs(buf, n);
	cprintf("===\n");
	seek(wfd, 0);

	//if ((r = ftruncate(wfd, 0)) < 0)
	//	panic("truncate /motd: %e", r);
	
	n = strlen(buf2);
	if ((r = write(wfd, buf2, n)) != n)
	   panic("write /motd: %e", r);

	seek(wfd, 0);

	cprintf("New MOTD\n===\n");
	n = read(wfd, buf1, n);
	sys_cputs(buf1, n);
	cprintf("\n===\n");

	if (n < 0)
	   panic("read /newmotd: %e", n);

	//close(rfd);
	close(wfd);
}
