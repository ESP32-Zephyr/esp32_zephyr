/******************************** INCLUDE FILES *******************************/
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

#include <zephyr/logging/log.h>

#include "nvs.h"
#include <string.h>
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(nvs, LOG_LEVEL_DBG);

#define NVS_PARTITION		    storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define NVS_WIFI_SSID_ID      1
#define NVS_WIFI_PASS_ID      2
/********************************* TYPEDEFS ***********************************/

/********************************* LOCAL DATA *********************************/
static struct nvs_fs fs;
static nvs_t nvs;

/****************************** LOCAL FUNCTIONS ******************************/
static bool nvs_init() {
    int rc = 0;
	struct flash_pages_info info;

	/* define the nvs file system by settings with:
	 *	sector_size equal to the pagesize,
	 *	1 sector starting at NVS_PARTITION_OFFSET
	 */
	fs.flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs.flash_device)) {
		LOG_ERR("Flash device %s is not ready\n", fs.flash_device->name);
		return false;
	}
	fs.offset = NVS_PARTITION_OFFSET;
	rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc) {
		LOG_ERR("Unable to get page info, rc=%d\n", rc);
		return false;
	}
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	rc = nvs_mount(&fs);
	if (rc) {
		LOG_ERR("Flash Init failed, rc=%d\n", rc);
		return false;
	}

    return true;
}

static int nvs_write_str(int id, const char *data, size_t len) {
    int rc = nvs_write(&fs, id, data, len);
    if (rc < 0) {
        LOG_ERR("NVS write failed, rc=%d\n", rc);
    }

    return rc;
}

static int nvs_read_str(int id, char *data, size_t len) {
    int rc = nvs_read(&fs, id, data, len);
    if (rc < 0) {
        LOG_ERR("NVS read failed, rc=%d\n", rc);
    }

    return rc;
}

static int nvs_erase(int id) {
    int rc = nvs_delete(&fs, id);
    if (rc < 0) {
        LOG_ERR("NVS delete failed, rc=%d\n", rc);
    }

    return rc;
}

static bool nvs_store_credentials(const char *ssid, size_t ssid_len,
    const char *pass, size_t pass_len) {

    int rc = nvs_write_str(NVS_WIFI_SSID_ID, ssid, ssid_len);
    if (rc < 0) {
        return false;
    }

    rc = nvs_write_str(NVS_WIFI_PASS_ID, pass, pass_len);
    if (rc < 0) {
        return false;
    }

    return true;
}

static bool nvs_read_credentials(char *ssid, size_t ssid_len,
    char *pass, size_t pass_len) {

    int rc = nvs_read_str(NVS_WIFI_SSID_ID, ssid, ssid_len);
    if (rc < 0) {
        return false;
    }

    rc = nvs_read_str(NVS_WIFI_PASS_ID, pass, pass_len);
    if (rc < 0) {
        return false;
    }

    return true;
}

static bool nvs_erase_credentials(void) {
    int rc = nvs_erase(NVS_WIFI_SSID_ID);
    if (rc < 0) {
        return false;
    }

    rc = nvs_erase(NVS_WIFI_PASS_ID);
    if (rc < 0) {
        return false;
    }

    return true;
}

/* NVS shell command */
static int cmd_nvs(const struct shell *sh, size_t argc, char **argv) {
    const char *ssid = NULL;
    const char *pass = NULL;
    bool erase = false;
    int i;

    if (argc <= 1) {
        /* No args - read and print stored credentials */
        char stored_ssid[64] = {0};
        char stored_pass[64] = {0};
        if (!nvs_read_credentials(stored_ssid, sizeof(stored_ssid), stored_pass,
            sizeof(stored_pass))) {
            shell_error(sh, "No credentials stored");
            return 0;
        }
        shell_print(sh, "Stored SSID: %s", stored_ssid);
        shell_print(sh, "Stored PASS: %s", stored_pass);
        return 0;
    }

    /* Parse flags: --ssid <value>, --pass <value>, and --erase/--delete */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ssid") == 0) {
            if (i + 1 < argc) {
                ssid = argv[++i];
            } else {
                shell_error(sh, "Missing value for --ssid");
                return -EINVAL;
            }
        } else if (strcmp(argv[i], "--pass") == 0) {
            if (i + 1 < argc) {
                pass = argv[++i];
            } else {
                shell_error(sh, "Missing value for --pass");
                return -EINVAL;
            }
        } else if ((strcmp(argv[i], "--erase") == 0) || (strcmp(argv[i], "--delete") == 0)) {
            erase = true;
        } else {
            shell_error(sh, "Unknown argument: %s", argv[i]);
            return -EINVAL;
        }
    }

    if (erase) {
        if (ssid || pass) {
            shell_error(sh, "Cannot use --erase with --ssid or --pass");
            return -EINVAL;
        }
        if (nvs_erase_credentials()) {
            shell_print(sh, "Credentials erased");
            return 0;
        } else {
            shell_error(sh, "Failed to erase credentials");
            return -EIO;
        }
    }

    if (ssid && pass) {
        if (nvs_store_credentials(ssid, strlen(ssid) + 1, pass, strlen(pass) + 1)) {
            shell_print(sh, "Credentials stored");
            return 0;
        } else {
            shell_error(sh, "Failed to store credentials");
            return -EIO;
        }
    }

    shell_error(sh, "Both --ssid and --pass must be provided to store credentials");
    return -EINVAL;
}

SHELL_CMD_ARG_REGISTER(nvs, NULL, "NVS commands: nvs [--ssid <ssid> --pass <pass>] | nvs --erase | nvs (to read)", cmd_nvs, 1, 5);
/***************************** INTERFACE FUNCTIONS ****************************/
nvs_t *nvs_get(void) {
    nvs.init = nvs_init;

    nvs.store_credentials = nvs_store_credentials;
    nvs.read_credentials = nvs_read_credentials;
    nvs.erase_credentials = nvs_erase_credentials;
    return &nvs;
}
