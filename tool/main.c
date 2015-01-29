/*
Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
PROPRIETARY/CONFIDENTIAL
This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You agree and acknowledge that
this software is owned by Samsung and you
shall not disclose such Confidential Information and shall
use it only in accordance with the terms of the license agreement
you entered into with SAMSUNG ELECTRONICS.  SAMSUNG make no
representations or warranties about the suitability
of the software, either express or implied, including but not
limited to the implied warranties of merchantability, fitness for
a particular purpose, or non-infringement.
SAMSUNG shall not be liable for any damages suffered by licensee arising out of or
related to this software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <app_manager.h>



static void _print_help(const char *cmd)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "[Get appinfo value]\n");
	fprintf(stderr, "    %s <app_id>\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "Ex) %s com.samsung.memo\n", cmd);
	fprintf(stderr, "\n");
}



static int _get_appinfo(const char *app_id)
{
	app_info_h app_info;
	int ret;
	char *appid = NULL;
	char *label = NULL;
	char *icon = NULL;
	char *package = NULL;
	char *type = NULL;

	ret = app_manager_get_app_info(app_id, &app_info);
	if (ret != APP_MANAGER_ERROR_NONE) {
		fprintf(stderr, "No corresponding app_id for [%s]\n", app_id);
		return APP_MANAGER_ERROR_NO_SUCH_APP;
	}

	app_info_get_app_id(app_info, &appid);
	app_info_get_label(app_info, &label);
	app_info_get_icon(app_info, &icon);
	app_info_get_package(app_info, &package);
	app_info_get_type(app_info, &type);

	fprintf(stderr, "appid   = [%s]\n", appid);
	fprintf(stderr, "label   = [%s]\n", label);
	fprintf(stderr, "icon    = [%s]\n", icon);
	fprintf(stderr, "package = [%s]\n", package);
	fprintf(stderr, "type    = [%s]\n", type);

	ret = app_info_destroy(app_info);
	if (ret != APP_MANAGER_ERROR_NONE) {
		return APP_MANAGER_ERROR_IO_ERROR;
	}

	free(appid);
	free(label);
	free(icon);
	free(package);
	free(type);

	return APP_MANAGER_ERROR_NONE;
}

int main(int argc, char** argv)
{
	int ret = APP_MANAGER_ERROR_NONE;

	if (2 == argc) {
		ret = _get_appinfo(argv[1]);
	}
	else {
		_print_help(argv[0]);
		return EXIT_FAILURE;
	}

	if (ret != APP_MANAGER_ERROR_NONE) {
		fprintf(stderr, "There are some problems\n");
	}

	return EXIT_SUCCESS;
}


