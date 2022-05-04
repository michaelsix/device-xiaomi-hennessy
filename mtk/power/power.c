/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_TAG "MTKPowerHAL"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

static void power_init(struct power_module *module)
{
	if (module)
		ALOGI("%s", __func__);
}

static void power_set_interactive(struct power_module *module __unused, int on __unused) {}

int sysfs_write(char* path, char* s) {
	char buf[80];
	int len;
	int ret = 0;
	int fd = open(path, O_WRONLY);

	if (fd < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error opening %s: %s\n", path, buf);
		return -1;
	}

	len = write(fd, s, strlen(s));
	if (len < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error writing to %s: %s\n", path, buf);
		ret = -1;
	}
	close(fd);

	return ret;
}

static void power_set_feature(struct power_module *module __unused, feature_t feature, int state)
{
#ifdef TAP_TO_WAKE_NODE
	if (feature == POWER_FEATURE_DOUBLE_TAP_TO_WAKE)
		sysfs_write(TAP_TO_WAKE_NODE, state ? "1" : "0");
#endif
}

static void power_hint(struct power_module *module __unused, power_hint_t hint __unused, void *data __unused) {}

static int power_open(const hw_module_t* module, const char* name, hw_device_t** device) {
	if (module)
		ALOGI("%s: enter; name=%s", __FUNCTION__, name);

	int retval = 0; /* 0 is ok; -1 is error */

	if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
		power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

		if (dev) {
			/* Common hw_device_t fields */
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.module_api_version = POWER_MODULE_API_VERSION_0_3;
			dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

			dev->init = power_init;
			dev->powerHint = power_hint;
			dev->setFeature = power_set_feature;
			dev->setInteractive = power_set_interactive;

			*device = (hw_device_t*)dev;
		} else
			retval = -ENOMEM;
	} else
		retval = -EINVAL;

	ALOGD("%s: exit %d", __FUNCTION__, retval);
	return retval;
}

static struct hw_module_methods_t power_module_methods = {
	.open = power_open
};

struct power_module HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.module_api_version = POWER_MODULE_API_VERSION_0_3,
		.hal_api_version = HARDWARE_HAL_API_VERSION,
		.id = POWER_HARDWARE_MODULE_ID,
		.name = "Mediatek Power HAL",
		.author = "The Android Open Source Project",
		.methods = &power_module_methods
	},

	.init = power_init,
	.setInteractive = power_set_interactive,
	.setFeature = power_set_feature,
	.powerHint = power_hint
};
