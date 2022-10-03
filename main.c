#include <vitasdk.h>
#include <taihen.h>
#include <libk/stdio.h>
#include <libk/string.h>

#define DEBUG_FILE "MLCL00001"

static char titleid[10];

int launchAppByTitleid(const char *titleid) {
	char uri[32];
	sprintf(uri, "psgm:play?titleid=%s", titleid);

	while (sceAppMgrLaunchAppByUri(0xFFFFF, uri) != 0)
        sceKernelDelayThread(100000);

	return 0;
}

int autoboot_thread(SceSize args, void *argp) {
    // NPXS10079 is launched alongside livearea so we can check for it for autobooting any app
    SceInt32 appId = 0;
    char appName[10];
    SceUID pid = 0;

    do {
        sceAppMgrGetRunningAppIdListForShell(&appId, 1);

        if (appId != 0) {
            pid = sceAppMgrGetProcessIdByAppIdForShell(appId);

            if(pid > 0) {
                sceAppMgrGetNameById(pid, appName);

                if (strcmp(appName, "NPXS10079") == 0)
                    break;
            }
        }

        sceKernelDelayThread(100000);
    } while(1);

    SceUID fd = sceIoOpen("ux0:/data/AutoBoot/boot.cfg", SCE_O_RDONLY, 0777);

    if (fd >= 0) {
        sceIoRead(fd, titleid, 9);
        sceIoClose(fd);
        titleid[9] = 0;
        launchAppByTitleid(titleid);
    }
	
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	// Starting secondary thread to delay boot until enso is setup
	SceUID thid = sceKernelCreateThread("autoboot_thread", autoboot_thread, 0x40, 0x100000, 0, 0, NULL);
	if (thid >= 0) sceKernelStartThread(thid, 0, NULL);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	return SCE_KERNEL_STOP_SUCCESS;
}
