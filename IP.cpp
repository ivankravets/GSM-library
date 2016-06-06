#include "IP.h"

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const char paramValue[]) {
	char buffer[22 + strlen(paramTag) + strlen(paramValue)];  // "AT+SAPBR=X,X,\"{paramTag}\",\"{paramValue}\"\r\r\n"
	struct BearerProfile bearerProfile;

	if(cmdType <= 5 && cmdType != 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d\r");
		sprintf_P(buffer, (const char *)command, cmdType, cid);
	}
	else if(cmdType == 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d,\"%s\",\"%s\"\r");
		sprintf_P(buffer, (const char *)command, cmdType, cid, paramTag, paramValue);
	}
	else
		return false;

  const __FlashStringHelper *response = F("+SAPBR: ");

  bearerProfile = this->bearerProfile[cid-1];
	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(cmdType == 2) {
		if(!dte->ATResponseContain(response)) return false;
		char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
		char *str = strtok(pointer, ",\"");
		for (unsigned char i = 0; i < 3 && str != NULL; i++) {
			if(i == 0) bearerProfile.cid = str[0] - '0';
			if(i == 1) bearerProfile.connStatus.status = str[0] - '0';
			if(i == 2) strcpy(bearerProfile.connStatus.ip, str);
			str = strtok(NULL, ",\"");
		}
		if(!dte->ATResponseOk()) return false;
		this->bearerProfile[cid-1] = bearerProfile;
	}
	else if(cmdType == 4) {
		if(!dte->ATResponseContain(response)) return false;
		for (unsigned char i = 0; i < 6; i++) {
			if(!dte->ATResponse()) return false;
			if(dte->isResponseContain("CONTYPE: ")) {
				const __FlashStringHelper *response = F("CONTYPE: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.contype, str);
			}
			else if (dte->isResponseContain("APN: ")) {
				const __FlashStringHelper *response = F("APN: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.apn, str);
			}
			else if (dte->isResponseContain("USER: ")) {
				const __FlashStringHelper *response = F("USER: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.user, str);
			}
			else if (dte->isResponseContain("PWD: ")) {
				const __FlashStringHelper *response = F("PWD: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.pwd, str);
			}
			else if (dte->isResponseContain("PHONENUM: ")) {
				const __FlashStringHelper *response = F("PHONENUM: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.phonenum, str);
			}
			else if (dte->isResponseContain("RATE: ")) {
				const __FlashStringHelper *response = F("RATE: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				bearerProfile.connParam.rate = str[0] - '0';
			}
		}
		if(!dte->ATResponseOk()) return false;
		this->bearerProfile[cid-1] = bearerProfile;
	}
	else if(!dte->ATResponseOk(10000)) return false;
	return true;
}

/* IP Class */
IP::IP(DTE &dte, GPRS &gprs)
{
	this->dte = &dte;
	this->gprs = &gprs;
}

void IP::setConnectionParamGprs(const char apn[], const char user[], const char pwd[], unsigned char cid) {
	struct ConnParam connParam = this->bearerProfile[cid-1].connParam;
	bool change = false;

	if (strcmp(connParam.apn, apn) != 0) {
		atBearerSettings(3, cid, "APN", apn);
		change = true;
	}
	if (strcmp(connParam.user, user) != 0) {
		atBearerSettings(3, cid, "USER", user);
		change = true;
	}
	if (strcmp(connParam.pwd, pwd) != 0) {
		atBearerSettings(3, cid, "PWD", pwd);
		change = true;
	}
	if (change) getConnectionParam(cid);
}

struct ConnStatus IP::getConnectionStatus(unsigned char cid) {
	atBearerSettings(2, cid);
	return bearerProfile[cid-1].connStatus;
}

struct ConnParam IP::getConnectionParam(unsigned char cid) {
	atBearerSettings(4, cid);
	return bearerProfile[cid-1].connParam;
}

bool IP::openConnection(unsigned char cid) {
	struct ConnStatus connStatus= getConnectionStatus(cid);

	if (connStatus.status == 3) {
		if (!gprs->isAttached()) return false;
		if (!atBearerSettings(1, cid)) return false;
		connStatus = getConnectionStatus(cid);
	}
	if (connStatus.status >= 2) return false;
	return true;
}

bool IP::closeConnection(unsigned char cid) {
	struct ConnStatus connStatus= getConnectionStatus(cid);

	if (connStatus.status == 1) {
		if (!atBearerSettings(0, cid)) return false;
		connStatus = getConnectionStatus(cid);
	}
	if (connStatus.status <= 1) return false;
	return true;
}
