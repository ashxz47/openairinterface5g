#include <string.h>
#include <stdlib.h>

#include "commonDef.h"
#include "memory.h"
#include "fs.h"
#include "conf_user_data.h"

int parse_ue_user_data(config_setting_t *ue_setting, int user_id, user_data_conf_t *u) {
	config_setting_t *ue_param_setting = NULL;

	int rc = true;
	ue_param_setting = config_setting_get_member(ue_setting, USER);
	if (ue_param_setting == NULL) {
		printf("Check USER section of UE%d. EXITING...\n", user_id);
		return false;
	}
	rc = config_setting_lookup_string(ue_param_setting, UE_IMEI, &u->imei);
	if (rc != 1) {
		printf("Check USER IMEI section for UE%d. Exiting\n", user_id);
		return false;
	}
	rc = config_setting_lookup_string(ue_param_setting, MANUFACTURER,
			&u->manufacturer);
	if (rc != 1) {
		printf("Check USER MANUFACTURER for UE%d FULLNAME. Exiting\n", user_id);
		return false;
	}
	rc = config_setting_lookup_string(ue_param_setting, MODEL, &u->model);
	if (rc != 1) {
		printf("Check USER MODEL for UE%d FULLNAME. Exiting\n", user_id);
		return false;
	}
	rc = config_setting_lookup_string(ue_param_setting, PINCODE, &u->pin);
	if (rc != 1) {
		printf("Check USER PIN for UE%d FULLNAME. Exiting\n", user_id);
		return false;
	}
	return true;
}

int parse_nssai(config_setting_t *all_nssai_setting, int user_id, user_data_conf_t *ue_nssai)
{
	config_setting_t *nssai_setting = NULL;
	all_ue_snssai_t *nssai = &ue_nssai->ue_nssai;
	char snssai[10];
	int snssai_nb = 0;

	snssai_nb = config_setting_length(all_nssai_setting);

	nssai->size = snssai_nb;
	for (int i = 0; i < snssai_nb; i++)
	{
		// nssai->items[i] = malloc(sizeof(snssai_t) * snssai_nb);
		memset(&nssai->items[i], 0, sizeof(snssai_t));
	}

	for (int i = 0; i < snssai_nb; i++)
	{
		snssai_t *ue_snssai = &nssai->items[i];
		sprintf(snssai, "%s%d", SNSSAI, i);

		nssai_setting = config_setting_get_member(all_nssai_setting, snssai);
		if (nssai_setting == NULL)
		{
			printf("Check SNSSAI section of UE%d. EXITING...\n", user_id);
			return false;
		}

		int rc = 0;
		rc = config_setting_lookup_string(nssai_setting, SST, &ue_snssai->sst);
		if (rc != 1)
		{
			printf("Error on SST\n");
			return false;
		}
		rc = config_setting_lookup_string(nssai_setting, SD, &ue_snssai->sd);
		if (rc != 1)
		{
			printf("Error on SD\n");
			return false;
		}
	}
	for (int i = (0 + snssai_nb); i < 8; i++)
	{
		nssai->items[i].sst = "-1";
		nssai->items[i].sd = "-1";
	}
	return true;
}



void gen_user_data(user_data_conf_t *u, user_nvdata_t *user_data) {
	memset(user_data, 0, sizeof(user_nvdata_t));
	snprintf(user_data->IMEI, USER_IMEI_SIZE + 1, "%s%d", u->imei, _luhn(u->imei));
	/*
	 * Manufacturer identifier
	 */
	strncpy(user_data->manufacturer, u->manufacturer, USER_MANUFACTURER_SIZE);
	/*
	 * Model identifier
	 */
	strncpy(user_data->model, u->model, USER_MODEL_SIZE);
	/*
	 * SIM Personal Identification Number
	 */
	strncpy(user_data->PIN, u->pin, USER_PIN_SIZE);
	//NSSAI
	if (u->ue_nssai.size >= 0)
	{
		for (int i = 0; i < u->ue_nssai.size; i++)
		{
			strncpy(user_data->nv_nssai.items[i].sst, u->ue_nssai.items[i].sst, SST_SIZE);
			strncpy(user_data->nv_nssai.items[i].sd, u->ue_nssai.items[i].sd, SD_SIZE);
		}
	}
}

bool write_user_data(const char *directory, int user_id, user_nvdata_t *data) {
    int rc;
	char* filename = make_filename(directory, USER_NVRAM_FILENAME, user_id);
	rc = memory_write(filename, data, sizeof(user_nvdata_t));
    free(filename);
	if (rc != RETURNok) {
		perror("ERROR\t: memory_write() failed");
		return false;
	}
	return true;
}

/*
 * Computes the check digit using Luhn algorithm
 */
int _luhn(const char* cc) {
	const int m[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };
	int odd = 1, sum = 0;

	for (int i = strlen(cc); i--; odd = !odd) {
		int digit = cc[i] - '0';
		sum += odd ? m[digit] : digit;
	}

	return 10 - (sum % 10);
}


