#include <vitasdk.h>
#include <taihen.h>
#include <libk/stdio.h>
#include <libk/string.h>
#include <kuio.h>

#define DEBUG_FILE "MLCL00001"

static char titleid[10];

int launchAppByTitleid(const char *titleid) {
	char uri[32];
	sprintf(uri, "psgm:play?titleid=%s", titleid);
	
	int i;
	for (i=0;i<400;i++){
		sceKernelDelayThread(10000);
		sceAppMgrLaunchAppByUri(0xFFFFF, uri);
	}

	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	SceUID fd;
	kuIoOpen("ux0:/data/AutoBoot/boot.cfg", SCE_O_RDONLY, &fd);
	if (fd >= 0){
		kuIoRead(fd, titleid, 9);
		kuIoClose(fd);
		titleid[9] = 0;
		launchAppByTitleid(titleid);
	}
	
	return SCE_KERNEL_START_SUCCESS;

}

int module_stop(SceSize argc, const void *args) {
	return SCE_KERNEL_STOP_SUCCESS;
}