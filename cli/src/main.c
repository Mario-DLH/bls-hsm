/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>
#include <kernel.h>
#include <stdio.h>
#include <secure_services.h>
#include <pm_config.h>
#include <fw_info.h>

#include <blst.h>
#include <common.h>


#define CONFIG_SPM_SERVICE_RNG

LOG_MODULE_REGISTER(app);

static int cmd_keygen(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
    keygen(argc, argv, NULL);
    return 0;
}

static int cmd_signature_message(const struct shell *shell, size_t argc, char **argv, char* buff)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
    signature(argc, argv, NULL);
	return 0;
}

static int cmd_signature_verification(const struct shell *shell, size_t argc, char **argv, char* buff)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
    verify(argc, argv, NULL);
	return 0;
}

static int cmd_get_keys(const struct shell *shell, size_t argc, char **argv, char* buff)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
    get_keys(argc, argv, NULL);
	return 0;
}

static int cmd_reset(const struct shell *shell, size_t argc, char **argv, char* buff){
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
    resetc(argc, argv, NULL);
    return 0;
}

static int cmd_prompt(const struct shell *shell, size_t argc, char **argv){
        ARG_UNUSED(argc);
	ARG_UNUSED(argv);

        if(strcmp(argv[1], "on") == 0){
            shell_prompt_change(shell, "uart:~$ ");
        }else if(strcmp(argv[1], "off") == 0){
            shell_prompt_change(shell, "");
        }else{
            printf("Usage: prompt on/off\n");
        }
        return 0;
}

SHELL_CMD_ARG_REGISTER(keygen, NULL, "Generates secret key and public key", cmd_keygen, 1, 1);

SHELL_CMD_ARG_REGISTER(signature, NULL, "Signs a message with a specific public key", cmd_signature_message, 3, 0);

SHELL_CMD_ARG_REGISTER(verify, NULL, "Verifies the signature", cmd_signature_verification, 4, 0);

SHELL_CMD_ARG_REGISTER(getkeys, NULL, "Returns the identifiers of the keys available to the signer", cmd_get_keys, 1, 0);

SHELL_CMD_ARG_REGISTER(reset, NULL, "Deletes all generated keys", cmd_reset, 1, 0);

SHELL_CMD_ARG_REGISTER(prompt, NULL, "Toggle prompt", cmd_prompt, 2, 0);

void main(void)
{

}