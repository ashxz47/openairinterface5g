#ifndef _CONF_USER_DATA_H
#define _CONF_USER_DATA_H

#include <stdbool.h>
#include <libconfig.h>
#include "userDef.h"
#include "commonDef.h"

#define USER "USER"
#define MANUFACTURER "MANUFACTURER"
#define MODEL "MODEL"
#define UE_IMEI "IMEI"
#define PINCODE "PIN"

//NSSAI
#define NSSAI "NSSAI"
#define SNSSAI "SNSSAI"
#define SST "SST"
#define SD "SD"


typedef struct {
	const char* sst;
	const char* sd;
} snssai_t;

typedef struct {
    int size;
    snssai_t items[8];
} all_ue_snssai_t; //all snssai per UE


typedef struct {
	const char* imei;
	const char* manufacturer;
	const char* model;
	const char* pin;
	all_ue_snssai_t ue_nssai;
} user_data_conf_t;





void gen_user_data(user_data_conf_t *u, user_nvdata_t *user_data);
bool write_user_data(const char *directory, int user_id, user_nvdata_t *data);
int parse_ue_user_data(config_setting_t *ue_setting, int user_id, user_data_conf_t *u);
int parse_nssai(config_setting_t *all_nssai_setting, int user_id, user_data_conf_t *snssai);

int _luhn(const char* cc);

#endif
